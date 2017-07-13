#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QDebug>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_clearButton(Q_NULLPTR),
    m_trayIconMenu(new QMenu(this)),
    m_trayIcon(new QSystemTrayIcon(this)),
    m_restoreAction(new QAction(tr("&隐藏清单"), this)),
    m_quitAction(new QAction(tr("&退出"), this)),
    m_newDlg(new NewDlg(0)),
    m_settingsDatabase(Q_NULLPTR),
    m_billView(Q_NULLPTR),
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
    setupDatabases();
    setupModelView();
    setupSignalsSlots();

    setupTrayIcon();
//    ui->billView->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_newDlg;
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

void MainWindow::setupTrayIcon()
{
    m_trayIconMenu->addAction(m_restoreAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    QIcon icon(":images/notes_system_tray_icon.png");
    m_trayIcon->setIcon(icon);
    m_trayIcon->setContextMenu(m_trayIconMenu);
    m_trayIcon->show();
}

void MainWindow::onLineEditTextChanged (const QString &keyword)
{
    m_searchQueue.enqueue(keyword);

    if(!m_isOperationRunning){
        m_isOperationRunning = true;
        if(!m_selectedBillBeforeSearchingInSource.isValid()
                 && m_currentSelectedBillProxy.isValid()){

            m_selectedBillBeforeSearchingInSource = m_proxyModel->mapToSource(m_currentSelectedBillProxy);
        }

        if(m_currentSelectedBillProxy.isValid()
                && m_isContentModified){

            saveBillToDB(m_currentSelectedBillProxy);
        }

         m_billView->setSearching(true);

        while(!m_searchQueue.isEmpty()){
            qApp->processEvents();
            QString str = m_searchQueue.dequeue();
            if(str.isEmpty()){
                m_billView->setFocusPolicy(Qt::StrongFocus);
                clearSearch();
                QModelIndex indexInProxy = m_proxyModel->mapFromSource(m_selectedBillBeforeSearchingInSource);
                selectBill(indexInProxy);
                m_selectedBillBeforeSearchingInSource = QModelIndex();
            }else{
                m_billView->setFocusPolicy(Qt::NoFocus);
                findBillsContain(str);
            }
        }

        m_isOperationRunning = false;
    }
}

void MainWindow::onBillPressed (const QModelIndex& index)
{
    if(sender() != Q_NULLPTR){
        QModelIndex indexInProxy = m_proxyModel->index(index.row(), 0);
        selectBill(indexInProxy);
    }
}

void MainWindow::QuitApplication ()
{
    MainWindow::close();
}

QDateTime MainWindow::getQDateTime (QString date)
{
    QDateTime dateTime = QDateTime::fromString(date, Qt::ISODate);
    return dateTime;
}

void MainWindow::loadBills ()
{
    QList<BillData*> billList = m_dbManager->getAllBills();

    if(!billList.isEmpty()){
        m_billModel->addListBill(billList);
        m_billModel->sort(0,Qt::AscendingOrder);
    }
}

void MainWindow::setupSignalsSlots()
{
    connect(m_newDlg, &NewDlg::newItem, this, &MainWindow::createNewBill);
    // line edit text changed
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::onLineEditTextChanged);
    // bill pressed
    connect(m_billView, &BillView::pressed, this, &MainWindow::onBillPressed);
    // billView viewport pressed
    connect(m_billView, &BillView::viewportPressed, this, [this](){
        if(m_proxyModel->rowCount() > 1){
            QModelIndex indexInProxy = m_proxyModel->index(1, 0);
            selectBill(indexInProxy);
        }
    });
    // note model rows moved
    connect(m_billModel, &BillModel::rowsAboutToBeMoved, m_billView, &BillView::rowsAboutToBeMoved);
    connect(m_billModel, &BillModel::rowsMoved, m_billView, &BillView::rowsMoved);
    // clear button
    connect(m_clearButton, &QToolButton::clicked, this, &MainWindow::onClearButtonClicked);
    // Restore Notes Action
    connect(m_restoreAction, &QAction::triggered, this, [this](){
        setMainWindowVisibility(isHidden()
                                || windowState() == Qt::WindowMinimized
                                || (qApp->applicationState() == Qt::ApplicationInactive));
    });
    // Quit Action
    connect(m_quitAction, &QAction::triggered, this, &MainWindow::QuitApplication);
    // Application state changed
    connect(qApp, &QApplication::applicationStateChanged, this,[this](){
        m_billView->update(m_billView->currentIndex());
    });
}

