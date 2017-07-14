#include "dbmanager.h"
#include <QtSql/QSqlQuery>
#include <QTimeZone>
#include <QDateTime>
#include <QDebug>
#include <QtConcurrent>

DBManager::DBManager(const QString& path, bool doCreate, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QList<BillData*> >("QList<BillData*>");

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()){
        qDebug() << "Error: connection with database fail";
    }else{
        qDebug() << "Database: connection ok";
    }

    if(doCreate){
        QSqlQuery query;
        QString active = "CREATE TABLE active_bills ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "no TEXT, "
                         "variety TEXT, "
                         "detail_code TEXT, "
                         "price FLOAT, "
                         "customer TEXT, "
                         "comment TEXT, "
                         "creation_date INTEGER NOT NULL DEFAULT (0));";

        query.exec(active);

        QString active_index = "CREATE UNIQUE INDEX active_index on active_bills (id ASC);";
        query.exec(active_index);
    }
}

bool DBManager::isBillExist(BillData* bill)
{
    QSqlQuery query;

    int id = bill->id().toInt();
    QString queryStr = QStringLiteral("SELECT EXISTS(SELECT 1 FROM active_bills WHERE id = %1 LIMIT 1 )")
                       .arg(id);
    query.exec(queryStr);
    query.next();

    return query.value(0).toInt() == 1;
}

QList<BillData*> DBManager::getBillsByNo(QString no) {
    QList<BillData *> billList;
    billList.clear();

    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT * FROM active_bills WHERE no = '%1'").arg(no));
    bool status = query.exec();
    if(status){
        while(query.next()){
            BillData* bill = new BillData(this);
            QString id = query.value(0).toString();
            QString no =  query.value(1).toString();
            QString variety = query.value(2).toString();
            QString detail_code = query.value(3).toString();
            QString price = query.value(4).toString();
            QString customer = query.value(5).toString();
            QString comment = query.value(6).toString();
            qint64 epochDateTimeCreation = query.value(7).toLongLong();
            QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation, QTimeZone::systemTimeZone());
            bill->setId(id);
            bill->setNo(no);
            bill->setVariety(variety);
            bill->setdetailCode(detail_code);
            bill->setPrice(price);
            bill->setCustomer(customer);
            bill->setComment(comment);
            bill->setCreationDateTime(dateTimeCreation);

            billList.push_back(bill);
        }

        emit billsReceived(billList);
    }

    return billList;
}

QList<BillData *> DBManager::getBillsByCustomer(const QString &customer)
{
    QList<BillData *> billList;
    billList.clear();

    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT * FROM active_bills WHERE customer = '%1'").arg(customer));
    bool status = query.exec();
    if(status){
        while(query.next()){
            BillData* bill = new BillData(this);
            QString id = query.value(0).toString();
            QString no =  query.value(1).toString();
            QString variety = query.value(2).toString();
            QString detail_code = query.value(3).toString();
            QString price = query.value(4).toString();
            QString customer = query.value(5).toString();
            QString comment = query.value(6).toString();
            qint64 epochDateTimeCreation = query.value(7).toLongLong();
            QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation, QTimeZone::systemTimeZone());
            bill->setId(id);
            bill->setNo(no);
            bill->setVariety(variety);
            bill->setdetailCode(detail_code);
            bill->setPrice(price);
            bill->setCustomer(customer);
            bill->setComment(comment);
            bill->setCreationDateTime(dateTimeCreation);

            billList.push_back(bill);
        }

        emit billsReceived(billList);
    }

    return billList;
}

QList<BillData *> DBManager::getAllBills()
{
    QList<BillData *> billList;
    billList.clear();

    QSqlQuery query;
    query.prepare("SELECT * FROM active_bills");
    bool status = query.exec();
    if(status){
        while(query.next()){
            BillData* bill = new BillData(this);
            QString id = query.value(0).toString();
            QString no =  query.value(1).toString();
            QString variety = query.value(2).toString();
            QString detail_code = query.value(3).toString();
            QString price = query.value(4).toString();
            QString customer = query.value(5).toString();
            QString comment = query.value(6).toString();
            qint64 epochDateTimeCreation = query.value(7).toLongLong();
            QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation, QTimeZone::systemTimeZone());
            bill->setId(id);
            bill->setNo(no);
            bill->setVariety(variety);
            bill->setdetailCode(detail_code);
            bill->setPrice(price);
            bill->setCustomer(customer);
            bill->setComment(comment);
            bill->setCreationDateTime(dateTimeCreation);

            billList.push_back(bill);
        }

        emit billsReceived(billList);
    }

    return billList;
}

