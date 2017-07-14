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
    billData.setVariety(ui->editVariety->text());
    billData.setPrice(ui->editPrice->text());
    billData.setCustomer(ui->editCustomer->text());
    billData.setComment(ui->editComment->toPlainText());
    QString detailCode;
    if (!ui->editCode1->text().isEmpty())
        detailCode.append(ui->editCode1->text());
    if (!ui->editCode2->text().isEmpty())
        detailCode.append(",").append(ui->editCode2->text());
    if (!ui->editCode3->text().isEmpty())
        detailCode.append(",").append(ui->editCode3->text());
    if (!ui->editCode4->text().isEmpty())
        detailCode.append(",").append(ui->editCode4->text());
    if (!ui->editCode5->text().isEmpty())
        detailCode.append(",").append(ui->editCode5->text());
    billData.setdetailCode(detailCode);
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
                 billData.comment());

    this->close();
}
