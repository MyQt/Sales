#include "BillsView.h"
#include "billmodel.h"
#include "BillWidgetDelegate.h"
#include "HHeaderView.h"
#include "HHeaderModel.h"

#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QAbstractItemView>
#include <QPaintEvent>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QScrollBar>

#pragma execution_character_set("utf-8")

BillsView::BillsView(QWidget *parent)
    : QTableView( parent ),
      m_isSearching(false),
      m_isMousePressed(false)
{

    QTimer::singleShot(0, this, SLOT(init()));

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

BillsView::~BillsView()
{
    QAbstractItemModel *pModel = model();
    delete pModel;
    pModel = NULL;
} 

void BillsView::initHeaderView()
{
   //! 初始化表格，这个表格我只自定义横向表头，纵向不管
   HHeaderView *pHeadView = new HHeaderView(Qt::Horizontal);
   HHeaderModel *pHeadModel = new HHeaderModel();
   pHeadView->initHeaderView(pHeadModel);
   pHeadView->setModel(pHeadModel);
   setHorizontalHeader(pHeadView);

   pHeadView->setSectionSize(pHeadModel, width());
   int colWidth = width()/pHeadModel->getColumnCount();
   setColumnWidth(0, colWidth);
   setColumnWidth(1, colWidth);
   setColumnWidth(2, colWidth);
   setColumnWidth(3, colWidth/2);
   setColumnWidth(4, colWidth/2);
   setColumnWidth(5, colWidth/2);
   setColumnWidth(6, colWidth/2);
   setColumnWidth(7, colWidth);
   setColumnWidth(8, colWidth);
   setColumnWidth(9, colWidth);
   setColumnWidth(10, colWidth);
   setColumnWidth(11, colWidth);
   setColumnWidth(12, colWidth*3);
//   resizeColumnsToContents();
   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   QTableView::verticalHeader()->hide();
}

//void BillsView::animateAddedRow(const QModelIndex& parent, int start, int end)
//{
//    Q_UNUSED(parent)
//    Q_UNUSED(end)

//    QModelIndex idx = model()->index(start,0);
//    // Bill: this line add flikering, seen when the animation runs slow
//    selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);

//    BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//    if(delegate != Q_NULLPTR){
//        delegate->setState( BillWidgetDelegate::Insert, idx);

//        // TODO find a way to finish this function till the animation stops
//        while(delegate->animationState() == QTimeLine::Running){
//            qApp->processEvents();
//        }
//    }
//}

/**
 * @brief Reimplemented from QWidget::paintEvent()
 */
void BillsView::paintEvent(QPaintEvent *e)
{
//    BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//    if(delegate != Q_NULLPTR)
//        delegate->setCurrentSelectedIndex(currentIndex());

    QTableView::paintEvent(e);
}

/**
 * @brief Reimplemented from QAbstractItemView::rowsInserted().
 */
void BillsView::rowsInserted(const QModelIndex &parent, int start, int end)
{
//    if (start == end)
//        animateAddedRow(parent, start, end);
    QTableView::rowsInserted(parent, start, end);
}

/**
 * @brief Reimplemented from QAbstractItemView::rowsAboutToBeRemoved().
 */
void BillsView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    if(start == end)/*{
        BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
        if(delegate != Q_NULLPTR){
            QModelIndex idx = model()->index(start,0);
            delegate->setCurrentSelectedIndex(QModelIndex());

            if(!m_isSearching){
                delegate->setState( BillWidgetDelegate::Remove, idx);
            }else{
                delegate->setState( BillWidgetDelegate::Normal, idx);
            }

            // TODO find a way to finish this function till the animation stops
            while(delegate->animationState() == QTimeLine::Running){
                qApp->processEvents();
            }
        }
    }*/

    QTableView::rowsAboutToBeRemoved(parent, start, end);
}

void BillsView::rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                  const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destinationParent)
    Q_UNUSED(destinationRow)

//    if(model() != Q_NULLPTR){
//        QModelIndex idx = model()->index(sourceStart,0);
//        BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//        if(delegate != Q_NULLPTR){
//            delegate->setState( BillWidgetDelegate::MoveOut, idx);

//            // TODO find a way to finish this function till the animation stops
//            while(delegate->animationState() == QTimeLine::Running){
//                qApp->processEvents();
//            }
//        }
//    }
}

