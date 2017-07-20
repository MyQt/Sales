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
#include <QTextOption>
#include <QDate>

#pragma execution_character_set("utf-8")

BillsView::BillsView(QWidget *parent)
    : QTableView( parent ),
      m_isSearching(false),
      m_isMousePressed(false)
{

    QTimer::singleShot(0, this, SLOT(init()));
    resizeColumnsToContents ();
    resizeRowsToContents();
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
   pHeadView->setModel(pHeadModel);
   pHeadView->initHeaderView(pHeadModel);
   setHorizontalHeader(pHeadView);

   pHeadView->setSectionSize(pHeadModel, width());
//   int colWidth = width()/pHeadModel->getColumnCount();
//   setColumnWidth(0, colWidth);
//   setColumnWidth(1, colWidth);
//   setColumnWidth(2, colWidth);
//   setColumnWidth(3, colWidth);
//   setColumnWidth(4, colWidth);
//   setColumnWidth(5, colWidth);
//   setColumnWidth(6, colWidth);
//   setColumnWidth(7, colWidth);
//   setColumnWidth(8, colWidth);
//   setColumnWidth(9, colWidth);
//   setColumnWidth(10, colWidth*2);
//   setColumnWidth(11, colWidth*2);
//   setColumnWidth(12, colWidth);
   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   verticalHeader()->hide();
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
//    viewport()->setAttribute(Qt::WA_Hover);

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
    HHeaderView *headerView = (HHeaderView*)this->horizontalHeader ();
    if (headerView != NULL)    pix_height = headerView->height();

    pageCount = 0;

    int i;

    printAreaStartRow.push_back(0);
    for (i = 0; i < model()->rowCount(); ) {
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
    HHeaderView *headerView = (HHeaderView*)this->verticalHeader ();
    if (headerView != NULL)    pix_width = headerView->width();

    pageCount = 0;

    int i;
    printAreaStartCol.push_back(0);
    for (i = 0; i < model()->columnCount() && !isColumnHidden(i); ) {
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

    HHeaderView *headerView = (HHeaderView*)this->verticalHeader ();
    if (headerView != NULL)    pix_width = headerView->width();

    for (int i = 0; i < model()->columnCount()&&!isColumnHidden(i); i++) {
        pix_width += columnWidth(i);
    }

    return pix_width;
}
// 计算整个页面高度
int BillsView::CalcultTableViewHeight()
{
    int pix_height = 0;
    HHeaderView *headerView = (HHeaderView *)this->horizontalHeader ();
    if (headerView != NULL)    pix_height = headerView->height();

    for (int i = 0; i < model()->rowCount(); i++) {
        pix_height += rowHeight(i);
    }

    return pix_height;
}

QString BillsView::getUpperTotalMoney(QString totalMoney)
{
    const QString DXSZ = "零壹贰叁肆伍陆柒捌玖";
    const QString DXDW = "毫厘分角元拾佰仟萬拾佰仟亿拾佰仟萬兆拾佰仟萬亿京拾佰仟萬亿兆垓";
    const QString SCDW = "元拾佰仟萬亿京兆垓";
    QString capValue = totalMoney;
    QString currCap = "";    //当前金额
    QString capResult = "";  //结果金额
    QString currentUnit = "";//当前单位
    QString resultUnit = ""; //结果单位
    int prevChar = -1;      //上一位的值
    int currChar = 0;       //当前位的值
    int posIndex = 4;       //位置索引，从"元"开始
    for (int i = capValue.length() - 1; i >= 0; i--)
    {
       currChar = QString(capValue.at(i)).toInt();
       if (posIndex > 22)
       {
           //已超出最大精度"垓"。注：可以将30改成22，使之精确到兆亿就足够了
           break;
       }
       else if (currChar != 0)
       {
           //当前位为非零值，则直接转换成大写金额
           currCap = QString(DXSZ.at(currChar)) + QString(DXDW.at(posIndex));
       }
       else
       {
           //防止转换后出现多余的零,例如：3000020
           switch (posIndex)
           {
               case 4: currCap = "元"; break;
               case 8: currCap = "萬"; break;
               case 12: currCap = "亿"; break;
               case 17: currCap = "兆"; break;
               case 23: currCap = "京"; break;
               case 30: currCap = "垓"; break;
               default: break;
           }
           if (prevChar != 0 )
           {
               if (currCap != "")
               {
                   if (currCap != "元") currCap += "零";
               }
               else
               {
                   currCap = "零";
               }
           }
       }
      //对结果进行容错处理
       if(capResult.length() > 0)
       {
             resultUnit = capResult.at(0);
             currentUnit = DXDW.at(posIndex);
             if (SCDW.indexOf(resultUnit) > 0)
             {
                   if (SCDW.indexOf(currentUnit) > SCDW.indexOf(resultUnit))
                   {
                         capResult = capResult.indexOf(1);
                   }
             }
       }
       capResult = currCap + capResult;
       prevChar = currChar;
       posIndex += 1;
       currCap = "";
    }
    capResult.append("整");
    return capResult;
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
//    QFontMetrics *fmt;
    //title font
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleFont.setUnderline(true);
    titleFmt =new QFontMetrics(titleFont);


    //header font
    headerFont.setBold(true);
    headerFont.setPointSize(10);
    headerFmt =new QFontMetrics(headerFont);

    QString titleText("商丘金花布业有限公司销售清单");
    int titleWidth = titleFmt->width(titleText);
    QString headerText;
    QString footerText;
    QString pageText;
    QString year, month, day;
    QDate date = QDate::currentDate();
    year.setNum(date.year());
    month.setNum(date.month());
    day.setNum(date.day());
    QString dateString(year);
    dateString.append("-").append(month).append("-").append(day);
    headerText = m_customer +"  ";
    headerText.append("地址: 商丘市梁园区清凉寺大道与陇海路交叉口往西300路北  ");
    headerText.append("电话: 0370-2212794  ");
    headerText.append("日期: ");
    headerText.append(dateString);
    int headerWidth = headerFmt->width(headerText);
    footerText="制单人: ";
    footerText.append(m_maker).append("                ");
    footerText.append("司机: ").append(m_driver).append("                ");
    footerText.append("卡号: 邮政卡号: 6221 8850 6100 7763 400 户名: 孟金花");
    footerText.append("\n                                                  农行卡号: 6228 4923 8900 1244 974 户名: 李凤启");
    footerText.append("\n                                                  工行卡号: 6222 0817 1600 1059 059 户名: 李凤启");
    footerText.append("\n                                                  农信社卡号: 6229 9113 4900 724 633 户名: 李凤启");

    int footerWidth = headerFmt->width(footerText);
    QRect rectOrig = geometry();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    int marginLeft = 10, marginTop = 100, marginRight = 10, marginBottom = 100;

    QRect paperRect = printer->pageRect();
    paperRect.adjust(0, 0, -marginLeft - marginRight, -marginTop - marginBottom);
    resize(paperRect.width(), paperRect.height());

    int i,j,k;
    int row_pageCount = 1, col_pageCount = 1;
    QVector<int> printAreaStartRow, printAreaHeight;
    QVector<int> printAreaStartCol, printAreaWidth;

    CalcultPrintAreaRow(paperRect.height(), row_pageCount, printAreaStartRow, printAreaHeight);
    CalcultPrintAreaCol(paperRect.width()+40,  col_pageCount, printAreaStartCol, printAreaWidth);

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
            QFont fontRe = painter.font();
            painter.setFont(titleFont);
            painter.drawText(QRectF(printer->pageRect().width()/2-titleWidth/2, marginTop/4, titleWidth, titleFmt->height()), titleText);
            painter.setFont(headerFont);
            painter.drawText(QRectF(marginLeft, marginTop-headerFmt->height()-2, headerWidth, headerFmt->height()), headerText);
            painter.setFont(headerFont);
            painter.drawText(QRectF(marginLeft, printer->pageRect().height()-headerFmt->height()-2, pageWidth, headerFmt->height()), pageText);
            if (i == row_pageCount-1) // 最后一页，加上合计信息
            {
                QString totalUpper("合(大写)计: ");
                QString totalCase("合(小写)计: ");
                QString totalUpperValue = getUpperTotalMoney(m_totalMoney);
                QString totalCaseValue = m_totalMoney.setNum(m_totalMoney.toFloat(), 'f', 2);

                int totalHeight = headerFmt->height()+8;

                // 上包边
                painter.drawLine(QPointF(marginLeft, marginTop+printAreaHeight[i]+1), QPointF(marginLeft+printAreaWidth[j], marginTop+printAreaHeight[i]+1));

                // 大写
                painter.drawLine(QPointF(marginLeft, marginTop+printAreaHeight[i]+1), QPointF(marginLeft, marginTop+printAreaHeight[i]+1+totalHeight));
                painter.drawText(QPointF(marginLeft+2, marginTop+printAreaHeight[i]+1+totalHeight*0.8), totalUpper);
                painter.drawLine(QPointF(marginLeft+headerFmt->width(totalUpper)+4, marginTop+printAreaHeight[i]+1), QPointF(marginLeft+headerFmt->width(totalUpper)+4, marginTop+printAreaHeight[i]+1+totalHeight));
                painter.drawText(QPointF(marginLeft+headerFmt->width(totalUpper)+6, marginTop+printAreaHeight[i]+1+totalHeight*0.8), totalUpperValue);
                // 小写
                painter.drawLine(QPointF(marginLeft+headerFmt->width(totalUpper)+headerFmt->width(totalUpperValue)+8, marginTop+printAreaHeight[i]+1), QPointF(marginLeft+headerFmt->width(totalUpper)+headerFmt->width(totalUpperValue)+8, marginTop+printAreaHeight[i]+1+totalHeight));
                painter.drawText(QPointF(marginLeft+headerFmt->width(totalUpper)+headerFmt->width(totalUpperValue)+10, marginTop+printAreaHeight[i]+1+totalHeight*0.8), totalCase);
                painter.drawLine(QPointF(marginLeft+headerFmt->width(totalUpper)+headerFmt->width(totalUpperValue)+headerFmt->width(totalCase)+12, marginTop+printAreaHeight[i]+1), QPointF(marginLeft+headerFmt->width(totalUpper)+headerFmt->width(totalUpperValue)+headerFmt->width(totalCase)+12, marginTop+printAreaHeight[i]+1+totalHeight));
                painter.drawText(QPointF(marginLeft+headerFmt->width(totalUpper)+headerFmt->width(totalUpperValue)+headerFmt->width(totalCase)+14, marginTop+printAreaHeight[i]+1+totalHeight*0.8), totalCaseValue);
                // 右包边
                painter.drawLine(QPointF(marginLeft+printAreaWidth[j], marginTop+printAreaHeight[i]+1), QPointF(marginLeft+printAreaWidth[j], marginTop+printAreaHeight[i]+totalHeight+2));
                // 底包边
                painter.drawLine(QPointF(marginLeft, marginTop+printAreaHeight[i]+totalHeight+2), QPointF(marginLeft+printAreaWidth[j], marginTop+printAreaHeight[i]+totalHeight+2));
                painter.setFont(headerFont);
                painter.drawText(QRectF(marginLeft, marginTop+printAreaHeight[i]+totalHeight+5, footerWidth, headerFmt->height()*4+8), footerText); // 在总计下面显示页脚信息
            } else // 直接在表格下面显示页脚信息
            {
                painter.setFont(headerFont);
                painter.drawText(QRectF(marginLeft, marginTop+printAreaHeight[i]+3, footerWidth, headerFmt->height()*4+8), footerText);
            }
            if (i > 0 || col_pageCount == 1 && !printer->fullPage())
            {
                painter.drawLine(QPointF(marginLeft, marginTop+printAreaHeight[i]+1), QPointF(marginLeft+printAreaWidth[j], marginTop+printAreaHeight[i]+1));
            }
            painter.setFont(fontRe);
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
    for(int m = 0; m < model()->rowCount(); m++)
    {
        setRowHidden(m, false);
    }
    for (k = 0; k < this->model()->columnCount() && k < 12; k++) setColumnHidden(k, false);     //重新把列显示出来
    setGeometry(rectOrig); // 最后再复原，否则最后一列会变宽
#endif

}
