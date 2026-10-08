// SPDX-FileCopyrightText: 2026 Tad Young <yyc12321@outlook.com>
//
// SPDX-License-Identifier: MIT

#include "winplaylistpatch.h"

#ifdef Q_OS_WIN

#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QRegularExpression>

#include <optional>

#include <exdisp.h>     // IShellWindows, CLSID_ShellWindows
#include <servprov.h>   // IServiceProvider
#include <shlguid.h>    // SID_STopLevelBrowser
#include <shlobj.h>

// YYC MARK:
// We use custom RAII wrappers instead of existing COM smart pointers because the
// project may be built with MinGW, which does not have good compatibility with
// CComPtr and Microsoft's WRL ComPtr (<wrl/client.h>).
// Furthermore, PIDLs are not COM objects (they are freed via ILFree, not Release),
// so no standard smart pointer covers them regardless of compiler.

/// RAII wrapper for COM initialization (CoInitialize / CoUninitialize).
/// Must be declared before any COM objects (e.g., ComPtr, UniquePidl) so that
/// CoUninitialize runs after all COM objects are released during scope exit.
class ComGuard {
public:
    ComGuard() : m_hr(CoInitialize(nullptr)) {}
    ~ComGuard() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
    ComGuard(const ComGuard&) = delete;
    ComGuard& operator=(const ComGuard&) = delete;

    bool IsInitialized() const { return SUCCEEDED(m_hr); }

private:
    HRESULT m_hr;
};

/// RAII wrapper for COM interface pointers.
template <typename T>
class ComPtr {
public:
    ComPtr() = default;
    ComPtr(std::nullptr_t) {}
    ~ComPtr() { Reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    T* operator->() const { return m_ptr; }
    T* Get() const { return m_ptr; }
    T** GetAddressOf() { return &m_ptr; }
    explicit operator bool() const { return m_ptr != nullptr; }
    void Reset() { if (m_ptr) { m_ptr->Release(); m_ptr = nullptr; } }

private:
    T* m_ptr = nullptr;
};

/// RAII wrapper for PIDL (Item ID List).
class UniquePidl {
public:
    UniquePidl() = default;
    explicit UniquePidl(LPITEMIDLIST pidl) : m_pidl(pidl) {}
    ~UniquePidl() { Reset(); }
    UniquePidl(const UniquePidl&) = delete;
    UniquePidl& operator=(const UniquePidl&) = delete;
    UniquePidl(UniquePidl&& other) noexcept : m_pidl(other.m_pidl) { other.m_pidl = nullptr; }
    UniquePidl& operator=(UniquePidl&& other) noexcept {
        if (this != &other) {
            Reset();
            m_pidl = other.m_pidl;
            other.m_pidl = nullptr;
        }
        return *this;
    }

