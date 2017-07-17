#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QDebug>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QTextDocument>
#include <QTooltip>

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
    grid=TDPreviewDialog::NoGrid;
    setupLine();
    setupLineEdit();
    setupDatabases();
    setupModelView();
    setupSignalsSlots();

    setupTrayIcon();
    loadBills();
    InitCombox();

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
        m_billModel->sort(0,Qt::AscendingOrder);
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
        ui->comboCustomer->insertItem(1, customer);
        ui->comboCustomer->setCurrentIndex(1);
        onLineEditTextChanged(customer);
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

//    m_billView->setItemDelegate(new BillWidgetDelegate(m_billView));
    m_billView->setModel(m_proxyModel);  
    m_billView->initHeaderView();
    m_billView->setColumnHidden(12, true);
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
    m_billModel->removeBill(m_curClickIndex);
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
        pBillData->setId(QString(m_dbManager->getLastRowID()+1));
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
            BillData* bill = m_billModel->getBill(m_curClickIndex);
            m_dbManager->removeBill(bill);
            m_billModel->removeBill(m_curClickIndex);
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
    QPrintPreviewDialog preview(&printer, this);
    preview.setMinimumSize(942,524);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)),this, SLOT(printTable(QPrinter *)));
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
            out << QString("<th>%1</th>").arg(pHeaderModel->item(0, column));
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
//    TDPreviewDialog *dialog = new TDPreviewDialog(m_billView,m_printer,this);
//    dialog->setGridMode(grid);
//    dialog->exec();
//    //do printing here...
//    //...
//    delete dialog;
    print();
}

void MainWindow::on_btnPrint_clicked()
{
    TDPreviewDialog *dialog = new TDPreviewDialog(m_billView,m_printer,this);
    dialog->setGridMode(grid);
    dialog->print();
    delete dialog;
}

//double totalWidth = 0.0;
//double totalHeight = 0.0;
//double totalPageHeight=0.0;

//int rows = view->model()->rowCount();   //行总数
//const int cols = view->model()->columnCount(); //列总数

//for (int c = 0; c < cols; ++c)  //求出列宽
//{
//    totalWidth += view->columnWidth(c);
//}

//for (int r = 0; r < rows; ++r) //求出行宽
//{
//    totalHeight += view->rowHeight(r);
//}

//QPainter painter(printer);
//painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//painter.begin(printer);
//QRect area = printer->paperRect();// paperRect();
//QVector<int> startRow;
//QVector<int> endRow;
//int iCount = 0;
//qreal left,top,right,bottom;
//printer->getPageMargins(&left,&top,&right,&bottom,QPrinter::Point);
////cout << "left = " << left << ", right = " << right << ", top = " << top << ", bottom = " << bottom << endl;
//area.setHeight( int(area.height() - top - bottom) );
//area.setWidth( int(area.width() - left - right) );

//for (int p=0; p < rows; p++) //rows中保存了所有的行数 ， 找出每次打印的开始行和结束行
//{
//    totalPageHeight += view->rowHeight(p); //求出所有的列高度
//    int pageFooter = 50 ;  //页脚的高度
//    int pageHeader = 50;   //页眉的高度

//    if ((totalPageHeight >= (area.height() - pageFooter - pageHeader)) || (p == rows -1))  //如果目前累加列的高度大于或者等于可用页面高度 || 到达最后一行
//    {
//        totalPageHeight = view->rowHeight(p);
//        if (p == rows -1)
//        {
//            if (p - iCount - 2 < 0)
//            {
//                startRow.push_back(0);
//            }
//            else
//            {
//                startRow.push_back(p - iCount - 1); //p - iCount - 2
//            }
//            endRow.push_back(p);
//            //cout << startRow.at(startRow.size() - 1) << " " << endRow.at(endRow.size() - 1) << endl;
//        }
//        else
//        {
//            if (p - iCount - 2 < 0)
//            {
//                startRow.push_back(0);
//                endRow.push_back(p - 2);
//            }
//            else
//            {
//                startRow.push_back(p - iCount - 1);
//                endRow.push_back(p - 2); // p - 3
//            }
//            //cout << startRow.at(startRow.size() - 1) << " " << endRow.at(endRow.size() - 1) << endl;
//        }
//        iCount = 0;
//    }
//    iCount++;
//}

