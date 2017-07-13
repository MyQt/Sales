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
    QList<BillData*> getBillsByNo(QString no);

    bool permanantlyRemoveAllBills();

signals:
    void billsReceived(QList<BillData*> billList);

public slots:
    QList<BillData*> getAllBills();
    QList<BillData*> getBillsByCustomer(const QString &customer);
    bool addBill(BillData* bill);
    bool removeBill(BillData* bill);
    bool removeBillsByCustomer(const QString &customer);
    bool removeBillsByNo(const QString &no);
    bool migrateBill(BillData* bill); // 插入表单
    int getLastRowID();
};

#endif // DBMANAGER_H