void BillsView::rowsMoved(const QModelIndex &parent, int start, int end,
                         const QModelIndex &destination, int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(destination)

    QModelIndex idx = model()->index(row,0);
    setCurrentIndex(idx);

//    BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//    if(delegate == Q_NULLPTR)
//        return;

//    delegate->setState( BillWidgetDelegate::MoveIn, idx );

//    // TODO find a way to finish this function till the animation stops
//    while(delegate->animationState() == QTimeLine::Running){
//        qApp->processEvents();
//    }
}

void BillsView::init()
{
    setMouseTracking(true);
    setUpdatesEnabled(true);
    viewport()->setAttribute(Qt::WA_Hover);

    setupStyleSheet();
    setupSignalsSlots();
}

void BillsView::mouseMoveEvent(QMouseEvent*e)
{
    if(!m_isMousePressed)
        QTableView::mouseMoveEvent(e);
}

void BillsView::mousePressEvent(QMouseEvent*e)
{
    m_isMousePressed = true;
    QTableView::mousePressEvent(e);
}

void BillsView::mouseReleaseEvent(QMouseEvent*e)
{
    m_isMousePressed = false;
    QTableView::mouseReleaseEvent(e);
}

bool BillsView::viewportEvent(QEvent*e)
{
    if(model() != Q_NULLPTR){
        switch (e->type()) {
        case QEvent::Leave:{
//            QPoint pt = mapFromGlobal(QCursor::pos());
//            QModelIndex index = indexAt(QPoint(10, pt.y()));
//            if(index.row() > 0){
//                index = model()->index(index.row()-1, 0);
//                BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//                if(delegate != Q_NULLPTR){
//                    delegate->setHoveredIndex(QModelIndex());
//                    viewport()->update(visualRect(index));
//                }
//            }
            break;
        }
        case QEvent::MouseButtonPress:{
            QPoint pt = mapFromGlobal(QCursor::pos());
            QModelIndex index = indexAt(QPoint(10, pt.y()));
            if(!index.isValid())
                emit viewportPressed();

            break;
        }
        default:
            break;
        }
    }

    return QTableView::viewportEvent(e);
}

void BillsView::setCurrentRowActive(bool isActive)
{
//    BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//    if(delegate == Q_NULLPTR)
//        return;

//    delegate->setActive(isActive);
    viewport()->update(visualRect(currentIndex()));
}

void BillsView::setSearching(bool isSearching)
{
    m_isSearching = isSearching;
}

void BillsView::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(model() != Q_NULLPTR){
        if(current.row() < previous.row()){
            if(current.row() > 0){
                QModelIndex prevIndex = model()->index(current.row()-1, 0);
                viewport()->update(visualRect(prevIndex));
            }
        }

        if(current.row() > 1){
            QModelIndex prevPrevIndex = model()->index(current.row()-2, 0);
            viewport()->update(visualRect(prevPrevIndex));
        }
    }
}

void BillsView::onEntered(QModelIndex index)
{
//    if(model() != Q_NULLPTR){
//        if(index.row() > 1){
//            QModelIndex prevPrevIndex = model()->index(index.row()-2, 0);
//            viewport()->update(visualRect(prevPrevIndex));

//            QModelIndex prevIndex = model()->index(index.row()-1, 0);
//            viewport()->update(visualRect(prevIndex));

//        }else if(index.row() > 0){
//            QModelIndex prevIndex = model()->index(index.row()-1, 0);
//            viewport()->update(visualRect(prevIndex));
//        }

//        BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate *>(itemDelegate());
//        if(delegate != Q_NULLPTR)
//            delegate->setHoveredIndex(index);
//    }
}

void BillsView::onViewportEntered()
{
    if(model() != Q_NULLPTR && model()->rowCount() > 1){
//        BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate *>(itemDelegate());
//        if(delegate != Q_NULLPTR)
//            delegate->setHoveredIndex(QModelIndex());

        QModelIndex lastIndex = model()->index(model()->rowCount()-2, 0);
        viewport()->update(visualRect(lastIndex));
    }
}

void BillsView::onRangeChanged(int min, int max)
{
    Q_UNUSED(min)

//    BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate*>(itemDelegate());
//    if(delegate != Q_NULLPTR){
//        if(max > 0){
//            delegate->setRowRightOffset(2);
//        }else{
//            delegate->setRowRightOffset(0);
//        }
//        viewport()->update();
//    }
}

void BillsView::setupSignalsSlots()
{
    // remove/add separator
    // current selectected row changed
    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &BillsView::onCurrentRowChanged);

    // row was entered
    connect(this, &BillsView::entered, this, &BillsView::onEntered);

    // viewport was entered
    connect(this, &BillsView::viewportEntered, this, &BillsView::onViewportEntered);


    // remove/add offset right side
    connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this, &BillsView::onRangeChanged);
}

