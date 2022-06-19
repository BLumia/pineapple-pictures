// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "metadatadialog.h"

#include <QDialogButtonBox>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHeaderView>

#include "metadatamodel.h"

class PropertyTreeView : public QTreeView
{
public:
    explicit PropertyTreeView(QWidget* parent) : QTreeView(parent) {}
    ~PropertyTreeView() {}

protected:
    void rowsInserted(const QModelIndex& parent, int start, int end) override
    {
        QTreeView::rowsInserted(parent, start, end);
        if (!parent.isValid()) {
            // we are inserting a section group
            for (int row = start; row <= end; ++row) {
                setupSection(row);
            }
        } else {
            // we are inserting a property
            setRowHidden(parent.row(), QModelIndex(), false);
        }
    }

    void reset() override
    {
        QTreeView::reset();
        if (model()) {
            for (int row = 0; row < model()->rowCount(); ++row) {
                setupSection(row);
            }
        }
    }

private:
    void setupSection(int row)
    {
        expand(model()->index(row, 0));
        setFirstColumnSpanned(row, QModelIndex(), true);
        setRowHidden(row, QModelIndex(), !model()->hasChildren(model()->index(row, 0)));
    }
};

class PropertyTreeItemDelegate : public QStyledItemDelegate
{
public:
    PropertyTreeItemDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {}

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyleOptionViewItem opt = option;
        if (!index.parent().isValid()) {
            opt.font.setBold(true);
            opt.features.setFlag(QStyleOptionViewItem::Alternate);
        }
        QStyledItemDelegate::paint(painter, opt, index);
    }
};

MetadataDialog::MetadataDialog(QWidget *parent)
    : QDialog(parent)
    , m_treeView(new PropertyTreeView(this))
{
    m_treeView->setRootIsDecorated(false);
    m_treeView->setIndentation(0);
    m_treeView->setItemDelegate(new PropertyTreeItemDelegate(m_treeView));
    m_treeView->header()->resizeSection(0, sizeHint().width() / 2);

    setWindowTitle(tr("Image Metadata"));

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_treeView);
    layout()->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);

    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

MetadataDialog::~MetadataDialog()
{

}

void MetadataDialog::setMetadataModel(MetadataModel * model)
{
    m_treeView->setModel(model);
}

QSize MetadataDialog::sizeHint() const
{
    return QSize(520, 350);
}
