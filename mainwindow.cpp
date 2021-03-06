﻿#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QDebug>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QTextDocument>
#include <QTooltip>
#include <QInputDialog>

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
    m_proxyModel(new QSortFilterProxyModel(this)),
    m_dbManager(Q_NULLPTR),
    m_printer(new QPrinter(QPrinter::HighResolution)),
    m_billCounter(0),
    m_trashCounter(0),
    m_layoutMargin(10),
    m_canMoveWindow(false),
    m_canStretchWindow(false),
    m_isOperationRunning(false)
{
    ui->setupUi(this);
//    bool isOk = false;
//    QString text = QInputDialog::getText(NULL, "Password",
//                                            "Please input password",
//                                            QLineEdit::Password,    //输入的是密码，不显示明文
//                                            NULL,
//                                            &isOk);

//    if(text != "yuer1234") {
//       QMessageBox::information(NULL, "Information",
//                                           "Your password is wrong",
//                                           QMessageBox::Yes);
//       setEnabled(false);
//       exit(0);
//    }
    grid=TDPreviewDialog::NormalGrid;
    setupLine();
    setupLineEdit();
    setupDatabases();
    setupModelView();
    setupSignalsSlots();

    setupTrayIcon();
    loadBills();
    InitCombox();
    QString dateText("日期: "), year, month, day;
    QDate date = QDate::currentDate();
    year.setNum(date.year());
    month.setNum(date.month());
    day.setNum(date.day());
    QString dateString(year);
    dateString.append("-").append(month).append("-").append(day);
    dateText.append(dateString);
    ui->labelDate->setText(dateText);

//    ui->billView->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_newDlg;
    delete m_printer;
}

