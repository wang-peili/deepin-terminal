#include "encodelistview.h"
#include "encodelistmodel.h"
#include "settings.h"
#include "service.h"
#include "termwidget.h"
//#include "encodeitemdelegate.h"

#include <DLog>

#include <QScrollBar>
#include <QStandardItemModel>
#include <QDebug>

EncodeListView::EncodeListView(QWidget *parent) : DListView(parent), m_encodeModel(new EncodeListModel(this))
{
    m_standardModel = new QStandardItemModel;
    m_Mainwindow = qobject_cast<MainWindow *>(parentWidget()->parentWidget()->parentWidget());
    // init view.
    this->setModel(m_standardModel);
    setBackgroundRole(QPalette::NoRole);
    setAutoFillBackground(false);

    setSelectionMode(QListView::NoSelection);
    setVerticalScrollMode(ScrollPerItem);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    /******** Add by nt001000 renfeixiang 2020-05-16:解决Alt+F2显示Encode时，高度变长的问题 Begin***************/
    setViewportMargins(0, 20, 0, 0); //将设置Encode顶上的空白区域放在这里，目前设置大小为20
    /******** Add by nt001000 renfeixiang 2020-05-16:解决Alt+F2显示Encode时，高度变长的问题 Begin***************/

    //add by ameng 设置属性，修复BUG#20074
    setFixedWidth(m_ContentWidth);
    setItemSize(QSize(m_ContentWidth, m_ContentHeight + m_Space));

    /***add by ut001121 zhangmeng 20200521 设置非编辑模式 修复BUG27443***/
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    initEncodeItems();
    update();

    connect(this, &DListView::clicked, this, &EncodeListView::onListViewClicked);
    connect(this, &DListView::activated, this, &QListView::clicked);
    connect(Service::instance(), &Service::checkEncode, this, &EncodeListView::checkEncode);
}

void EncodeListView::initEncodeItems()
{
    QList<QByteArray> encodeDataList = m_encodeModel->listData();
    for (int i = 0; i < encodeDataList.size(); i++) {
        QByteArray encodeData = encodeDataList.at(i);

        QString strEncode = QString(encodeData);
        DStandardItem *item = new DStandardItem;
        item->setText(strEncode);
        item->setCheckable(true);
        m_standardModel->appendRow(item);
    }
    // 默认起动选择第一个。
    m_standardModel->item(0)->setCheckState(Qt::Checked);
}

void EncodeListView::focusOutEvent(QFocusEvent *event)
{
    emit focusOut();

    DListView::focusOutEvent(event);
}

void EncodeListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    DListView::selectionChanged(selected, deselected);
}

void EncodeListView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    DListView::setSelection(rect, command);
}

/*******************************************************************************
 1. @函数:    setLabelStyle
 2. @作者:    m000750 zhangmeng
 3. @日期:    2020-04-23
 4. @说明:    用于移动滚动条位置，修复BUG#20074
*******************************************************************************/
void EncodeListView::showEvent(QShowEvent *e)
{
    static bool moveScrollBar = true;
    if (moveScrollBar == true) {
        moveScrollBar = false;
        QScrollBar *pScrollBar = verticalScrollBar();
        pScrollBar->move(pScrollBar->x() + 5, pScrollBar->y());
    }
    DListView::showEvent(e);
}

void EncodeListView::resizeContents(int width, int height)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
}

QSize EncodeListView::contentsSize() const
{
    return itemSize();
}

void EncodeListView::onListViewClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    qDebug() << "check encode " << index.data().toString();
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(this->model());
    for (int row = 0; row < model->rowCount(); row++) {
        DStandardItem *modelItem = dynamic_cast<DStandardItem *>(model->item(row));
        if (row == index.row()) {
            modelItem->setCheckState(Qt::Checked);
            // 修改配置生效。
            m_Mainwindow->currentPage()->currentTerminal()->selectEncode(index.data().toString());
        } else {
            modelItem->setCheckState(Qt::Unchecked);
        }
    }
}

void EncodeListView::checkEncode(QString encode)
{
    // 判断是否是当前窗口
    if (this->isActiveWindow()) {
        qDebug() << "check encode " << encode;
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(this->model());
        for (int row = 0; row < model->rowCount(); row++) {
            QModelIndex modelindex = model->index(row, 0);
            DStandardItem *modelItem = dynamic_cast<DStandardItem *>(model->item(row));
            if (modelindex.data().toString() == encode) {
                modelItem->setCheckState(Qt::Checked);
                scrollTo(modelindex);
            } else {
                modelItem->setCheckState(Qt::Unchecked);
            }
        }
    }
}
