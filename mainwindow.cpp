#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_clearButton(Q_NULLPTR),
    m_trayIcon(new QSystemTrayIcon(this)),
    m_restoreAction(new QAction(tr("&Hide Sales"), this)),
    m_quitAction(new QAction(tr("&Quit"), this)),
    m_billModel(new BillModel(this)),
    m_deletedBillsModel(new BillModel(this)),
    m_proxyModel(new QSortFilterProxyModel(this)),
    m_dbManager(Q_NULLPTR),
    m_billCounter(0),
    m_trashCounter(0),
    m_layoutMargin(10),
    m_billListWidth(200),
    m_canMoveWindow(false),
    m_canStretchWindow(false),
    m_isTemp(false),
    m_isListViewScrollBarHidden(true),
    m_isContentModified(false),
    m_isOperationRunning(false)
{
    ui->setupUi(this);
    setupLine();
    setupLineEdit();
    // clear button
    connect(m_clearButton, &QToolButton::clicked, this, &MainWindow::onClearButtonClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupLine()
{
    ui->lineLeft->setStyleSheet("border: 1px solid rgb(221, 221, 221)");
    ui->lineLeft->setStyleSheet("border: 1px solid rgb(221, 221, 221)");
}

void MainWindow::setupLineEdit()
{
    int frameWidth = ui->lineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QString ss = QString("QLineEdit{ "
                         "  padding-right: %1px; "
                         "  padding-left: 21px;"
                         "  padding-right: 19px;"
                         "  border: 1px solid rgb(205, 205, 205);"
                         "  border-radius: 3px;"
                         "  background: rgb(255, 255, 255);"
                         "  selection-background-color: rgb(61, 155, 218);"
                         "} "
                         "QToolButton { "
                         "  border: none; "
                         "  padding: 0px;"
                         "}"
                         ).arg(frameWidth + 1);

    ui->lineEdit->setStyleSheet(ss);

    // clear button
    m_clearButton = new QToolButton(ui->lineEdit);
    QPixmap pixmap(":images/closeButton.png");
    m_clearButton->setIcon(QIcon(pixmap));
    QSize clearSize(15, 15);
    m_clearButton->setIconSize(clearSize);
    m_clearButton->setCursor(Qt::ArrowCursor);
    m_clearButton->hide();

    // search button
    QToolButton *searchButton = new QToolButton(ui->lineEdit);
    QPixmap newPixmap(":images/magnifyingGlass.png");
    searchButton->setIcon(QIcon(newPixmap));
    QSize searchSize(24, 25);
    searchButton->setIconSize(searchSize);
    searchButton->setCursor(Qt::ArrowCursor);
    searchButton->show();
    // layout
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::RightToLeft, ui->lineEdit);
    layout->setContentsMargins(0,0,3,0);
    layout->addWidget(m_clearButton);
    layout->addStretch();
    layout->addWidget(searchButton);
    ui->lineEdit->setLayout(layout);

    ui->lineEdit->installEventFilter(this);
}

void MainWindow::setupFonts()
{
    ui->lineEdit->setFont(QFont("Roboto", 10));
    //m_editorDateLabel->setFont(QFont(QStringLiteral("Roboto"), 10, QFont::Bold));
}

void MainWindow::setMainWindowVisibility(bool state)
{
    if(state){
        showNormal();
        setWindowState(Qt::WindowNoState);
        qApp->processEvents();
        setWindowState(Qt::WindowActive);
        qApp->processEvents();
        qApp->setActiveWindow(this);
        qApp->processEvents();
        m_restoreAction->setText(tr("&Hide Sales"));
    }else{
        m_restoreAction->setText(tr("&Show Sales"));
        hide();
    }
}

void MainWindow::onClearButtonClicked()
{
    if(!m_isOperationRunning){

        clearSearch();

        if(m_billModel->rowCount() > 0){
            QModelIndex indexInProxy = m_proxyModel->mapFromSource(m_selectedBillBeforeSearchingInSource);
            int row = m_selectedBillBeforeSearchingInSource.row();
            if(row == m_billModel->rowCount())
                indexInProxy = m_proxyModel->index(m_proxyModel->rowCount()-1,0);

            selectBill(indexInProxy);
        }else{
            m_currentSelectedBillProxy = QModelIndex();
        }

        m_selectedBillBeforeSearchingInSource = QModelIndex();

    }
}

void MainWindow::clearSearch()
{
    m_billView->setFocusPolicy(Qt::StrongFocus);

    ui->lineEdit->blockSignals(true);
    ui->lineEdit->clear();
    ui->lineEdit->blockSignals(false);

    m_proxyModel->setFilterFixedString((""));

    m_clearButton->hide();
    ui->lineEdit->setFocus();

    m_billView->setSearching(false);
}

void MainWindow::findBillsContain(const QString& keyword)
{
    m_proxyModel->setFilterFixedString(keyword);
    m_clearButton->show();

    if(m_proxyModel->rowCount() > 0){
        selectFirstBill();
    }else{
        m_currentSelectedBillProxy = QModelIndex();
    }
}

void MainWindow::selectBill(const QModelIndex &billIndex)
{
    if(billIndex.isValid()){
        // save the position of text edit scrollbar
        if(!m_isTemp && m_currentSelectedBillProxy.isValid()){
            //int pos = m_textEdit->verticalScrollBar()->value();
//            QModelIndex indexSrc = m_proxyModel->mapToSource(m_currentSelectedNoteProxy);
            //m_billModel->setData(indexSrc, QVariant::fromValue(pos), billModel::NoteScrollbarPos);
        }

        if(m_isTemp && billIndex.row() != 0){
            // delete the unmodified new note
            deleteBill(m_currentSelectedBillProxy, false);
            m_currentSelectedBillProxy = m_proxyModel->index(billIndex.row()-1, 0);
        }else if(!m_isTemp
                 && m_currentSelectedBillProxy.isValid()
                 && billIndex != m_currentSelectedBillProxy
                 && m_isContentModified){
            // save if the previous selected note was modified
            saveBillToDB(m_currentSelectedBillProxy);
            m_currentSelectedBillProxy = billIndex;
        }else{
            m_currentSelectedBillProxy = billIndex;
        }

        m_billView->selectionModel()->select(m_currentSelectedBillProxy, QItemSelectionModel::ClearAndSelect);
        m_billView->setCurrentIndex(m_currentSelectedBillProxy);
        m_billView->scrollTo(m_currentSelectedBillProxy);

    }else{
        qDebug() << "MainWindow::selectBill() : billIndex is not valid";
    }
}

void MainWindow::selectFirstBill()
{
    if(m_proxyModel->rowCount() > 0){
        QModelIndex index = m_proxyModel->index(0,0);
        m_billView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
        m_billView->setCurrentIndex(index);

        m_currentSelectedBillProxy = index;
    }
}

void MainWindow::saveBillToDB(const QModelIndex &billIndex)
{
    if(billIndex.isValid() && m_isContentModified){
        QModelIndex indexInSrc = m_proxyModel->mapToSource(billIndex);
        BillData* bill = m_billModel->getBill(indexInSrc);
        if(bill != Q_NULLPTR){
            bool doExist = m_dbManager->isBillExist(bill);
            if(doExist){
                QtConcurrent::run(m_dbManager, &DBManager::modifyBill, bill);
            }else{
                QtConcurrent::run(m_dbManager, &DBManager::addBill, bill);
            }
        }

        m_isContentModified = false;
    }
}

void MainWindow::removeBillFromDB(const QModelIndex& billIndex)
{
    if(billIndex.isValid()){
        QModelIndex indexInSrc = m_proxyModel->mapToSource(billIndex);
        BillData* bill = m_billModel->getBill(indexInSrc);
        m_dbManager->removeBill(bill);
    }
}

void MainWindow::deleteBill(const QModelIndex &billIndex, bool isFromUser)
{
    if(billIndex.isValid()){
        // delete from model
        QModelIndex indexToBeRemoved = m_proxyModel->mapToSource(m_currentSelectedBillProxy);
        BillData* billTobeRemoved = m_billModel->removeBill(indexToBeRemoved);

        if(m_isTemp){
            m_isTemp = false;
        }else{
            billTobeRemoved->setDeletionDateTime(QDateTime::currentDateTime());
            QtConcurrent::run(m_dbManager, &DBManager::removeBill, billTobeRemoved);
        }

        if(isFromUser){

            if(m_billModel->rowCount() > 0){
                QModelIndex index = m_billView->currentIndex();
                m_currentSelectedBillProxy = index;
            }else{
                m_currentSelectedBillProxy = QModelIndex();
            }
        }
    }else{
        qDebug() << "MainWindow::deleteBill billIndex is not valid";
    }

    m_billView->setFocus();
}
