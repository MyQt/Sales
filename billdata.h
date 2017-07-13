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
    QString price() const;
    void setPrice(const QString &price);
    QString customer() const;
    void setCustomer(const QString &customer);
    QString comment() const;
    void setComment(const QString &comment);
    QDateTime creationDateTime() const;
    void setCreationDateTime(const QDateTime& creationDateTime);

    bool isSelected() const;
    void setSelected(bool isSelected);

    int scrollBarPosition() const;
    void setScrollBarPosition(int scrollBarPosition);

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
    int m_scrollBarPosition;
};
QDataStream &operator<<(QDataStream &stream, const BillData* BillData);
QDataStream &operator>>(QDataStream &stream, BillData *&BillData);

#endif // BillData_H
