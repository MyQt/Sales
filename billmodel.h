#ifndef BillModel_H
#define BillModel_H

#include <QAbstractListModel>
#include "BillData.h"

class BillModel : public QAbstractListModel
{

    friend class tst_BillModel;

public:

    enum BillRoles{
        BillID = Qt::UserRole + 1,
        BillFullTitle,
        BillCreationDateTime,
        BillLastModificationDateTime,
        BillDeletionDateTime,
        BillContent,
        BillScrollbarPos
    };

    explicit BillModel(QObject *parent = Q_NULLPTR);
    ~BillModel();

    QModelIndex addBill(BillData* bill);
    QModelIndex insertBill(BillData* bill, int row);
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
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;

private:
    QList<BillData *> m_billList;

signals:
    void billRemoved();
};

#endif // BillModel_H