bool DBManager::addBill(BillData* bill)
{
    QSqlQuery query;
    QString emptyStr;

    QString no = bill->no()
            .replace("'","''")
            .replace(QChar('\x0'), emptyStr);
    QString variety  = bill->variety()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    QString detail_code  = bill->detailCode()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    float price = bill->price().toFloat();
    QString customer  = bill->customer()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    QString comment  = bill->comment()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    qint64 epochTimeDateCreated = bill->creationDateTime()
                                  .toMSecsSinceEpoch();

    // 参数-1表示不需要参数值传入
    QString queryStr = QString("INSERT INTO active_bills (no, variety, detail_code, price, customer, comment, creation_date) "
                               "VALUES ('%1', '%2', '%3', %4, '%5', '%6', %7);")
                        .arg(no)
                        .arg(variety)
                        .arg(detail_code)
                        .arg(price)
                        .arg(customer)
                        .arg(comment)
                        .arg(epochTimeDateCreated);


    query.exec(queryStr);

    if (this->parent() == Q_NULLPTR) {
        delete bill;
    }
    return (query.numRowsAffected() == 1);
}

bool DBManager::removeBill(BillData* bill)
{
    QSqlQuery query;

    int id = bill->id().toInt();

    QString queryStr = QStringLiteral("DELETE FROM active_bills "
                                      "WHERE id=%1")
                       .arg(id);
    query.exec(queryStr);
    bool removed = (query.numRowsAffected() == 1);

    return (removed);
}

bool DBManager::removeBillsByCustomer(const QString &customer)
{
    QSqlQuery query;

    QString queryStr = QStringLiteral("DELETE FROM active_bills "
                                      "WHERE customer='%1'")
                       .arg(customer);
    query.exec(queryStr);
    bool removed = (query.numRowsAffected() == 1);

    return (removed);
}

bool DBManager::removeBillsByNo(const QString &no)
{
    QSqlQuery query;

    QString queryStr = QStringLiteral("DELETE FROM active_bills "
                                      "WHERE no='%1'")
                       .arg(no);
    query.exec(queryStr);
    bool removed = (query.numRowsAffected() == 1);

    return (removed);
}

bool DBManager::permanantlyRemoveAllBills() {
    QSqlQuery query;
    return query.exec(QString("DELETE FROM active_bills"));
}

bool DBManager::migrateBill(BillData* bill)
{
    QSqlQuery query;

    QString emptyStr;

    int id = bill->id().split('_')[1].toInt();
    QString no = bill->no()
            .replace("'","''")
            .replace(QChar('\x0'), emptyStr);
    QString variety  = bill->variety()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    QString detail_code  = bill->detailCode()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    float price = bill->price().toFloat();
    QString customer  = bill->customer()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    QString comment  = bill->comment()
                  .replace("'","''")
                  .replace(QChar('\x0'), emptyStr);
    qint64 epochTimeDateCreated = bill->creationDateTime()
                                  .toMSecsSinceEpoch();

    QString queryStr = QString("INSERT INTO active_bills "
                               "VALUES (%1, '%2', '%3', %4, '%5', '%6', '%7', %8);")
                        .arg(id)
                        .arg(no)
                        .arg(variety)
                        .arg(detail_code)
                        .arg(price)
                        .arg(customer)
                        .arg(comment)
                        .arg(epochTimeDateCreated);

    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
}

int DBManager::getLastRowID()
{
    QSqlQuery query;
    query.exec("SELECT seq from SQLITE_SEQUENCE WHERE name='active_bills';");
    query.next();
    return query.value(0).toInt();
}

void DBManager::importBills(QList<BillData*> billList) {
    QSqlDatabase::database().transaction();
    QtConcurrent::blockingMap(billList, [this] (BillData* bill) { this->addBill(bill); });
    QSqlDatabase::database().commit();
}

void DBManager::restoreBills(QList<BillData*> billList) {
    this->permanantlyRemoveAllBills();
    this->importBills(billList);
}
