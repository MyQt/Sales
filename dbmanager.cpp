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
                         "creation_date INTEGER NOT NULL DEFAULT (0),"
                         "modification_date INTEGER NOT NULL DEFAULT (0),"
                         "deletion_date INTEGER NOT NULL DEFAULT (0),"
                         "content TEXT, "
                         "full_title TEXT);";

        query.exec(active);

        QString active_index = "CREATE UNIQUE INDEX active_index on active_bills (id ASC);";
        query.exec(active_index);

        QString deleted = "CREATE TABLE deleted_bills ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                          "creation_date INTEGER NOT NULL DEFAULT (0),"
                          "modification_date INTEGER NOT NULL DEFAULT (0),"
                          "deletion_date INTEGER NOT NULL DEFAULT (0),"
                          "content TEXT,"
                          "full_title TEXT)";
        query.exec(deleted);
    }
}

bool DBManager::isBillExist(BillData* bill)
{
    QSqlQuery query;

    int id = bill->id().split('_')[1].toInt();
    QString queryStr = QStringLiteral("SELECT EXISTS(SELECT 1 FROM active_bills WHERE id = %1 LIMIT 1 )")
                       .arg(id);
    query.exec(queryStr);
    query.next();

    return query.value(0).toInt() == 1;
}

BillData* DBManager::getBill(QString id) {
    QSqlQuery query;

    int parsedId = id.split('_')[1].toInt();
    QString queryStr = QStringLiteral("SELECT * FROM active_bills WHERE id = %1 LIMIT 1").arg(parsedId);
    query.exec(queryStr);

    if (query.first()) {
        BillData* bill = new BillData(this->parent() == Q_NULLPTR ? Q_NULLPTR : this);
        int id =  query.value(0).toInt();
        qint64 epochDateTimeCreation = query.value(1).toLongLong();
        QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation, QTimeZone::systemTimeZone());
        qint64 epochDateTimeModification= query.value(2).toLongLong();
        QDateTime dateTimeModification = QDateTime::fromMSecsSinceEpoch(epochDateTimeModification, QTimeZone::systemTimeZone());
        QString content = query.value(4).toString();
        QString fullTitle = query.value(5).toString();

        bill->setId(QStringLiteral("billID_%1").arg(id));
        bill->setCreationDateTime(dateTimeCreation);
        bill->setLastModificationDateTime(dateTimeModification);
        bill->setContent(content);
        bill->setFullTitle(fullTitle);
        return bill;
    }
    return Q_NULLPTR;
}

