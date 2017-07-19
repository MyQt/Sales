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
    int rowCnt = rowCount();

    int existedIndex = getExistedItem(bill);
    if (existedIndex != -1)
    {
        // 加上该清单项客户编号品种的布匹的件数，数量，金额
        BillData* pTarget = m_billList.at(existedIndex);

        if (!pTarget->isRepeated())
        {
            QString detailCodeNum, billNum, billPrice;
            detailCodeNum.setNum(bill->detailCodeNum());
            billNum.setNum(bill->billNum());
            billPrice.setNum(bill->billPrice(), 'f', 2);
            pTarget->addTotalDetailCodeNum(detailCodeNum);
            pTarget->addTotalBillNum(billNum);
            pTarget->addTotalBillPrice(billPrice);
        }

        int lastExistedIndex = getLastExistedItem(bill);
        return insertBill(bill, lastExistedIndex+1);
    } else {
        QString detailCodeNum, billNum, billPrice;
        detailCodeNum.setNum(bill->detailCodeNum());
        billNum.setNum(bill->billNum());
        billPrice.setNum(bill->billPrice(), 'f', 2);
        bill->addTotalDetailCodeNum(detailCodeNum);
        bill->addTotalBillNum(billNum);
        bill->addTotalBillPrice(billPrice);
        int insertRow = getPreBillRow(bill);
        beginInsertRows(QModelIndex(), insertRow, insertRow);
        m_billList.insert(insertRow, bill);
        endInsertRows();
        rowCnt = insertRow;
    }
    return createIndex(rowCnt, 0);
}

QModelIndex BillModel::insertBill(BillData *bill, int row)
{
    if(row >= rowCount()){
        beginInsertRows(QModelIndex(), row, row);
        m_billList << bill;
        endInsertRows();
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
    for (int i = 0; i < billList.size(); i++)
    {
        addBill(billList.at(i)) ;
    }
}

BillData* BillModel::removeBill(const QModelIndex &billIndex)
{
    int row = billIndex.row();
    beginRemoveRows(QModelIndex(), row, row);
    BillData* bill = m_billList.takeAt(row);
    endRemoveRows();
    // 减去该清单项客户编号品种的布匹的件数，数量，金额
    for (int i = 0; i < m_billList.size(); i++)
    {
        BillData* pData = m_billList.at(i);
        if (pData != Q_NULLPTR && !pData->isRepeated() && pData->no() == bill->no() && pData->variety() == bill->variety() && pData->customer() == bill->customer())
        {
            QString detailCodeNum, billNum, billPrice;
            detailCodeNum.setNum(bill->detailCodeNum());
            billNum.setNum(bill->billNum());
            billPrice.setNum(bill->billPrice(), 'f', 2);
            pData->decTotalDetailCodeNum(detailCodeNum);
            pData->decTotalBillNum(billNum);
            pData->decTotalBillPrice(billPrice);
            break;
        }
    }
    return bill;
}

void BillModel::removeBillByCustomer(const QString &customer)
{
    for (int i = 0; i < m_billList.size(); i++)
    {
        if (m_billList.at(i)->customer() == customer)
        {
            beginRemoveRows(QModelIndex(), i, i);
            BillData* bill = m_billList.takeAt(i);
            endRemoveRows();
        }
    }
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
            index.column() < 0 || index.column() >= 13)
    {
        return QVariant();
    }
    if (index.row() < 0 || index.row() >= m_billList.count())
        return QVariant();

    BillData* bill = m_billList[index.row()];
    if (role==Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }else if(role == Qt::DisplayRole)
    {
      switch (index.column())
      {
        case 0:
        {
            if (bill->isRepeated())
                return "";
            else
                return bill->no();
        }
          break;
        case 1:
        {
          if (bill->isRepeated())
              return "";
          else
              return bill->variety();
        }
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
        {
          if (bill->isRepeated())
              return "";
          else
              return bill->getTotalDetailCodeNum();
        }
        case 8:
        {
          if (bill->isRepeated())
              return "";
          else
              return bill->getTotalBillNum();
        }
        break;
        case 9:
        {
          if (bill->isRepeated())
              return "";
          else
              return bill->price();
        }
          break;
        case 10:
        {
          if (bill->isRepeated())
              return "";
          else
              return bill->getTotalBillPrice();
        }
          break;
        case 11:
            return bill->comment();
          break;
        case 12:
            return bill->customer();
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
        case 11:
            bill->setComment(value.toString());
        case 12:
            bill->setCustomer(value.toString());
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

int BillModel::columnCount(const QModelIndex &parent) const
{
    return 13;
}
void BillModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::stable_sort(m_billList.begin(), m_billList.end(), [](BillData* lhs, BillData* rhs){
        if (lhs->no() < rhs->no())
            return true;
        if (lhs->variety() < rhs->variety())
            return true;
        if (lhs->creationDateTime() < rhs->creationDateTime())
            return true;

        return false;
    });

    emit dataChanged(index(0,0), index(rowCount()-1, 12));
}


void BillModel::print()
{
    qDebug() << "打印\n";
}

int BillModel::getExistedItem(BillData *bill)
{
    int index = -1;
    for (int i = 0; i < m_billList.size(); i++)
    {
        BillData* pData = m_billList.at(i);
        if (pData != Q_NULLPTR)
        {
            if (bill->no() == pData->no() && bill->variety() == pData->variety() && bill->customer() == pData->customer())
            {
                bill->setRepeated(true);
                index = i;
                break;
            }
        }
    }

    return index;
}

int BillModel::getLastExistedItem(BillData *bill)
{
    int index = -1;
    for (int i = 0; i < m_billList.size(); i++)
    {
        BillData* pData = m_billList.at(i);
        if (pData != Q_NULLPTR)
        {
            if (bill->no() == pData->no() && bill->variety() == pData->variety() && bill->customer() == pData->customer())
            {
                index = i;
            } else if (index != -1) // 该清单与目标不同且已经找到了，退出循环
            {
                break;
            }
        }
    }

    return index;
}

int BillModel::getPreBillRow(BillData *bill)
{
    int row = 0;
    for (int i = 0; i < m_billList.size(); i++)\
    {
        BillData* billData = m_billList.at(i);
        if (billData != Q_NULLPTR && billData->no() < bill->no())
            row++;
    }

    return row;
}

QList<BillData *> BillModel::getBillsByCustomer(QString& customer)
{
    QList<BillData *> billList;
    billList.clear();
    for (int i = 0; i < m_billList.size(); i++)\
    {
        BillData* billData = m_billList.at(i);
        if (billData != Q_NULLPTR && customer == billData->customer())
            billList.push_back(billData);
    }

    return billList;
}

QString BillModel::getTotalMoneyByCustomer(QString &customer)
{
    float totalMoney = 0;
    QList<BillData *> billList = getBillsByCustomer(customer);
    for (int i = 0; i < billList.size(); i++)
    {
        BillData* pData = billList.at(i);
        if (pData != Q_NULLPTR && !pData->isRepeated())
        {
           totalMoney += pData->getTotalBillPrice().toFloat();
        }
    }

    return QString::number((int)totalMoney);
}

QVariant BillModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal)
    {
        return QVariant();
    }
    else if(orientation == Qt::Vertical)
    {
        return QVariant(section+1);
    }

    return QVariant();
}

int BillModel::getLastID() const
{
    int lastID = 0;
    for (int i = 0; i < m_billList.size(); i++)
    {
        BillData* pData = m_billList.at(i);
        if (pData != Q_NULLPTR)
        {
            if (lastID < pData->id().toInt())
            {
                lastID = pData->id().toInt();
            }
        }
    }

    return lastID;
}
