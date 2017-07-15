#include "BillData.h"
#include <QDataStream>

BillData::BillData(QObject *parent)
    : QObject(parent),
      m_isSelected(false),
      m_isRepeated(false)
{

}

QString BillData::id() const
{
    return m_id;
}

void BillData::setId(const QString &id)
{
    m_id = id;
}

QString BillData::no() const
{
    return m_no;
}

void BillData::setNo(const QString &no)
{
    m_no = no;
}

QString BillData::variety() const
{
    return m_variety;
}

void BillData::setVariety(const QString &variety)
{
    m_variety = variety;
}

QString BillData::detailCode() const
{
    return m_detail_code;
}

void BillData::setdetailCode(const QString &detailCode)
{
    m_detail_code = detailCode;
}

QString BillData::childDetailCode(int nIndex) const
{
    if (nIndex < 0 || nIndex >= 5 || nIndex >= detailCodeNum()) return "";
    QStringList myList = m_detail_code.split(",");

    return myList[nIndex];
}

void BillData::setChildDetailCode(QString childCode)
{
    if (!childCode.isEmpty())
        m_detail_code.append(childCode);
    if (detailCodeNum() < 5)
        m_detail_code.append(",");
}

int BillData::detailCodeNum() const
{
    QStringList myList = m_detail_code.split(",");

    return myList.count();
}

int BillData::billNum() const
{
    int num = 0;
    QStringList myList = m_detail_code.split(",");
    for (int i = 0; i < myList.size(); i++)
    {
        QString strDetailCode = myList[i];
        num = num + strDetailCode.toInt();
    }

    return num;
}

float BillData::billPrice() const
{
    return (float)(price().toFloat()*billNum());
}

QString BillData::price() const
{
    return m_price;
}

void BillData::setPrice(const QString &price)
{
    m_price = price;
}

QString BillData::customer() const
{
    return m_customer;
}

void BillData::setCustomer(const QString &customer)
{
    m_customer = customer;
}

QString BillData::comment() const
{
    return m_comment;
}

void BillData::setComment(const QString &comment)
{
    m_comment = comment;
}

bool BillData::isSelected() const
{
    return m_isSelected;
}

void BillData::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}

QDateTime BillData::creationDateTime() const
{
    return m_creationDateTime;
}

void BillData::setCreationDateTime(const QDateTime&creationDateTime)
{
    m_creationDateTime = creationDateTime;
}

bool BillData::isRepeated() const
{
    return m_isRepeated;
}

void BillData::setRepeated(bool isRepeated)
{
    m_isRepeated = isRepeated;
}

QString BillData::getTotalDetailCodeNum() const
{
    return m_totalDetailCodeNum;
}

void BillData::setTotalDetailCodeNum(QString totalDetailCodeNum)
{
    m_totalDetailCodeNum = totalDetailCodeNum;
}

void BillData::addTotalDetailCodeNum(QString addNum)
{
    int nAdd = addNum.toInt();
    int nTotalDetailCodeNum = m_totalDetailCodeNum.toInt();
    nTotalDetailCodeNum += nAdd;
    m_totalDetailCodeNum.setNum(nTotalDetailCodeNum);
}

void BillData::decTotalDetailCodeNum(QString decNum)
{
    int nDec = decNum.toInt();
    int nTotalDetailCodeNum = m_totalDetailCodeNum.toInt();
    nTotalDetailCodeNum -= nDec;
    m_totalDetailCodeNum.setNum(nTotalDetailCodeNum);
}

QString BillData::getTotalBillNum() const
{
    return m_totalBillNum;
}

void BillData::setTotalBillNum(QString totalBillNum)
{
    m_totalBillNum = totalBillNum;
}

void BillData::addTotalBillNum(QString addNum)
{
    int nAdd = addNum.toInt();
    int nTotalBillNum = m_totalBillNum.toInt();
    nTotalBillNum += nAdd;
    m_totalBillNum.setNum(nTotalBillNum);
}

void BillData::decTotalBillNum(QString decNum)
{
    int nDec = decNum.toInt();
    int nTotalBillNum = m_totalBillNum.toInt();
    nTotalBillNum -= nDec;
    m_totalBillNum.setNum(nTotalBillNum);
}

QString BillData::getTotalBillPrice() const
{
    return m_totalBillPrice;
}

void BillData::setTotalBillPrice(QString totalBillPrice)
{
    m_totalBillPrice = totalBillPrice;
}

void BillData::addTotalBillPrice(QString addPrice)
{
    float fAdd = addPrice.toFloat();
    float fTotalBillPrice = m_totalBillPrice.toFloat();
    fTotalBillPrice += fAdd;
    m_totalBillPrice.setNum(fTotalBillPrice, 'f', 2);
}

void BillData::decTotalBillPrice(QString decPrice)
{
    float fDec = decPrice.toFloat();
    float fTotalBillPrice = m_totalBillPrice.toFloat();
    fTotalBillPrice -= fDec;
    m_totalBillPrice.setNum(fTotalBillPrice, 'f', 2);
}

BillData& BillData::operator=(const BillData& billData)
{
    setId(billData.id());
    setNo(billData.no());
    setVariety(billData.variety());
    setdetailCode(billData.detailCode());
    setPrice(billData.price());
    setCustomer(billData.customer());
    setComment(billData.comment());
    setCreationDateTime(billData.creationDateTime());
    setRepeated(billData.isRepeated());

    return *this;
}

QDataStream &operator<<(QDataStream &stream, const BillData* billData) {
    return stream << billData->id() << billData->no() << billData->variety() << billData->detailCode() << billData->price() << billData->customer() << billData->comment() << billData->creationDateTime() << billData->isRepeated();
}

QDataStream &operator>>(QDataStream &stream, BillData* &billData){
    billData = new BillData();
    QString     id;
    QString     number; // 编号
    QString     variety; // 品种
    QString     detail_code; // 明细码
    QString     price; // 单价
    QString     customer; // 客户
    QString     comment; // 备注
    QDateTime   creationDateTime;
    bool        isRepeated;
    stream >> id >> number >> variety >> detail_code >> price >> customer >> comment >> creationDateTime >> isRepeated;
    billData->setId(id);
    billData->setNo(number);
    billData->setVariety(variety);
    billData->setdetailCode(detail_code);
    billData->setPrice(price);
    billData->setCustomer(customer);
    billData->setComment(comment);
    billData->setCreationDateTime(creationDateTime);
    billData->setRepeated(isRepeated);

    return stream;
}

