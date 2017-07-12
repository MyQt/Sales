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

    QString fullTitle() const;
    void setFullTitle(const QString &fullTitle);

    QDateTime lastModificationdateTime() const;
    void setLastModificationDateTime(const QDateTime &lastModificationdateTime);

    QDateTime creationDateTime() const;
    void setCreationDateTime(const QDateTime& creationDateTime);

    QString content() const;
    void setContent(const QString &content);

    bool isModified() const;
    void setModified(bool isModified);

    bool isSelected() const;
    void setSelected(bool isSelected);

    int scrollBarPosition() const;
    void setScrollBarPosition(int scrollBarPosition);

    QDateTime deletionDateTime() const;
    void setDeletionDateTime(const QDateTime& deletionDateTime);


private:
    QString m_id;
    QString m_number; // 编号
    QString m_fullTitle;
    QString m_variety; // 品种
    QString m_detail_code; // 明细码
    QString m_price; // 单价
    QString m_sales; // 金额
    QString m_customer; // 客户
    QString m_tel; // 电话
    QString m_comment; // 备注
    QDateTime m_lastModificationDateTime;
    QDateTime m_creationDateTime;
    QDateTime m_deletionDateTime;
    QString m_content;
    bool m_isModified;
    bool m_isSelected;
    int m_scrollBarPosition;
};

QDataStream &operator<<(QDataStream &stream, const BillData* BillData);
QDataStream &operator>>(QDataStream &stream, BillData *&BillData);

#endif // BillData_H
