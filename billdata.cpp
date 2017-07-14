#include "BillData.h"
#include <QDataStream>

BillData::BillData(QObject *parent)
    : QObject(parent),
      m_isSelected(false)
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
    int nNum = 0;
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

    return *this;
}

QDataStream &operator<<(QDataStream &stream, const BillData* billData) {
    return stream << billData->id() << billData->no() << billData->variety() << billData->detailCode() << billData->price() << billData->customer() << billData->comment() << billData->creationDateTime();
}

QDataStream &operator>>(QDataStream &stream, BillData* &billData){
    billData = new BillData();
    QString id;
    QString number; // 编号
    QString variety; // 品种
    QString detail_code; // 明细码
    QString price; // 单价
    QString customer; // 客户
    QString comment; // 备注
    QDateTime creationDateTime;
    stream >> id >> number >> variety >> detail_code >> price >> customer >> comment >> creationDateTime;
    billData->setId(id);
    billData->setNo(number);
    billData->setVariety(variety);
    billData->setdetailCode(detail_code);
    billData->setPrice(price);
    billData->setCustomer(customer);
    billData->setComment(comment);
    billData->setCreationDateTime(creationDateTime);

    return stream;
}