    LPCITEMIDLIST Get() const { return m_pidl; }
    LPITEMIDLIST* GetAddressOf() { return &m_pidl; }
    explicit operator bool() const { return m_pidl != nullptr; }
    void Reset() { if (m_pidl) { ILFree(m_pidl); m_pidl = nullptr; } }

private:
    LPITEMIDLIST m_pidl = nullptr;
};

/// Outputs a diagnostic warning for debugging Windows Explorer integration failures.
static void ReportError(const QString& detail)
{
    qWarning("Windows Explorer file list fetch error: %s",
             qPrintable(detail));
}

/// Normalizes a Windows directory path for case-insensitive comparison:
/// converts to native separators, lowercases, and ensures a trailing backslash.
static QString NormalizePath(const QString& path)
{
    QString result = QDir::toNativeSeparators(path).toLower();
    if (!result.endsWith(QChar('\\'))) {
        result += QChar('\\');
    }
    return result;
}

/// Checks whether the Explorer window represented by the given IDispatch is
/// displaying the target folder. Returns its IFolderView2 on match, or null.
static ComPtr<IFolderView2> TryMatchWindow(IDispatch* dispatch, const QString& normalizedTarget)
{
    // Navigate: IDispatch -> IServiceProvider -> IShellBrowser.
    ComPtr<IServiceProvider> serviceProvider;
    if (FAILED(dispatch->QueryInterface(IID_PPV_ARGS(serviceProvider.GetAddressOf())))) {
        return nullptr;
    }

    ComPtr<IShellBrowser> shellBrowser;
    if (FAILED(serviceProvider->QueryService(SID_STopLevelBrowser,
            IID_PPV_ARGS(shellBrowser.GetAddressOf())))) {
        return nullptr;
    }

    // IShellBrowser -> active IShellView.
    ComPtr<IShellView> shellView;
    if (FAILED(shellBrowser->QueryActiveShellView(shellView.GetAddressOf()))) {
        return nullptr;
    }

    // Retrieve the folder's path via IPersistIDList.
    ComPtr<IPersistIDList> persistIdList;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(persistIdList.GetAddressOf())))) {
        return nullptr;
    }

    UniquePidl folderPidl;
    if (FAILED(persistIdList->GetIDList(folderPidl.GetAddressOf()))) {
        return nullptr;
    }

    PWSTR pathStr = nullptr;
    if (FAILED(SHGetNameFromIDList(folderPidl.Get(), SIGDN_DESKTOPABSOLUTEPARSING, &pathStr))) {
        return nullptr;
    }

    QString windowPath = NormalizePath(QString::fromWCharArray(pathStr));
    CoTaskMemFree(pathStr);

    if (windowPath != normalizedTarget) {
        return nullptr;
    }

    // Path matches — query IFolderView2 from the ShellView.
    ComPtr<IFolderView2> folderView2;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView2.GetAddressOf())))) {
        return nullptr;
    }
    return folderView2;
}

/// Tries to find a live Explorer window that is currently displaying the given
/// folder path. Returns the IFolderView2 from that window, or an empty ComPtr.
///
/// Explorer's sort order is stored only in the running Explorer process's memory.
/// Creating a new shell view via IShellFolder::CreateViewObject yields a default
/// sort (always by name), so we must find the actual live IFolderView2 instead.
///
/// Reference:
/// - https://www.stephenli.ca/getting-windows-explorers-current-directory-sort-order
/// - https://github.com/d2phap/ImageGlass/blob/75a296287f338edcf409eee3917531b25ff5d927/v9/Components/ImageGlass.Base/FileSystem/FileFinder.cs
/// - NuGet Package: D2Phap.EggShell
static ComPtr<IFolderView2> FindLiveFolderView(const QString& targetFolderPath)
{
    // Create the Shell windows collection (all open Explorer windows).
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
            IID_PPV_ARGS(shellWindows.GetAddressOf())))) {
        ReportError("Failed to create IShellWindows instance");
        return nullptr;
    }

    QString normalizedTarget = NormalizePath(targetFolderPath);

    // 1. Try the Desktop window first — it may contain the file the user opened.
    {
        VARIANT varLoc;
        VariantInit(&varLoc);
        varLoc.vt = VT_I4;
        varLoc.lVal = 0;

        VARIANT varLocRoot;
        VariantInit(&varLocRoot);

        long desktopHwnd = 0;
        ComPtr<IDispatch> desktopDispatch;
        HRESULT findHr = shellWindows->FindWindowSW(
            &varLoc, &varLocRoot, SWC_DESKTOP,
            &desktopHwnd, SWFO_NEEDDISPATCH,
            desktopDispatch.GetAddressOf());

        VariantClear(&varLoc);
        VariantClear(&varLocRoot);

        if (SUCCEEDED(findHr) && desktopDispatch) {
            ComPtr<IFolderView2> result = TryMatchWindow(desktopDispatch.Get(), normalizedTarget);
            if (result) return result;
        }
    }

    // 2. Iterate regular Explorer windows.
    LONG count = 0;
    shellWindows->get_Count(&count);

    for (LONG i = 0; i < count; i++) {
        VARIANT varIndex;
        VariantInit(&varIndex);
        varIndex.vt = VT_I4;
        varIndex.lVal = i;

        ComPtr<IDispatch> dispatch;
        HRESULT itemHr = shellWindows->Item(varIndex, dispatch.GetAddressOf());
        VariantClear(&varIndex);

        if (FAILED(itemHr) || !dispatch) {
            continue;
        }

        ComPtr<IFolderView2> result = TryMatchWindow(dispatch.Get(), normalizedTarget);
        if (result) return result;
    }

    return nullptr;
}