void MainWindow::setupFonts()
{
    ui->lineEdit->setFont(QFont("Roboto", 10));
    //m_editorDateLabel->setFont(QFont(QStringLiteral("Roboto"), 10, QFont::Bold));
}

void MainWindow::initializeSettingsDatabase()
{
    if(m_settingsDatabase->value("version", "NULL") == "NULL")
        m_settingsDatabase->setValue("version", "1.0.0");
}

void MainWindow::setupDatabases ()
{
    m_settingsDatabase = new QSettings(QSettings::IniFormat, QSettings::UserScope, "yuanmu", "Settings", this);
    m_settingsDatabase->setFallbacksEnabled(false);
    initializeSettingsDatabase();

    bool doCreate = false;
    QFileInfo fi(m_settingsDatabase->fileName());
    QDir dir(fi.absolutePath());
    QString billDBFilePath(dir.path() + "/bills.db");

    if(!QFile::exists(billDBFilePath)){
        QFile billDBFile(billDBFilePath);
        if(!billDBFile.open(QIODevice::WriteOnly)){
            qDebug() << "can't create db file";
            qApp->exit(-1);
        }
        billDBFile.close();
        doCreate = true;
    }

    m_dbManager = new DBManager(billDBFilePath, doCreate, this);
    m_billCounter = m_dbManager->getLastRowID();
}


void MainWindow::setupModelView()
{
    m_billView = static_cast<BillView*>(ui->billView);
    m_proxyModel->setSourceModel(m_billModel);
    m_proxyModel->setFilterKeyColumn(0);
    m_proxyModel->setFilterRole(BillModel::BillCustomer);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_billView->setItemDelegate(new BillWidgetDelegate(m_billView));
    m_billView->setModel(m_proxyModel);
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
        m_restoreAction->setText(tr("&隐藏清单"));
    }else{
        m_restoreAction->setText(tr("&显示清单"));
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

void MainWindow::moveBillToTop()
{
    // check if the current note is note on the top of the list
    // if true move the note to the top
    if(m_currentSelectedBillProxy.isValid()
            && m_billView->currentIndex().row() != 0){

        m_billView->scrollToTop();

        // move the current selected note to the top
        QModelIndex sourceIndex = m_proxyModel->mapToSource(m_currentSelectedBillProxy);
        QModelIndex destinationIndex = m_billModel->index(0);
        m_billModel->moveRow(sourceIndex, sourceIndex.row(), destinationIndex, 0);

        // update the current item
        m_currentSelectedBillProxy = m_proxyModel->mapFromSource(destinationIndex);
        m_billView->setCurrentIndex(m_currentSelectedBillProxy);
    }
}


void MainWindow::saveBillToDB(const QModelIndex &billIndex)
{
    if(billIndex.isValid() && m_isContentModified){
        QModelIndex indexInSrc = m_proxyModel->mapToSource(billIndex);
        BillData* bill = m_billModel->getBill(indexInSrc);
        if(bill != Q_NULLPTR){
            bool doExist = m_dbManager->isBillExist(bill);
            if(!doExist){
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

void MainWindow::on_btnInsert_clicked()
{
    m_newDlg->show();
}

void MainWindow::createNewBill(QString no,
                               QString variety,
                               QString detail_code,
                               QString price,
                               QString customer,
                               QString comment,
                               QDateTime creationDateTime)
{
    if (!m_isOperationRunning)
    {
        m_isOperationRunning = true;
        BillData* pBillData = new BillData(this);
        pBillData->setId(QString(m_dbManager->getLastRowID()+1));
        pBillData->setNo(no);
        pBillData->setVariety(variety);
        pBillData->setdetailCode(detail_code);
        pBillData->setPrice(price);
        pBillData->setCustomer(customer);
        pBillData->setComment(comment);
        pBillData->setCreationDateTime(creationDateTime);
        m_billView->scrollToTop();
        ++m_billCounter;
        QModelIndex indexSrc = m_billModel->insertBill(pBillData, 0);
        m_currentSelectedBillProxy = m_proxyModel->mapFromSource(indexSrc);
    }
    m_billView->setCurrentIndex(m_currentSelectedBillProxy);
    m_isOperationRunning = false;
    saveBillToDB(m_currentSelectedBillProxy);
    QMessageBox::critical(0, "创建表单", "表单创建成功", QMessageBox::Ok);
}