//QTableWidget *printTable = new QTableWidget();
////printTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
////printTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
////printTable->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
////printTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
//const double xscale = area.width() / totalWidth;
//const double yscale = area.height() / totalHeight;
////const double pscale = area.height() / totalPageHeight;
////painter.scale(xscale , yscale); // With the scaling 3 It is Printing all
////painter.translate(area.x() + xscale, area.y() + pscale); //This is original
//painter.translate(area.x() + xscale, area.y() + yscale);
////painter.save(); //commented
////int y=0 ;// This is introduced for the columns
////view->paintEvent(&event);
////QPainter paint(this);
////paint.setPen(Qt::red);
////paint.drawRect(0, 0, 0, 0);
//QStyleOptionViewItem option;
//QPen pen;
//pen.setColor(Qt::black);
//pen.setWidth(1);
//painter.setPen(pen);

//int irowcount = -1;
//float rate = 1;
//for(int ipage = 0; ipage < startRow.size();ipage++)  //startRow有大，就有多少页需要打印
//{
//    printTable->setColumnCount(view->columnCount() + 1);  //打印的列数为view的列数
//    printTable->setRowCount(endRow[ipage] - startRow[ipage] +2);  //设置当前打印页的行数

//    for(int c = 1;c < cols + 1;c++)
//    {
//        printTable->setColumnWidth( c,(view->columnWidth(c - 1)) );
//        QTableWidgetItem *newItem = printTable->item(0,c);
//        if (!newItem)  //如果printTable的首行不存在，添加文字或者序号
//        {
//            if (view->horizontalHeaderItem(c - 1)) //Returns the horizontal(水平的) header item for column
//                newItem = new QTableWidgetItem(view->horizontalHeaderItem(c - 1)->text());
//            else
//                newItem = new QTableWidgetItem(QString::number(c));
//            printTable->setItem(0,c ,newItem);
//        }
//        else    //否则，直接设置文字或者序号
//        {
//            if (view->horizontalHeaderItem(c - 1))
//                newItem->setText(view->horizontalHeaderItem(c - 1)->text());
//            else
//                newItem->setText(QString::number(c));
//        }
//        //printTable->verticalHeader()->hide();
//        //printTable->horizontalHeader()->hide();
//    }

///////////////////////////求出合适的比率，用来完美表格的显示
//    QRect firstTable , lastTable;
//    QModelIndex firstIndex = printTable->model()->index(0,0);
//    QModelIndex lastIndex = printTable->model()->index(0,cols);
//    firstTable = printTable->visualRect(firstIndex);
//    lastTable =  printTable->visualRect(lastIndex);
//    float totalTableWidth = lastTable.topRight().x() - firstTable.topLeft().x();
//    float pagerWidth = printer->paperRect().width() - 20 - 20;
//    rate = totalTableWidth/pagerWidth;
//    //cout << "totalTableWidth = " << totalTableWidth << ", pagerWidth = " << pagerWidth << ", rate = " << rate << endl;
///////////////////////////求出合适的比率，用来完美表格的显示

//    for(int c = 1;c < cols + 1;c++)  //重设宽度
//    {
//        printTable->setColumnWidth( c,int((view->columnWidth(c - 1))/rate) );
//    }
//    printTable->setColumnWidth(0,view->verticalHeader()->width());  //Returns the table view's vertical header.
//    int iRow = 0;
//    for(int ir = startRow[ipage]; ir <= endRow[ipage];ir++) //ir变量用来标识当前打印的是哪些行，设置当前打印页的首行
//    {
//        printTable->setRowHeight(iRow,view->rowHeight(ir));  //Sets the height of the given row to be height.
//        for(int ic = 0; ic < view->columnCount(); ic++)  //设置每个单元格的内容
//        {
//            QTableWidgetItem *newItem = printTable->item(iRow  + 1,ic + 1);
//            if (!newItem)
//            {
//                if (view->item(ir,ic))
//                    newItem = new QTableWidgetItem(view->item(ir,ic)->text());
//                else
//                    newItem = new QTableWidgetItem("");
//                printTable->setItem(iRow + 1,ic + 1,newItem);
//            }
//            else
//            {
//                if (view->item(ir,ic))
//                    newItem->setText(view->item(ir,ic)->text());
//                else
//                    newItem->setText("");
//            }
//        }
//        iRow++;
//    }
//    qreal ioffx = (area.width() - totalWidth)/2;
//    qreal ioffy = 50;
//   /*首页的曲线打印
//    if (ipage == 0)
//    {
//        ioffy = 500;
//        QwtPlotPrintFilter filter;
//        int options = QwtPlotPrintFilter::PrintAll;
//        options &= ~QwtPlotPrintFilter::PrintBackground;
//        options |= QwtPlotPrintFilter::PrintFrameWithScales;
//        filter.setOptions(options);
//        d_plot->print(&painter,QRect(50,65,650,400),filter);