/**
 * @brief setup styleSheet
 */
void BillsView::setupStyleSheet()
{
    QString ss = QString("QTableView {background-color: rgb(255, 255, 255);} "
                         "QTableView::item{selection-background-color:rgb(23,165,230)}"
//                         "QScrollBar {margin-right: 2px; background: transparent;} "
//                         "QScrollBar:hover { background-color: rgb(217, 217, 217);}"
//                         "QScrollBar:handle:vertical:hover { background: rgb(170, 170, 171); } "
//                         "QScrollBar:handle:vertical:pressed { background: rgb(149, 149, 149);}"
//                         "QScrollBar:vertical { border: none; width: 10px; border-radius: 4px;} "
//                         "QScrollBar::handle:vertical { border-radius: 4px; background: rgb(188, 188, 188); min-height: 20px; }  "
//                         "QScrollBar::add-line:vertical { height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }  "
//                         "QScrollBar::sub-line:vertical { height: 0px; subcontrol-position: top; subcontrol-origin: margin; }"
                         );

    setStyleSheet(ss);
}

// 计算行页数，及每页起始行号及区域高度
void BillsView::CalcultPrintAreaRow(int h, int &pageCount, QVector<int> &printAreaStartRow, QVector<int> &printAreaHeight)
{
    int pix_height = 0;
    BillModel *item_model = (BillModel*)this->model();
    if (item_model == NULL) return;

    HHeaderView *headerView = (HHeaderView*)this->horizontalHeader ();
    if (headerView != NULL)    pix_height = headerView->height();

    pageCount = 0;

    int i;

    printAreaStartRow.push_back(0);
    for (i = 0; i < item_model->rowCount(); ) {
        if (pix_height + rowHeight(i) > h) { // 到当前行时，已经到了页尾
            printAreaStartRow.push_back(i); // 记录行号
            printAreaHeight.push_back(pix_height); // 记录总高度

            pix_height = headerView->height();

            pageCount ++; // 页数加1

            continue;
        }

        pix_height += rowHeight(i); // 加上当前行高

        i++;

    }

    if (pix_height <= h) { // 最后一页
        printAreaStartRow.push_back(i);
        printAreaHeight.push_back(pix_height);

        pageCount ++;
    }

}
// 计算列页数及每页起始列号及打印区域宽度
void BillsView::CalcultPrintAreaCol(int w, int &pageCount, QVector<int> &printAreaStartCol, QVector<int> &printAreaWidth)
{
    int pix_width = 0;
    BillModel *item_model = (BillModel*)this->model();
    if (item_model == NULL) return;

    HHeaderView *headerView = (HHeaderView*)this->verticalHeader ();
    if (headerView != NULL)    pix_width = headerView->width();

    pageCount = 0;

    int i;
    printAreaStartCol.push_back(0);
    for (i = 0; i < item_model->columnCount() && !isColumnHidden(i); ) {
        if (pix_width + columnWidth(i) > w) { // 宽度超打印页
            printAreaStartCol.push_back(i);
            printAreaWidth.push_back(pix_width);

            pix_width = headerView->width();

            pageCount ++;

            continue;
        }

        pix_width += columnWidth(i);

        i++;
    }

    if (pix_width <= w) {
        printAreaStartCol.push_back(i);
        printAreaWidth.push_back(pix_width);

        pageCount ++;
    }
}

// 计算整个页面宽度
int BillsView::CalcultTableViewWidth()
{
    int pix_width = 0;

    BillModel *item_model = (BillModel*)this->model();
    if (item_model == NULL) return 0;

    HHeaderView *headerView = (HHeaderView*)this->verticalHeader ();
    if (headerView != NULL)    pix_width = headerView->width();

    for (int i = 0; i < item_model->columnCount()&&!isColumnHidden(i); i++) {
        pix_width += columnWidth(i);
    }

    return pix_width;
}
// 计算整个页面高度
int BillsView::CalcultTableViewHeight()
{
    int pix_height = 0;

    BillModel *item_model = (BillModel *)this->model();
    if (item_model == NULL) return 0;

    HHeaderView *headerView = (HHeaderView *)this->horizontalHeader ();
    if (headerView != NULL)    pix_height = headerView->height();

    for (int i = 0; i < item_model->rowCount(); i++) {
        pix_height += rowHeight(i);
    }

    return pix_height;
}


