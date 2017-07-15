#include "newdlg.h"
#include "ui_newdlg.h"
#include "billdata.h"

NewDlg::NewDlg(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::NewDlg)
{
    ui->setupUi(this);
}

NewDlg::~NewDlg()
{
    delete ui;
}

void NewDlg::init(QList<BillData *>& billlist, QString& customer)
{
    m_billList.clear();
    m_customer.clear();
    ui->comboNo->clear();
    if (!customer.isEmpty()) // 有客户时，客户固定,编号选择后,品种/单价固定，否则可自由输入
    {
        m_billList = billlist;
        m_customer = customer;
        ui->editCustomer->setText(customer);
        ui->editCustomer->setFocusPolicy(Qt::NoFocus);
        InitComboxItem();
        ui->comboNo->show();
    } else
    {
        ui->editCustomer->setFocusPolicy(Qt::StrongFocus);
        ui->comboNo->hide();
    }
}

void NewDlg::InitComboxItem()
{
    if (m_billList.isEmpty())
    {
        ui->comboNo->hide();
        return;
    }
    ui->comboNo->addItem("");
    for (int i = 0; i < m_billList.size(); i++)
    {
        BillData* pData = m_billList.at(i);
        if (pData != Q_NULLPTR)
        {
            int index = ui->comboNo->findText(pData->no());
            if (index == -1)
            {
                ui->comboNo->addItem(pData->no());
            }
        }
    }
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

void NewDlg::on_comboNo_currentIndexChanged(const QString &arg1)
{
    if (arg1.isEmpty())
    {
        ui->editVariety->setFocusPolicy(Qt::StrongFocus);
        ui->editPrice->setFocusPolicy(Qt::StrongFocus);
        ui->editNo->setFocusPolicy(Qt::StrongFocus);
        return;
    }
    ui->editNo->setText(arg1);
    ui->editNo->setFocusPolicy(Qt::NoFocus);
    // 根据选定的编号，品种/单价固定
    for (int i = 0; i < m_billList.size(); i++)
    {
        BillData* pData = m_billList.at(i);
        if (pData != Q_NULLPTR)
        {
            if (arg1 == pData->no())
            {
                ui->editVariety->setText(pData->variety());
                ui->editVariety->setFocusPolicy(Qt::NoFocus);
                ui->editPrice->setText(pData->price());
                ui->editPrice->setFocusPolicy(Qt::NoFocus);
                break;
            }
        }
    }
}


void NewDlg::closeEvent(QCloseEvent *event)
{
    m_billList.clear();
    m_customer.clear();
    ui->comboNo->clear();
}
