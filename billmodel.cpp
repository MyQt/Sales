#include "BillModel.h"
#include <QDebug>

BillModel::BillModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

BillModel::~BillModel()
{

}

QModelIndex BillModel::addBill(BillData* bill)
{
    const int rowCnt = rowCount();
    beginInsertRows(QModelIndex(), rowCnt, rowCnt);
    m_billList << bill;
    endInsertRows();

    return createIndex(rowCnt, 0);
}

QModelIndex BillModel::insertBill(BillData *bill, int row)
{
    if(row >= rowCount()){
        return addBill(bill);
    }else{
        beginInsertRows(QModelIndex(), row, row);
        m_billList.insert(row, bill);
        endInsertRows();
    }

    return createIndex(row,0);
}

BillData* BillModel::getBill(const QModelIndex& index)
{
    if(index.isValid()){
        return m_billList.at(index.row());
    }else{
        return Q_NULLPTR;
    }
}

void BillModel::addListBill(QList<BillData *> billList)
{
    int start = rowCount();
    int end = start + billList.count()-1;
    beginInsertRows(QModelIndex(), start, end);
    m_billList << billList;
    endInsertRows();
}

BillData* BillModel::removeBill(const QModelIndex &billIndex)
{
    int row = billIndex.row();
    beginRemoveRows(QModelIndex(), row, row);
    BillData* bill = m_billList.takeAt(row);
    endRemoveRows();

    return bill;
}

bool BillModel::moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild)
{
    if(sourceRow<0
            || sourceRow >= m_billList.count()
            || destinationChild <0
            || destinationChild >= m_billList.count()){

        return false;
    }

    beginMoveRows(sourceParent,sourceRow,sourceRow,destinationParent,destinationChild);
    m_billList.move(sourceRow,destinationChild);
    endMoveRows();

    return true;
}

void BillModel::clearBills()
{
    beginResetModel();
    m_billList.clear();
    endResetModel();
}

QVariant BillModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_billList.count())
        return QVariant();

    BillData* bill = m_billList[index.row()];
    if(role == BillID){
        return bill->id();
    }else if(role == BillFullTitle){
        return bill->fullTitle();
    }else if(role == BillCreationDateTime){
        return bill->creationDateTime();
    }else if(role == BillLastModificationDateTime){
        return bill->lastModificationdateTime();
    }else if(role == BillDeletionDateTime){
        return bill->deletionDateTime();
    }else if(role == BillContent){
        return bill->content();
    }else if(role == BillScrollbarPos){
        return bill->scrollBarPosition();
    }

    return QVariant();
}

bool BillModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    BillData* bill = m_billList[index.row()];


    if(role == BillID){
        bill->setId(value.toString());
    }else if(role == BillFullTitle){
        bill->setFullTitle(value.toString());
    }else if(role == BillCreationDateTime){
        bill->setCreationDateTime(value.toDateTime());
    }else if(role == BillLastModificationDateTime){
        bill->setLastModificationDateTime(value.toDateTime());
    }else if(role == BillDeletionDateTime){
        bill->setDeletionDateTime(value.toDateTime());
    }else if(role == BillContent){
        bill->setContent(value.toString());
    }else if(role == BillScrollbarPos){
        bill->setScrollBarPosition(value.toInt());
    }else{
        return false;
    }

    emit dataChanged(this->index(index.row()),
                     this->index(index.row()),
                     QVector<int>(1,role));

    return true;
}

Qt::ItemFlags BillModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEditable ;
}

int BillModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_billList.count();
}

void BillModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::stable_sort(m_billList.begin(), m_billList.end(), [](BillData* lhs, BillData* rhs){
        return lhs->lastModificationdateTime() > rhs->lastModificationdateTime();
    });

    emit dataChanged(index(0), index(rowCount()-1));
}