//    }
//    */
//    rows = printTable->rowCount();
//    iRow = 1;
//    for(int ir = startRow[ipage] ; ir <= endRow[ipage] ; ir++)  //ir变量用来标识当前打印的是哪些行，设置当前打印页的首列序号
//    {
//        QTableWidgetItem *newItem = printTable->item(iRow,0);
//        if (!newItem)
//        {
//            if (view->verticalHeaderItem(ir))
//                newItem = new QTableWidgetItem(view->verticalHeaderItem(ir)->text());
//            else
//                newItem = new QTableWidgetItem(QString::number(iRow));
//            printTable->setItem(iRow,0,newItem);
//        }
//        else
//        {
//            if (view->verticalHeaderItem(ir))
//                newItem->setText(view->verticalHeaderItem(ir)->text());
//            else
//                newItem->setText(QString::number(iRow));
//        }
//        iRow ++;
//    }

//    QPointF offPt(20,ioffy);  //20表示边框的空格

//    int rr,cc,rowSpan,colSpan;
//    rr=0;cc=0; rowSpan=3, colSpan=2;//合并的单元格，测试用，此信息需要保存

//    for (int r = 0; r < rows-1 /*rows*/; r++)   //画横线，若有合并的单元格，需要修改，分两次画
//    {
//         if(r==rr && rowSpan!=1 && rr>=startRow[ipage]) //增加合并单元格的处理,判断startRow为了定位合并单元格所在的页
//         {
//             QRect rt1,rt2;
//             QModelIndex idx1 = printTable->model()->index(r,0);
//             QModelIndex idx2 = printTable->model()->index(r,cc-1);
//             rt1 = printTable->visualRect(idx1);
//             rt2 = printTable->visualRect(idx2);
//             if(cc!=0)
//             {
//                painter.drawLine( (rt1.bottomLeft() + offPt) , (rt2.bottomRight() + offPt) ); //合并单元格前面的横线
//             }

//             idx1 = printTable->model()->index(r,cc+colSpan);
//             idx2 = printTable->model()->index(r,cols-1);
//             rt1 = printTable->visualRect(idx1);
//             rt2 = printTable->visualRect(idx2);
//             painter.drawLine( (rt1.bottomLeft() + offPt) , (rt2.bottomRight() + offPt) ); //合并单元格后面的横线

//             rr++;
//             rowSpan--;

//             if (r == 0)
//             {
//                 idx1 = printTable->model()->index(r,0);
//                 idx2 = printTable->model()->index(r,cols-1/*cols*/);
//                 rt1 = printTable->visualRect(idx1);
//                 rt2 = printTable->visualRect(idx2);
//                 painter.drawLine( (rt1.topLeft() + offPt) , (rt2.topRight() + offPt) );
//             }
//         }
//         else //
//         {
//            QRect rt1,rt2;
//            QModelIndex idx1 = printTable->model()->index(r,0);
//            QModelIndex idx2 = printTable->model()->index(r,cols-1/*cols*/);
//            rt1 = printTable->visualRect(idx1);
//            rt2 = printTable->visualRect(idx2);
//            painter.drawLine( (rt1.bottomLeft() + offPt) , (rt2.bottomRight() + offPt) );
//            if (r == 0)
//            {
//                painter.drawLine( (rt1.topLeft() + offPt) , (rt2.topRight() + offPt) );
//            }
//        }
//    }

//    rr=0;cc=0; rowSpan=3, colSpan=2; //合并的单元格，测试用，此信息需要保存

//    for (int c = 0; c < cols /*+ 1*/; c++)      //画竖线，若有合并的单元格，需要修改，分两次画
//    {
//        if(cc==(c-1) && colSpan!=1 &&rr>=startRow[ipage]) //增加合并单元格的处理, 判断startRow为了定位合并单元格所在的页
//        {
//            QRect rt1,rt2;
//            QModelIndex idx1 = printTable->model()->index(0,c);
//            QModelIndex idx2 = printTable->model()->index(rr-1,c);
//            rt1 = printTable->visualRect(idx1);
//            rt2 = printTable->visualRect(idx2);
//            if(rr!=0)
//            {
//               painter.drawLine( (rt1.topLeft() + offPt) , (rt2.bottomLeft() + offPt) ); //合并单元格前面的竖线
//            }