QList<BillData *> DBManager::getAllBills()
{
    QList<BillData *> billList;

    QSqlQuery query;
    query.prepare("SELECT * FROM active_bills");
    bool status = query.exec();
    if(status){
        while(query.next()){
            BillData* bill = new BillData(this);
            int id =  query.value(0).toInt();
            qint64 epochDateTimeCreation = query.value(1).toLongLong();
            QDateTime dateTimeCreation = QDateTime::fromMSecsSinceEpoch(epochDateTimeCreation, QTimeZone::systemTimeZone());
            qint64 epochDateTimeModification= query.value(2).toLongLong();
            QDateTime dateTimeModification = QDateTime::fromMSecsSinceEpoch(epochDateTimeModification, QTimeZone::systemTimeZone());
            QString content = query.value(4).toString();
            QString fullTitle = query.value(5).toString();

            bill->setId(QStringLiteral("billID_%1").arg(id));
            bill->setCreationDateTime(dateTimeCreation);
            bill->setLastModificationDateTime(dateTimeModification);
            bill->setContent(content);
            bill->setFullTitle(fullTitle);

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

    qint64 epochTimeDateCreated = bill->creationDateTime()
                                  .toMSecsSinceEpoch();
    QString content = bill->content()
                      .replace("'","''")
                      .replace(QChar('\x0'), emptyStr);
    QString fullTitle = bill->fullTitle()
                        .replace("'","''")
                        .replace(QChar('\x0'), emptyStr);

    qint64 epochTimeDateLastModified = bill->lastModificationdateTime().isNull() ? epochTimeDateCreated :  bill->lastModificationdateTime().toMSecsSinceEpoch();
    // 参数-1表示不需要参数值传入
    QString queryStr = QString("INSERT INTO active_bills (creation_date, modification_date, deletion_date, content, full_title) "
                               "VALUES (%1, %2, -1, '%3', '%4');")
                       .arg(epochTimeDateCreated)
                       .arg(epochTimeDateLastModified)
                       .arg(content)
                       .arg(fullTitle);

    query.exec(queryStr);

    if (this->parent() == Q_NULLPTR) {
        delete bill;
    }
    return (query.numRowsAffected() == 1);
}

bool DBManager::removeBill(BillData* bill)
{
    QSqlQuery query;
    QString emptyStr;

    int id = bill->id().split('_')[1].toInt();

    QString queryStr = QStringLiteral("DELETE FROM active_bills "
                                      "WHERE id=%1")
                       .arg(id);
    query.exec(queryStr);
    bool removed = (query.numRowsAffected() == 1);

    qint64 epochTimeDateCreated = bill->creationDateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateModified = bill->lastModificationdateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateDeleted = bill->deletionDateTime().toMSecsSinceEpoch();
    QString content = bill->content()
                      .replace("'","''")
                      .replace(QChar('\x0'), emptyStr);
    QString fullTitle = bill->fullTitle()
                        .replace("'","''")
                        .replace(QChar('\x0'), emptyStr);

    queryStr = QString("INSERT INTO deleted_bills "
                       "VALUES (%1, %2, %3, %4, '%5', '%6');")
               .arg(id)
               .arg(epochTimeDateCreated)
               .arg(epochTimeDateModified)
               .arg(epochTimeDateDeleted)
               .arg(content)
               .arg(fullTitle);

    query.exec(queryStr);
    bool addedToTrashDB = (query.numRowsAffected() == 1);

    return (removed && addedToTrashDB);
}

bool DBManager::permanantlyRemoveAllBills() {
    QSqlQuery query;
    return query.exec(QString("DELETE FROM active_bills"));
}

bool DBManager::modifyBill(BillData* bill)
{
    QSqlQuery query;
    QString emptyStr;

    int id = bill->id().split('_')[1].toInt();
    qint64 epochTimeDateModified = bill->lastModificationdateTime().toMSecsSinceEpoch();
    QString content = bill->content()
                      .replace("'","''")
                      .replace(QChar('\x0'), emptyStr);
    QString fullTitle = bill->fullTitle()
                        .replace("'","''")
                        .replace(QChar('\x0'), emptyStr);

    QString queryStr = QStringLiteral("UPDATE active_bills "
                                      "SET modification_date=%1, content='%2', full_title='%3' "
                                      "WHERE id=%4")
                       .arg(epochTimeDateModified)
                       .arg(content)
                       .arg(fullTitle)
                       .arg(id);
    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
}

bool DBManager::migrateBill(BillData* bill)
{
    QSqlQuery query;

    QString emptyStr;

    int id = bill->id().split('_')[1].toInt();
    qint64 epochTimeDateCreated = bill->creationDateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateModified = bill->lastModificationdateTime().toMSecsSinceEpoch();
    QString content = bill->content()
                      .replace("'","''")
                      .replace(QChar('\x0'), emptyStr);
    QString fullTitle = bill->fullTitle()
                        .replace("'","''")
                        .replace(QChar('\x0'), emptyStr);

    QString queryStr = QString("INSERT INTO active_bills "
                               "VALUES (%1, %2, %3, -1, '%4', '%5');")
                       .arg(id)
                       .arg(epochTimeDateCreated)
                       .arg(epochTimeDateModified)
                       .arg(content)
                       .arg(fullTitle);

    query.exec(queryStr);
    return (query.numRowsAffected() == 1);
}

bool DBManager::migrateTrash(BillData* bill)
{
    QSqlQuery query;
    QString emptyStr;

    int id = bill->id().split('_')[1].toInt();
    qint64 epochTimeDateCreated = bill->creationDateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateModified = bill->lastModificationdateTime().toMSecsSinceEpoch();
    qint64 epochTimeDateDeleted = bill->deletionDateTime().toMSecsSinceEpoch();
    QString content = bill->content()
                      .replace("'","''")
                      .replace(QChar('\x0'), emptyStr);
    QString fullTitle = bill->fullTitle()
                        .replace("'","''")
                        .replace(QChar('\x0'), emptyStr);

    QString queryStr = QString("INSERT INTO deleted_bills "
                       "VALUES (%1, %2, %3, %4, '%5', '%6');")
               .arg(id)
               .arg(epochTimeDateCreated)
               .arg(epochTimeDateModified)
               .arg(epochTimeDateDeleted)
               .arg(content)
               .arg(fullTitle);

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
