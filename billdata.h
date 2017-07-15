#ifndef BillData_H
#define BillData_H

#include <QObject>
#include <QDateTime>

class BillData : public QObject
{
    Q_OBJECT

    friend class tst_BillData;

public:
    explicit BillData(QObject *parent = Q_NULLPTR);

    QString id() const;
    void setId(const QString &id);
    QString no() const;
    void setNo(const QString &no);
    QString variety() const;
    void setVariety(const QString &variety);
    QString detailCode() const;
    void setdetailCode(const QString &detailCode);
    QString childDetailCode(int nIndex) const;
    void setChildDetailCode(QString childCode);
    int detailCodeNum() const; // 布匹明细码列数
    int billNum() const; // 布匹数量
    float billPrice() const; // 布匹金额
    QString price() const;
    void setPrice(const QString &price);
    QString customer() const;
    void setCustomer(const QString &customer);
    QString comment() const;
    void setComment(const QString &comment);
    QDateTime creationDateTime() const;
    void setCreationDateTime(const QDateTime& creationDateTime);
    bool isRepeated() const;
    void setRepeated(bool isRepeated);
    bool isSelected() const;
    void setSelected(bool isSelected);

    QString getTotalDetailCodeNum() const;
    void setTotalDetailCodeNum(QString totalDetailCodeNum);
    void addTotalDetailCodeNum(QString addNum);
    void decTotalDetailCodeNum(QString decNum);

    QString getTotalBillNum() const;
    void setTotalBillNum(QString totalBillNum);
    void addTotalBillNum(QString addNum);
    void decTotalBillNum(QString decNum);

    QString getTotalBillPrice() const;
    void setTotalBillPrice(QString totalBillPrice);
    void addTotalBillPrice(QString addPrice);
    void decTotalBillPrice(QString decPrice);

    BillData& operator=(const BillData& billData);

private:
    QString m_id;
    QString m_no; // 编号
    QString m_variety; // 品种
    QString m_detail_code; // 明细码
    QString m_price; // 单价
    QString m_customer; // 客户
    QString m_comment; // 备注
    QDateTime m_creationDateTime;
    bool m_isSelected;
    bool m_isRepeated; // 是否已经有同类的清单了(只有明细码不同)

    QString m_totalDetailCodeNum; // 同编号，品种，客户的总件数
    QString m_totalBillNum; // 同编号，品种，客户的总数量
    QString m_totalBillPrice; // 同编号，品种客户的总金额
};
QDataStream &operator<<(QDataStream &stream, const BillData* BillData);
QDataStream &operator>>(QDataStream &stream, BillData *&BillData);

#endif // BillData_H