//            idx1 = printTable->model()->index(rr+rowSpan,c);
//            idx2 = printTable->model()->index(rows-2,c);
//            rt1 = printTable->visualRect(idx1);
//            rt2 = printTable->visualRect(idx2);
//            painter.drawLine( (rt1.topLeft() + offPt) , (rt2.bottomLeft() + offPt) );//合并单元格后面的竖线

//            cc++;
//            colSpan--;

//            if (c == cols-1/*cols*/)
//            {
//                idx1 = printTable->model()->index(0,c);
//                idx2 = printTable->model()->index(rows-2 /*- 1*/,c);
//                rt1 = printTable->visualRect(idx1);
//                rt2 = printTable->visualRect(idx2);
//                painter.drawLine( (rt1.topRight() + offPt) , (rt2.bottomRight() + offPt) );
//            }
//        }
//        else
//        {
//            QRect rt1,rt2;
//            QModelIndex idx1 = printTable->model()->index(0,c);
//            QModelIndex idx2 = printTable->model()->index(rows-2 /*- 1*/,c);
//            rt1 = printTable->visualRect(idx1);
//            rt2 = printTable->visualRect(idx2);
//            painter.drawLine( (rt1.topLeft() + offPt) , (rt2.bottomLeft() + offPt) );
//            if (c == cols-1/*cols*/)
//            {
//                painter.drawLine( (rt1.topRight() + offPt) , (rt2.bottomRight() + offPt) );
//            }
//        }
//    }

//    rr=0;cc=0; rowSpan=3, colSpan=2;//合并的单元格，测试用，此信息需要保存
//    bool isDraw=FALSE;

//    for (int r = 1/*0*/; r < rows; r++) //表格内容输出，若有合并单元格，需要特殊处理，居中显示
//    {
//        irowcount ++;
//        for(int c =1 /*0*/; c < cols  + 1; c++)
//        {
//            if((r==(rr+1) && rowSpan!=0) &&(c==(cc+1) && colSpan!=0) &&rr>=startRow[ipage])//增加合并单元格的处理, 判断startRow为了定位合并单元格所在的页
//            {
//                if(!isDraw)
//                {
//                    QRect rt0,rt1,rt2;
//                    QModelIndex idx0 = printTable->model()->index(rr+2,cc+1);
//                    QModelIndex idx1 = printTable->model()->index(rr+2,cc+colSpan-1);
//                    QModelIndex idx2 = printTable->model()->index(rr+rowSpan,cc+1);
//                    rt0 = printTable->visualRect(idx0);
//                    rt1 = printTable->visualRect(idx1);
//                    rt2 = printTable->visualRect(idx2);

//                    if(printTable->item(rr+1,cc+1))
//                    {
//                        QRectF rt(rt0.left(),rt0.top(),rt1.right()-rt0.left(),rt2.bottom()-rt0.top());
//                        painter.drawText(rt,Qt::AlignHCenter,printTable->item(rr+1,cc+1)->text());
//                    }
//                }
//                isDraw=TRUE;

//                c+=cc+colSpan-2;
//                rr++;
//                rowSpan--;
//            }
//            else
//            {
//                QModelIndex idx = printTable->model()->index(r-1,c-1/*r,c*/);
//                option.rect = printTable->visualRect(idx);
//                if (printTable->item(r,c))
//                {
//                    QRectF rt(option.rect.left() + 3,option.rect.top(),option.rect.width(),option.rect.height());
//                    rt = rt.translated(offPt);
//                    if (r != 0 && c == 0)
//                    {
//                        painter.drawText(rt,Qt::AlignCenter,QString("%1").arg(irowcount));
//                    }
//                    else
//                    {
//                        if (r == 0)
//                        {
//                            painter.drawText(rt,Qt::AlignCenter,printTable->item(r,c)->text());
//                        }
//                        else
//                        {
//                            painter.drawText(rt,Qt::AlignVCenter,printTable->item(r,c)->text());
//                        }
//                    }
//                }
//            }
//        }
//    }
//    //ipage
//    if (ipage == 0)  //设置标题
//    {
//        QRectF rttt(area.left(),area.top(),area.width(),50);
//        painter.drawText(rttt,Qt::AlignCenter,stitile);
//    }
//    QRectF rttt(area.left(),area.bottom() - 65,area.width(),25);
//    painter.drawText(rttt,Qt::AlignCenter,QString::number(ipage + 1));
//    if (ipage < startRow.size() - 1)
//    {
//        printer->newPage();
//        irowcount--;
//    }
//}
//painter.end();

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
