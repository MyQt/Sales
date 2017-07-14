#include <QApplication>
#include <QPainter>

#include "HHeaderItemDelegate.h"
#include "HHeaderView.h"
#include <QDebug>

HHeaderItemDelegate::HHeaderItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void HHeaderItemDelegate::setHeaderView(HHeaderView *pHeader)
{
    m_pHeaderView = pHeader;
}

//! headerView中绘制的item，根据不同的操作显示不同的效果
void HHeaderItemDelegate::paint (QPainter * painter
                                ,const QStyleOptionViewItem& option
                                ,const QModelIndex &index) const
{

    int row = index.row();
    int col = index.column();

    //const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
    
    QRect smallRect;
    smallRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                    QSize(option.fontMetrics.width(index.data(Qt::DisplayRole).toString()) + 8, option.fontMetrics.height() + 3),
                                    option.rect);
    QStyleOptionHeader header_opt;
    header_opt.rect = option.rect;
    header_opt.text = option.text;
    header_opt.position = QStyleOptionHeader::Middle;
    header_opt.textAlignment = Qt::AlignCenter;

    header_opt.state = option.state;

    if (m_pHeaderView->isItemPress(row, col))
    {
        header_opt.state |= QStyle::State_Sunken; //按钮按下效果
    }

    painter->save();
    QApplication::style()->drawControl(QStyle::CE_Header, &header_opt, painter);
    painter->restore();
    painter->setPen(QColor(255,0,0));
    QStyleOptionViewItemV4 xopt(option);
    xopt.state &= ~QStyle::State_MouseOver;    
    xopt.rect = smallRect;
}


bool HHeaderItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    return false;
}
