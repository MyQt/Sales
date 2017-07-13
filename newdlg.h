#ifndef NEWDLG_H
#define NEWDLG_H

#include <QFrame>
#include <QDateTime>

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
    void init();
signals:
    void newItem(QString no,
    QString variety,
    QString detail_code,
    QString price,
    QString customer,
    QString comment,
    QDateTime creationDateTime);
private slots:
    void on_btnSure_clicked();

private:
    Ui::NewDlg *ui;
};

#endif // NEWDLG_H
