#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "BillData.h"
#include <QObject>
#include <QtSql/QSqlDatabase>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(const QString& path, bool doCreate = false, QObject *parent = 0);
    bool isBillExist(BillData* bill);
    void importBills(QList<BillData*> billList);
    void restoreBills(QList<BillData*> billList);

private:
    QSqlDatabase m_db;
    BillData* getBill(QString id);
    bool permanantlyRemoveAllBills();

signals:
    void billsReceived(QList<BillData*> billList);

public slots:
    QList<BillData*> getAllBills();
    bool addBill(BillData* bill);
    bool removeBill(BillData* bill);
    bool modifyBill(BillData* bill);
    bool migrateBill(BillData* bill);
    bool migrateTrash(BillData* bill);
    int getLastRowID();
};

#endif // DBMANAGER_H
