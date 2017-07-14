#include "BillsView.h"
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
   pHeadView->setSectionSize(pHeadModel, width());
   pHeadView->setModel(pHeadModel);
   setHorizontalHeader(pHeadView);
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
    if(model() != Q_NULLPTR){
        if(index.row() > 1){
            QModelIndex prevPrevIndex = model()->index(index.row()-2, 0);
            viewport()->update(visualRect(prevPrevIndex));

            QModelIndex prevIndex = model()->index(index.row()-1, 0);
            viewport()->update(visualRect(prevIndex));

        }else if(index.row() > 0){
            QModelIndex prevIndex = model()->index(index.row()-1, 0);
            viewport()->update(visualRect(prevIndex));
        }

//        BillWidgetDelegate* delegate = static_cast<BillWidgetDelegate *>(itemDelegate());
//        if(delegate != Q_NULLPTR)
//            delegate->setHoveredIndex(index);
    }
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
                         "QScrollBar {margin-right: 2px; background: transparent;} "
                         "QScrollBar:hover { background-color: rgb(217, 217, 217);}"
                         "QScrollBar:handle:vertical:hover { background: rgb(170, 170, 171); } "
                         "QScrollBar:handle:vertical:pressed { background: rgb(149, 149, 149);}"
                         "QScrollBar:vertical { border: none; width: 10px; border-radius: 4px;} "
                         "QScrollBar::handle:vertical { border-radius: 4px; background: rgb(188, 188, 188); min-height: 20px; }  "
                         "QScrollBar::add-line:vertical { height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }  "
                         "QScrollBar::sub-line:vertical { height: 0px; subcontrol-position: top; subcontrol-origin: margin; }"
                         );

    setStyleSheet(ss);
}