void MainWindow::setupLine()
{
//    ui->lineLeft->setStyleSheet("border: 1px solid rgb(221, 221, 221)");
//    ui->lineLeft->setStyleSheet("border: 1px solid rgb(221, 221, 221)");
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
    if(!m_isOperationRunning){
        m_isOperationRunning = true;
        if(!m_selectedBillBeforeSearchingInSource.isValid()
                 && m_currentSelectedBillProxy.isValid()){

            m_selectedBillBeforeSearchingInSource = m_proxyModel->mapToSource(m_currentSelectedBillProxy);
        }
        m_billView->setSearching(true);

        if(keyword.isEmpty()){
            m_billView->setFocusPolicy(Qt::StrongFocus);
            clearSearch();
            m_proxyModel->setFilterFixedString("这是一个非法的查询");
            m_curClickIndex = QModelIndex();
            m_currentSelectedBillProxy = QModelIndex();
            m_selectedBillBeforeSearchingInSource = QModelIndex();
        }else{
            m_billView->setFocusPolicy(Qt::NoFocus);
            findBillsContain(keyword);
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
    QList<BillData*> billList;
    if (ui->lineEdit->text().isEmpty())
    {
        billList = m_dbManager->getAllBills();
    } else
    {
        billList = m_dbManager->getBillsByCustomer(ui->lineEdit->text());
    }

    if(!billList.isEmpty()){
        m_billModel->addListBill(billList);
    }
}

void MainWindow::InitCombox()
{
    ui->comboCustomer->addItem("");
    QList<BillData *> billList = m_billModel->getAllBills();
    for (int i = 0; i < billList.size(); i++)
    {
        BillData* pData = billList.at(i);
        if (pData != Q_NULLPTR)
        {
            int nIndex = ui->comboCustomer->findText(pData->customer());
            if (nIndex == -1)
                ui->comboCustomer->addItem(pData->customer());
        }
    }
    ui->comboCustomer->setCurrentIndex(ui->comboCustomer->count()-1);
}

void MainWindow::InsertComboxItem(QString customer)
{
    int nIndex = ui->comboCustomer->findText(customer);
    if (nIndex == -1)
    {
        ui->comboCustomer->insertItem(1, customer);
        ui->comboCustomer->setCurrentIndex(1);
        onLineEditTextChanged(customer);
    }
}

void MainWindow::RemoveComboxItem(QString customer)
{
    int nIndex = ui->comboCustomer->findText(customer);
    if (nIndex != -1)
        ui->comboCustomer->removeItem(nIndex);
}

void MainWindow::setupSignalsSlots()
{
    connect(m_newDlg, &NewDlg::newItem, this, &MainWindow::createNewBill);
    // line edit text changed
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::onLineEditTextChanged);
    // bill pressed
    connect(m_billView, &BillsView::pressed, this, &MainWindow::onBillPressed);
    // billView viewport pressed
    connect(m_billView, &BillsView::viewportPressed, this, [this](){
        if(m_proxyModel->rowCount() > 1){
            QModelIndex indexInProxy = m_proxyModel->index(1, 0);
            selectBill(indexInProxy);
        }

    });
    // note model rows moved
    connect(m_billModel, &BillModel::rowsAboutToBeMoved, m_billView, &BillsView::rowsAboutToBeMoved);
    connect(m_billModel, &BillModel::rowsMoved, m_billView, &BillsView::rowsMoved);
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
    m_billCounter = m_dbManager->getLastRowID(); // 空提示没有合法记录，返回0
}


void MainWindow::setupModelView()
{
    m_billView = static_cast<BillsView*>(ui->tableBills);
    m_proxyModel->setSourceModel(m_billModel);
//    m_proxyModel->setFilterKeyColumn(-1);
    m_proxyModel->setFilterKeyColumn(12);
    m_proxyModel->setFilterRole(Qt::DisplayRole);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setSortRole(Qt::InitialSortOrderRole);
    m_proxyModel->invalidate();
//    m_billView->setItemDelegate(new BillWidgetDelegate(m_billView));
    m_billView->setModel(m_proxyModel);  
    m_billView->initHeaderView();
    m_billView->setColumnHidden(12, true);
    m_miniPrintSize = QSize(860, m_billView->height());
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
        m_currentSelectedBillProxy = billIndex;

        m_billView->selectionModel()->select(m_currentSelectedBillProxy, QItemSelectionModel::ClearAndSelect);
        m_billView->setCurrentIndex(m_currentSelectedBillProxy);
//        m_billView->scrollTo(m_currentSelectedBillProxy);

    }else{
        m_currentSelectedBillProxy = billIndex;

        m_billView->clearSelection();


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
    if(billIndex.isValid()){
        QModelIndex indexInSrc = m_proxyModel->mapToSource(billIndex);
        BillData* bill = m_billModel->getBill(indexInSrc);
        if(bill != Q_NULLPTR){
            bool doExist = m_dbManager->isBillExist(bill);
            if(!doExist){
                QtConcurrent::run(m_dbManager, &DBManager::addBill, bill);
            }
        }
    }
}

void MainWindow::saveBill(BillData *billData)
{
    if(billData != Q_NULLPTR){
        bool doExist = m_dbManager->isBillExist(billData);
        if(!doExist){
            QtConcurrent::run(m_dbManager, &DBManager::addBill, billData);
        }
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

void MainWindow::removeBillByCustomer(const QString customer)
{
    m_dbManager->removeBillsByCustomer(customer);
    QModelIndex indexSrc = m_proxyModel->mapToSource(m_curClickIndex);
    m_billModel->removeBill(indexSrc);
    m_curClickIndex = QModelIndex();
    m_currentSelectedBillProxy = QModelIndex();
    m_selectedBillBeforeSearchingInSource = QModelIndex();
}

void MainWindow::deleteBill(const QModelIndex &billIndex, bool isFromUser)
{
    if(billIndex.isValid()){
        // delete from model
        QModelIndex indexToBeRemoved = m_proxyModel->mapToSource(m_currentSelectedBillProxy);
        BillData* billTobeRemoved = m_billModel->removeBill(indexToBeRemoved);

        QtConcurrent::run(m_dbManager, &DBManager::removeBill, billTobeRemoved);

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
    QString customer = ui->comboCustomer->currentText();
    m_newDlg->init(m_billModel->getBillsByCustomer(customer), customer);
    m_newDlg->show();
}

void MainWindow::createNewBill(QString no,
                               QString variety,
                               QString detail_code,
                               QString price,
                               QString customer,
                               QString comment)
{
    if (!m_isOperationRunning)
    {
        m_isOperationRunning = true;
        BillData* pBillData = new BillData(this);
        QString id;
        pBillData->setId(id.setNum(m_billModel->getLastID()+1));
        pBillData->setNo(no);
        pBillData->setVariety(variety);
        pBillData->setdetailCode(detail_code);
        pBillData->setPrice(price);
        pBillData->setCustomer(customer);
        pBillData->setComment(comment);
        QDateTime billDate = QDateTime::currentDateTime();
        pBillData->setCreationDateTime(billDate);
        ++m_billCounter;
        int nIndex = ui->comboCustomer->findText(customer);
        QModelIndex indexSrc;
        if (nIndex == -1) // 创建新客户，清零
        {
            m_curClickIndex = QModelIndex();
            m_currentSelectedBillProxy = QModelIndex();
            m_selectedBillBeforeSearchingInSource = QModelIndex();
            m_billModel->addBill(pBillData);
            saveBill(pBillData);
        } else
        {
            indexSrc = m_billModel->addBill(pBillData);
            m_currentSelectedBillProxy = m_proxyModel->mapFromSource(indexSrc);
            saveBillToDB(m_currentSelectedBillProxy);
        }

        m_billView->setCurrentIndex(m_currentSelectedBillProxy);
        m_isOperationRunning = false;
        QMessageBox::information(0, "创建表单", "表单创建成功", QMessageBox::Ok);
        InsertComboxItem(customer);
//        m_billModel->sort(0,Qt::AscendingOrder);
    }

}

void MainWindow::on_comboCustomer_currentIndexChanged(int index)
{

}

void MainWindow::on_comboCustomer_currentIndexChanged(const QString &arg1)
{
    if (!arg1.isEmpty())
    {
        // 根据客户名筛选表项
        QString customerText("客户名: ");
        customerText.append(arg1);
        ui->labelCustomer->setText(customerText);
        onLineEditTextChanged(arg1);
    } else
    {
        ui->labelCustomer->setText("");
        onLineEditTextChanged(arg1);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) // 删除选定的行
    {
        if (m_curClickIndex.isValid())
        {

            QModelIndex indexSrc = m_proxyModel->mapToSource(m_curClickIndex);

            BillData* bill = m_billModel->getBill(indexSrc);
            m_dbManager->removeBill(bill);
            m_billModel->removeBill(indexSrc);
            m_curClickIndex = QModelIndex();
            m_currentSelectedBillProxy = QModelIndex();
            m_selectedBillBeforeSearchingInSource = QModelIndex();
            int posX = m_billView->pos().x();
            int posY = m_billView->pos().y();
            QToolTip::showText(QPoint(posX+m_billView->width()/2, posY+m_billView->height()/2), "列表项删除成功");
        }
    }
}

void MainWindow::print()   //printFlag =2 , 打印预览
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageSize(QPrinter::A4);

    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("打印预览");
    selectBill(QModelIndex());
//    preview.setMinimumSize(m_miniPrintSize);
    preview.setFixedSize(m_miniPrintSize);
    m_billView->setInputInfo(ui->labelCustomer->text(), ui->lineMaker->text(), ui->lineDriver->text());
    m_billView->setTotalMoney(m_billModel->getTotalMoneyByCustomer(ui->comboCustomer->currentText()));
    connect(&preview, &QPrintPreviewDialog::paintRequested,m_billView,&BillsView::prints);
    preview.exec();
}

void MainWindow::printTable(QPrinter *printer)
{
    QString strStream,strTitle;
    QTextStream out(&strStream);
    strTitle="清单打印";
    const int rowCount = ui->tableBills->model()->rowCount();
    const int columnCount = ui->tableBills->model()->columnCount();
    out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Textml; charset=Windows-1251\">\n"
         <<  QString("<title>%1</title>\n").arg(strTitle)
          <<  "</head>\n"
              "<body bgcolor=#ffffff link=#5000A0>\n"
              "<table border=1 cellspacing=0 cellpadding=2>\n";
    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    HHeaderModel* pHeaderModel = (HHeaderModel *)(ui->tableBills->horizontalHeader()->model());
    for (int column = 0; column < columnCount; ++column)
    {
        if (!ui->tableBills->isColumnHidden(column))
        {
//            if (column <= 3 || column >= 7)
//            {
                out << QString("<th>%1</th>").arg(pHeaderModel->item(0, column));
//            } else
//            {
//                out << QString("%1").arg(pHeaderModel->item(0, column));
//            }
        }
    }
    out << "</tr></thead>\n";
    // data table
    for (int row = 0; row < rowCount; ++row)
    {
        out << "<tr>";
        for (int column = 0; column < columnCount; ++column)
        {
            if (!ui->tableBills->isColumnHidden(column)) {
                QString data = ui->tableBills->model()->data(ui->tableBills->model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
            "</body>\n"
            "<ml>\n";
    QTextDocument *document = new QTextDocument();
    document->setHtml(strStream);
    document->print(printer);
}

void MainWindow::on_btnPreview_clicked()
{
    print();
}

void MainWindow::on_tableBills_clicked(const QModelIndex &index)
{
    m_curClickIndex = index;
}

void MainWindow::on_btnDelCustomer_clicked()
{
    QString customer = ui->comboCustomer->currentText();
    if (!customer.isEmpty())
    {
        // 删除不需要的客户表单列表
        m_dbManager->removeBillsByCustomer(customer);
        m_billModel->removeBillByCustomer(customer);
        m_curClickIndex = QModelIndex();
        m_selectedBillBeforeSearchingInSource = QModelIndex();
        m_currentSelectedBillProxy = QModelIndex();
        QMessageBox::information(0, "提示", "删除成功", QMessageBox::Ok);
        ui->comboCustomer->removeItem(ui->comboCustomer->currentIndex());
        ui->labelCustomer->setText("");
    } else
    {
        QMessageBox::information(0, "提示", "请先选择要删除的客户", QMessageBox::Ok);
    }
}
