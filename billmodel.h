﻿#ifndef BillModel_H
#define BillModel_H

#include <QAbstractTableModel>
#include <QString>
#include "BillData.h"

class BillModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class tst_BillModel;

public:

    enum BillRoles{
        BillID = Qt::UserRole + 1,
        BillNo,
        BillVariety,
        BillDetailCode,
        BillPrice,
        BillCustomer,
        BillComment,
        BillCreationDateTime
    };

    explicit BillModel(QObject *parent = Q_NULLPTR);
    ~BillModel();

    QModelIndex addBill(BillData* bill);
    BillData* getBill(const QModelIndex& index);
    void addListBill(QList<BillData*> billList);
    BillData* removeBill(const QModelIndex& billIndex);
    bool moveRow(const QModelIndex& sourceParent,
                 int sourceRow,
                 const QModelIndex& destinationParent,
                 int destinationChild);

    void clearBills();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &) const;
    int getExistedItem(BillData* bill);
    int getLastExistedItem(BillData* bill);
    void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;
    void print();
    QList<BillData *>& getAllBills() { return m_billList; }
private:
    QModelIndex insertBill(BillData* bill, int row);
private:
    QList<BillData *> m_billList;
signals:
    void billRemoved();
};

#endif // BillModel_H
