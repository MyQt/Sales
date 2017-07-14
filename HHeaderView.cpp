#include <QPainter>
#include <QMouseEvent>
#include <QIcon>
#include <QCursor>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "HHeaderView.h"
#include "HHeaderItemDelegate.h"
#include "HHeaderModel.h"
#pragma execution_character_set("utf-8")

HHeaderView::HHeaderView(Qt::Orientation orientation, QWidget * parent) : QHeaderView(orientation, parent),m_bQuit(false)
{
    //! 设置代理，由代理进行画单元格
    m_pItemDelegate = new HHeaderItemDelegate();
    m_pItemDelegate->setHeaderView(this);
    setItemDelegate(m_pItemDelegate);

    connect(this, SIGNAL(sectionResized(int,int,int)), this, SLOT(onSectionResized(int,int,int)));
    //! 交互式，代表支持鼠标拖动
    setSectionResizeMode(QHeaderView::Stretch);
    setCascadingSectionResizes(true);

    this->setOffset(0);    //! 这将会影响item绘制的位置
    setSectionsClickable(true);
    setHighlightSections(false);
    setMouseTracking(false);

    setAttribute(Qt::WA_Hover, false);
}

HHeaderView::~HHeaderView()
{
    if(m_pItemDelegate)
    {
        delete m_pItemDelegate;
        m_pItemDelegate = NULL;
    }
}

void HHeaderView::initHeaderView(HHeaderModel *pModel)
{
    //! 设置各个独立单元格Item
    pModel->setItem(0,0, "编号");
    pModel->setItem(0,1, "品种");
    pModel->setItem(0,2, "明细码");
    pModel->setItem(0,7, "件数");
    pModel->setItem(0,8, "数量");
    pModel->setItem(0,9, "单价");
    pModel->setItem(0,10, "金额");

    pModel->setItem(1,2, "");
    pModel->setItem(1,3, "");
    pModel->setItem(1,4, "");
    pModel->setItem(1,5, "");
    pModel->setItem(1,6, "");

    //! 设置合并单元格
    pModel->setSpan(0,2,1,5);
    pModel->takeVerticalHeaderItem(2);
    pModel->takeVerticalHeaderItem(3);
    pModel->takeVerticalHeaderItem(4);
    pModel->takeVerticalHeaderItem(5);
    pModel->takeVerticalHeaderItem(6);
}

void HHeaderView::setSectionSize(HHeaderModel *pModel, int width)
{
    int nColWdth = width / pModel->getColumnCount();
    setDefaultSectionSize(nColWdth);
}

void HHeaderView::setQuit()
{
    m_bQuit = true;
}
 
int HHeaderView::sectionSizes(int lIndex, int sectionCount)
{
    int width = 0;
    for(int i = lIndex; i < lIndex + sectionCount; ++i)
    {
        width += sectionSize(i);
    }

    return width;
}

QSize HHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    //return QHeaderView::sectionSizeFromContents(logicalIndex);
    QSize size;
    int maxWidth = 0;

    HHeaderModel* model = static_cast<HHeaderModel*> (this->model());
    for(int i = 0; i < model->rowCount(QModelIndex()); ++i)
    {
        QFont fnt;
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        fnt.setBold(true);
        opt.text = model->item(i, logicalIndex);
        opt.fontMetrics = QFontMetrics(fnt);
        size = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);
        maxWidth = qMax(maxWidth, size.width());
    }
    //! 主要是设置高度，宽度这里设指了没有用，
    //! 需要由headerview的父窗体里设置
    return QSize(maxWidth+4,size.height()*model->rowCount(QModelIndex()));
}
/**
 ** 这里实质上没有绘制任何item,而是调用delegate的paint函数
 ** 进行绘制item的
*/
void HHeaderView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(viewport());
    QMultiMap<int, int> rowSpanList;

    int cnt = this->count ();
    int curRow, curCol;
    HHeaderModel* model = static_cast<HHeaderModel*> (this->model());
    for(int row = 0; row < model->rowCount(QModelIndex()); ++row)
    {
        for(int col = 0; col < model->columnCount(QModelIndex()); ++col)
        {
            curRow = row;
            curCol = col;

            //! 设置style
            QStyleOptionViewItemV4 opt = viewOptions();
            QStyleOptionHeader header_opt;
            initStyleOption(&header_opt);

            header_opt.textAlignment = Qt::AlignCenter;
            QFont fnt;
            fnt.setBold(true);
            header_opt.fontMetrics = QFontMetrics(fnt);
            opt.fontMetrics = QFontMetrics(fnt);
            QSize size = style()->sizeFromContents(QStyle::CT_HeaderSection, &header_opt, QSize(), this);
            header_opt.position = QStyleOptionHeader::Middle;

            //! 判断当前行是否处于鼠标悬停状态
            if(m_hoverIndex == model->index(row, col, QModelIndex()))
            {
                header_opt.state |= QStyle::State_MouseOver;
               // header_opt.state |= QStyle::State_Active;
            }

            //! 获取某个cell要显示的文本
            opt.text = model->item(row, col);
            header_opt.text = model->item(row, col);
          
            //! 获取某个cell所在的合并单元格
            CellSpan span = model->getSpan(row, col);

            //! 本单元格横跨几行，几列？
            int rowSpan = span.nRowSpan;
            int columnSpan = span.nColSpan;

            if(columnSpan > 1 && rowSpan > 1)
            {
                //! 单元格跨越多列和多行, 不支持，改为多行1列
                continue;
                /*header_opt.rect = QRect(sectionViewportPosition(logicalIndex(col)), row * size.height(), sectionSizes(col, columnSpan), rowSpan * size.height());
                opt.rect = header_opt.rect;
                col += columnSpan - 1; */
            }
            else if(columnSpan > 1)
            {
                //! 单元格跨越多列
                header_opt.rect = QRect(sectionViewportPosition(logicalIndex(col))
                                        ,row*size.height()
                                        ,sectionSizes(col,columnSpan)
                                        ,size.height());

                opt.rect = header_opt.rect;
                col += columnSpan - 1;
            }
            else if(rowSpan > 1)
            {
                //! 单元格跨越多行
                header_opt.rect = QRect(sectionViewportPosition(logicalIndex(col))
                                        ,row*size.height()
                                        ,sectionSize(logicalIndex(col))
                                        ,size.height()*rowSpan);

                opt.rect = header_opt.rect;
                //! rect 遍历到合并单元格中的左上角时，整个合并单元格已经形成
                //! 合并单元格的其他小格子，就不需要绘制了
                //! rowSpanList中记录的就是不需要绘制的小格子了
                for(int i = row + 1; i <= rowSpan - 1; ++i)
                {
                    rowSpanList.insert(i, col);
                }
            }
            else
            {
                //! 正常的单元格
                header_opt.rect = QRect(sectionViewportPosition(logicalIndex(col))
                                        ,row * size.height()
                                        ,sectionSize(logicalIndex(col))
                                        ,size.height());

                opt.rect = header_opt.rect;
            }

            opt.state = header_opt.state;
            QMultiMap<int, int>::iterator it = rowSpanList.find(curRow, curCol);
            if(it == rowSpanList.end())
            {
                //! 保存当前item的矩形
                m_itemRectMap[curRow][curCol] = header_opt.rect;
                m_pItemDelegate->paint(&painter
                                      ,opt
                                      ,model->index(curRow, curCol, QModelIndex()));
            }
        }
    }
}

