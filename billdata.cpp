#include "BillData.h"
#include <QDataStream>

BillData::BillData(QObject *parent)
    : QObject(parent),
      m_isModified(false),
      m_isSelected(false),
      m_scrollBarPosition(0)
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

QString BillData::fullTitle() const
{
    return m_fullTitle;
}

void BillData::setFullTitle(const QString &fullTitle)
{
    m_fullTitle = fullTitle;
}

QDateTime BillData::lastModificationdateTime() const
{
    return m_lastModificationDateTime;
}

void BillData::setLastModificationDateTime(const QDateTime &lastModificationdateTime)
{
    m_lastModificationDateTime = lastModificationdateTime;
}

QString BillData::content() const
{
    return m_content;
}

void BillData::setContent(const QString &content)
{
    m_content = content;
}

bool BillData::isModified() const
{
    return m_isModified;
}

void BillData::setModified(bool isModified)
{
    m_isModified = isModified;
}

bool BillData::isSelected() const
{
    return m_isSelected;
}

void BillData::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}

int BillData::scrollBarPosition() const
{
    return m_scrollBarPosition;
}

void BillData::setScrollBarPosition(int scrollBarPosition)
{
    m_scrollBarPosition = scrollBarPosition;
}

QDateTime BillData::deletionDateTime() const
{
    return m_deletionDateTime;
}

void BillData::setDeletionDateTime(const QDateTime& deletionDateTime)
{
    m_deletionDateTime = deletionDateTime;
}

QDateTime BillData::creationDateTime() const
{
    return m_creationDateTime;
}

void BillData::setCreationDateTime(const QDateTime&creationDateTime)
{
    m_creationDateTime = creationDateTime;
}

QDataStream &operator<<(QDataStream &stream, const BillData* billData) {
    return stream << billData->id() << billData->fullTitle() << billData->creationDateTime() << billData->lastModificationdateTime() << billData->content();
}

QDataStream &operator>>(QDataStream &stream, BillData* &billData){
    billData = new BillData();
    QString id;
    QString fullTitle;
    QDateTime lastModificationDateTime;
    QDateTime creationDateTime;
    QString content;
    stream >> id >> fullTitle >> creationDateTime >> lastModificationDateTime >> content;
    billData->setId(id);
    billData->setFullTitle(fullTitle);
    billData->setLastModificationDateTime(lastModificationDateTime);
    billData->setCreationDateTime(creationDateTime);
    billData->setContent(content);
    return stream;
}