void BillsView::prints(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    // 设置页眉，页脚
    QFont titleFont;
    QFont headerFont;
    QFont font;
    QFontMetrics *titleFmt;
    QFontMetrics *headerFmt;
    QFontMetrics *fmt;
    //title font
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleFmt =new QFontMetrics(titleFont);


    //header font
    headerFont.setBold(true);
    headerFont.setPointSize(11);
    headerFmt =new QFontMetrics(headerFont);

    QString titleText("金花布业销售单");
    int titleWidth = titleFmt->width(titleText);
    QString headerText;
    QString footerText;
    QString pageText;
    QString date=QDate::currentDate().toString(QLocale().dateFormat());
    QString time=QTime::currentTime().toString(QLocale().timeFormat(QLocale::ShortFormat));
    headerText = m_customer +"     ";
    headerText.append("地址: 商丘市梁园区海淀路9号金花布业     ");
    headerText.append("电话: 13939605733     ");
    headerText.append("日期: ");
    headerText.append(date);
    int headerWidth = headerFmt->width(headerText);
    footerText="制单人: ";
    footerText.append(m_maker).append("     ");
    footerText.append("司机: ").append(m_driver).append("     ");
    footerText.append("银行卡号: 6666666666666666666");

    int footerWidth = headerFmt->width(footerText);
    QRect rectOrig = geometry();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    int marginLeft = 20, marginTop = 100, marginRight = 20, marginBottom = 100;

    QRect paperRect = printer->pageRect();
    paperRect.adjust(0, 0, -marginLeft - marginRight, -marginTop - marginBottom);
    resize(paperRect.width(), paperRect.height());

    int i,j,k;
    int row_pageCount = 1, col_pageCount = 1;
    QVector<int> printAreaStartRow, printAreaHeight;
    QVector<int> printAreaStartCol, printAreaWidth;

    CalcultPrintAreaRow(paperRect.height(), row_pageCount, printAreaStartRow, printAreaHeight);
    CalcultPrintAreaCol(paperRect.width(),  col_pageCount, printAreaStartCol, printAreaWidth);

    //限制打印页数, 处理实际打印页(根据需要自己处理，判断有效打印区域)
//    CalcultTableViewPage(printAreaStartRow, printAreaStartCol, row_pageCount, col_pageCount);

    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.begin(printer);
  //隐藏行列很重要，慢慢体会吧

    for (i = 0; i < row_pageCount; i ++) {

        for (k = 0; k < this->model()->columnCount() && k < 12; k++) setColumnHidden(k, false);     //重新把列显示出来

        for (j = 0; j < col_pageCount; j++) {
            resize(printAreaWidth[j], printAreaHeight[i]);
            QString pageCur = i;
            QString pageEnd = row_pageCount;
            pageText = "页数: ";
            pageText.append(QString::number(i+1));
            pageText.append(" / ").append(QString::number(row_pageCount));
            int pageWidth = headerFmt->width(pageText);
            painter.drawText(QRectF(printer->pageRect().width()/2-titleWidth/2, marginTop/4, titleWidth, titleFmt->height()), titleText);
            painter.drawText(QRectF(marginLeft, marginTop-headerFmt->height()-2, headerWidth, headerFmt->height()), headerText);
            painter.drawText(QRectF(marginLeft, marginTop+printAreaHeight[i]+3, footerWidth, headerFmt->height()), footerText);
            painter.drawText(QRectF(marginLeft, printer->pageRect().height()-headerFmt->height()-2, pageWidth, headerFmt->height()), pageText);
            if (i > 0 || col_pageCount == 1 && !printer->fullPage())
            {
                painter.drawLine(QPointF(marginLeft, marginTop+printAreaHeight[i]+1), QPointF(marginLeft+printAreaWidth[j], marginTop+printAreaHeight[i]+1));
            }
            render(&painter, QPoint(marginLeft, marginTop), QRegion(0, 0, printAreaWidth[j], printAreaHeight[i]));            //绘制到指定区域
            //隐藏列
            for (k =  printAreaStartCol[j]; k < printAreaStartCol[j + 1]; k++) setColumnHidden(k, true);

            if (i != (row_pageCount - 1) ||
                j != (col_pageCount - 1))
                    printer->newPage();  //创建新页
        }

        //隐藏行
        for (k = printAreaStartRow[i]; k < printAreaStartRow[i + 1]; k++) setRowHidden(k, true);
    }

    painter.end();
    setGeometry(rectOrig);
    for(int m = 0; m < model()->rowCount(); m++)
    {
        setRowHidden(m, false);

    }
    for (k = 0; k < this->model()->columnCount() && k < 12; k++) setColumnHidden(k, false);     //重新把列显示出来

#endif

}