void HHeaderView::onSectionResized(int logicalIndex, int oldSize, int newSize)
{
    if (0 == newSize)
    {
        //过滤掉隐藏列导致的resize
        viewport()->update();
        return;
    }

    static bool selfEmitFlag = false;
    if (selfEmitFlag)
    {
        return;
    }

    int minWidth = 99999;
    QFontMetrics metrics(this->font());
    //获取这列上最小的字体宽度，移动的长度不能大于最小的字体宽度
    HHeaderModel* model = static_cast<HHeaderModel*> (this->model());
    for(int i = 0; i < model->rowCount(QModelIndex()); ++i)
    {
        QString text =  model->item(i, logicalIndex);
        if (text.isEmpty())
            continue;

        int textWidth = metrics.width(text);
        if (minWidth > textWidth)
        {
            minWidth = textWidth;
        }
    }

    if (newSize < minWidth)
    {
        selfEmitFlag = true;
        this->resizeSection(logicalIndex, oldSize);
        selfEmitFlag = false;
    }

    viewport()->update();
}
/*
int HHeaderView::sectionSizeHint(int logicalIndex)
{
    if (logicalIndex == 0)
    {
        return 10;
    }
}
*/
 //算出当前鼠标位置所在的index
QModelIndex HHeaderView::indexAt(const QPoint& pos) const
{
    int x = pos.x();
    int y = pos.y();

    QMap<int, QRect>::const_iterator iterCol;
    QMap<int, QMap<int, QRect> >::const_iterator iterRow = m_itemRectMap.begin();
    for (; iterRow != m_itemRectMap.end(); ++iterRow)
    {
        for (iterCol = iterRow.value().begin(); iterCol != iterRow.value().end(); ++iterCol)
        {
            if(x > iterCol.value().x() && x < iterCol.value().x() + iterCol.value().width() &&
                y > iterCol.value().y() && y < iterCol.value().y() + iterCol.value().height())
            {
                return model()->index(iterRow.key(), iterCol.key(), QModelIndex());
            }
        }
    }

    return QModelIndex();
}

void HHeaderView::mouseMoveEvent(QMouseEvent *event)
{
    QHeaderView::mouseMoveEvent(event);
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
    {
        QHeaderView::mouseMoveEvent(event);
        return;
    }

    //保存当前hover的index
    m_hoverIndex = index;
    
    viewport()->update();
    QHeaderView::mouseMoveEvent(event);
}

void HHeaderView::mousePressEvent ( QMouseEvent * event )
{
    m_pressIndex = indexAt(event->pos());
    viewport()->update();
    QHeaderView::mousePressEvent(event);
}

void HHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressIndex = QModelIndex();

    int row = indexAt(event->pos()).row();
    int col = indexAt(event->pos()).column();  
    //需要更新界面
    viewport()->update();

    QHeaderView::mouseReleaseEvent(event);
}

bool HHeaderView::event(QEvent* event )
{
    if (event->type() == QEvent::Leave)
    {//处理离开headerview时，取消hover状态
        m_hoverIndex = QModelIndex();
        viewport()->update(); //这里需要update，要不界面不更新
    }

    return true;
}

bool HHeaderView::isItemPress(int row, int col)
{
    if (!m_pressIndex.isValid())
        return false;

    if (row == m_pressIndex.row() && col == m_pressIndex.column())
        return true;

    return false;
}
