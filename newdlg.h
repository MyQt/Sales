#ifndef NEWDLG_H
#define NEWDLG_H

#include <QFrame>
#include <QDateTime>
#include <QList>

#include "billdata.h"

namespace Ui {
class NewDlg;
}

class NewDlg : public QFrame
{
    Q_OBJECT

public:
    explicit NewDlg(QWidget *parent = 0);
    ~NewDlg();
public:
    void init(QList<BillData *>& billlist, QString& customer);
private:
    void InitComboxItem(); // 添加新的编号到第一项
protected:
    virtual void closeEvent(QCloseEvent *event);
signals:
    void newItem(QString no,
    QString variety,
    QString detail_code,
    QString price,
    QString customer,
    QString comment);
private slots:
    void on_btnSure_clicked();
    void on_comboNo_currentIndexChanged(const QString &arg1);

private:
    QList<BillData *> m_billList;
    QString m_customer;
private:
    Ui::NewDlg *ui;
};

#endif // NEWDLG_H
