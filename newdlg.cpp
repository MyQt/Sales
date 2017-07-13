#include "newdlg.h"
#include "ui_newdlg.h"
#include "billdata.h"

NewDlg::NewDlg(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::NewDlg)
{
    ui->setupUi(this);
    init();
}

NewDlg::~NewDlg()
{
    delete ui;
}

void NewDlg::init()
{
    ui->editNo->setFocus();
}

void NewDlg::on_btnSure_clicked()
{
    // 传送给主窗口，添加表单
    BillData billData(0);
    billData.setNo(ui->editNo->text());
    billData.setPrice(ui->editPrice->text());
    billData.setCustomer(ui->editCustomer->text());
    billData.setComment(ui->editComment->toPlainText());
    QString detailCode;
    detailCode.append(ui->editCode1->text()).append(",");
    detailCode.append(ui->editCode2->text()).append(",");
    detailCode.append(ui->editCode3->text()).append(",");
    detailCode.append(ui->editCode4->text()).append(",");
    detailCode.append(ui->editCode5->text());
    billData.setdetailCode(detailCode);
    QDateTime billDate = QDateTime::currentDateTime();
    billData.setCreationDateTime(billDate);
    // 清空当前项
    ui->editNo->clear();
    ui->editVariety->clear();
    ui->editPrice->clear();
    ui->editCustomer->clear();
    ui->editCode1->clear();
    ui->editCode2->clear();
    ui->editCode3->clear();
    ui->editCode4->clear();
    ui->editCode5->clear();
    ui->editComment->clear();
    
    emit newItem(billData.no(),billData.variety(),
                 billData.detailCode(), billData.price(), billData.customer(),
                 billData.comment(), billData.creationDateTime());

    this->close();
}