bool WinPlaylistPatch::loadPlaylist(
    const QUrl &url, const QStringList &allowedSuffixes, QList<QUrl> &playlist,
    std::optional<qsizetype> &indexHint) {

    QFileInfo info(url.toLocalFile());
    QString folderPath = info.path();
    QString currentFileName = info.fileName();

    // Pre-compile wildcard suffix filters into regular expressions for performance.
    QList<QRegularExpression> nameRegexps;
    for (const QString &pattern : allowedSuffixes) {
        nameRegexps.append(QRegularExpression(
            QRegularExpression::wildcardToRegularExpression(pattern),
            QRegularExpression::CaseInsensitiveOption));
    }

    // Initialize COM. The ComGuard must be declared before any COM objects
    // below so that CoUninitialize runs after all of them are released.
    ComGuard comGuard;
    if (!comGuard.IsInitialized()) {
        ReportError("COM initialization failed");
        return false;
    }

    // Find a live Explorer window that is currently displaying the target folder.
    ComPtr<IFolderView2> folderView2 = FindLiveFolderView(folderPath);
    if (!folderView2) {
        ReportError("No open Explorer window matches the folder: " + folderPath);
        return false;
    }

    // Get items already sorted in Explorer's view order, as an IShellItemArray.
    // SVGIO_ALLVIEW returns all items; SVGIO_FLAG_VIEWORDER respects the view's sort.
    ComPtr<IShellItemArray> itemArray;
    if (FAILED(folderView2->Items(SVGIO_ALLVIEW | SVGIO_FLAG_VIEWORDER,
            IID_PPV_ARGS(itemArray.GetAddressOf())))) {
        ReportError("IFolderView2::Items(SVGIO_ALLVIEW | SVGIO_FLAG_VIEWORDER) failed");
        return false;
    }

    DWORD itemCount = 0;
    itemArray->GetCount(&itemCount);

    // Enumerate sorted items, filter by pattern, and build the playlist.
    for (DWORD i = 0; i < itemCount; i++) {
        ComPtr<IShellItem> shellItem;
        if (FAILED(itemArray->GetItemAt(i, shellItem.GetAddressOf()))) {
            continue;
        }

        // Get the full parsing path of the item.
        PWSTR pathStr = nullptr;
        if (FAILED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pathStr)) || !pathStr) {
            continue;
        }
        QString filePath = QString::fromWCharArray(pathStr);
        CoTaskMemFree(pathStr);

        // Skip directories — the view may include folders alongside files.
        QFileInfo fileInfo(filePath);
        if (!fileInfo.isFile()) {
            continue;
        }

        QString fileName = fileInfo.fileName();

        // Match against compiled wildcard patterns.
        bool matches = nameRegexps.isEmpty();
        for (const QRegularExpression &re : std::as_const(nameRegexps)) {
            if (re.match(fileName).hasMatch()) {
                matches = true;
                break;
            }
        }
        if (!matches) {
            continue;
        }

        playlist.append(QUrl::fromLocalFile(filePath));
        if (fileName == currentFileName) {
            indexHint = playlist.size() - 1;
        }
    }

    return true;
}

#endif // Q_OS_WIN
