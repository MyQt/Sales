#include "BillModel.h"
#include <QDebug>

BillModel::BillModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_billList.clear();
}

BillModel::~BillModel()
{
    m_billList.clear();
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
    if(index.isValid() && index.row() < m_billList.size()){
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
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= m_billList.count() ||
            index.column() < 0 || index.column() >= 11)
    {
        return QVariant();
    }
    if (index.row() < 0 || index.row() >= m_billList.count())
        return QVariant();

    BillData* bill = m_billList[index.row()];
    if(role == Qt::DisplayRole)
    {
      switch (index.column())
      {
        case 0:
          return bill->no();
          break;
        case 1:
            return bill->variety();
        break;

        case 2:
            return bill->childDetailCode(0);
        break;
        case 3:
          return bill->childDetailCode(1);
        break;
        case 4:
          return bill->childDetailCode(2);
        break;
        case 5:
          return bill->childDetailCode(3);
        break;
        case 6:
          return bill->childDetailCode(4);
        break;
        case 7:
          return bill->detailCodeNum();
        case 8:
          return bill->billNum();
        break;
        case 9:
          return bill->price();
          break;
        case 10:
          return bill->billPrice();
          break;
        default:
          break;
      }
    }
//    if(role == BillID){
//        return bill->id();
//    }else if(role == BillNo)
//    {
//        return bill->no();
//    }else if(role == BillVariety)
//    {
//        return bill->variety();
//    }else if(role == BillDetailCode)
//    {
//        return bill->detailCode();
//    }else if(role == BillPrice)
//    {
//        return bill->price();
//    }else if(role == BillCustomer)
//    {
//        return bill->customer();
//    }else if(role == BillComment)
//    {
//        return bill->comment();
//    }

    return QVariant();
}

bool BillModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    BillData* bill = m_billList[index.row()];
    if(role == Qt::DisplayRole)
    {
      switch (index.column())
      {
        case 0:
            bill->setNo(value.toString());
          break;
        case 1:
            bill->setVariety(value.toString());
        break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            bill->setChildDetailCode(value.toString());
          break;
        case 9:
            bill->setPrice(value.toString());
          break;
        default:
          break;
      }
    }

//    if(role == BillID){
//        bill->setId(value.toString());
//    }else if(role == BillNo)
//    {
//        bill->setNo(value.toString());
//    }else if(role == BillVariety)
//    {
//        bill->setVariety(value.toString());
//    }else if(role == BillDetailCode)
//    {
//        bill->setdetailCode(value.toString());
//    }else if(role == BillPrice)
//    {
//        bill->setPrice(value.toString());
//    }else if(role == BillCustomer)
//    {
//        bill->setCustomer(value.toString());
//    }else if(role == BillComment)
//    {
//        bill->setComment(value.toString());
//    }else{
//        return false;
//    }

    emit dataChanged(this->index(index.row(), index.column()),
                     this->index(index.row(), index.column()),
                     QVector<int>(1,role));

    return true;
}

int BillModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_billList.count();
}

int BillModel::columnCount(const QModelIndex &) const
{
    return 11;
}
void BillModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::stable_sort(m_billList.begin(), m_billList.end(), [](BillData* lhs, BillData* rhs){
        return lhs->creationDateTime() > rhs->creationDateTime();
    });

    emit dataChanged(index(0,0), index(rowCount()-1, 9));
}


void BillModel::print()
{
    qDebug() << "打印\n";
}
