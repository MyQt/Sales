#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "billdata.h"
#include "billmodel.h"
#include "billsview.h"
#include "HHeaderModel.h"
#include "billwidgetdelegate.h"
#include "dbmanager.h"
#include "newdlg.h"
#include "tdpreviewdialog.h"

#include <QMainWindow>
#include <QToolButton>
#include <QSystemTrayIcon>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QQueue>
#include <QString>
#include <QSettings>
#include <QMenu>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setMainWindowVisibility(bool state);
private slots:
    void on_btnInsert_clicked();
protected:
    virtual void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;
    QToolButton* m_clearButton;
    QSystemTrayIcon* m_trayIcon;
    QAction* m_restoreAction;
    QAction* m_quitAction;
    QMenu* m_trayIconMenu;

    NewDlg* m_newDlg;
    QSettings* m_settingsDatabase;

    BillsView* m_billView;
    BillModel* m_billModel;
    QSortFilterProxyModel* m_proxyModel;
    QModelIndex m_currentSelectedBillProxy;
    QModelIndex m_selectedBillBeforeSearchingInSource;
    QModelIndex m_curClickIndex;
    DBManager* m_dbManager;
    int m_currentVerticalScrollAreaRange;
    int m_mousePressX;
    int m_mousePressY;
    int m_textEditLeftPadding;
    int m_billCounter;
    int m_trashCounter;
    int m_layoutMargin;
    bool m_canMoveWindow;
    bool m_canStretchWindow;
    bool m_isOperationRunning;

    TDPreviewDialog::Grids grid;
    QPrinter *m_printer;
private:
    void setupLine();
    void setupLineEdit();
    void setupFonts();
    void setupModelView();
    void setupDatabases();
    void setupTrayIcon();
    void setupSignalsSlots();
    void loadBills();
    void initializeSettingsDatabase();
    QDateTime getQDateTime (QString date);
    void clearSearch();
    void findBillsContain(const QString &keyword);
    void selectBill(const QModelIndex& billIndex);
    void selectFirstBill();
    void saveBillToDB(const QModelIndex& billIndex);
    void saveBill(BillData* billData);
    void removeBillFromDB(const QModelIndex& billIndex);
    void removeBillByCustomer(const QString customer);
    void deleteBill(const QModelIndex& billIndex, bool isFromUser=true);
    void InitCombox(); // 根据加载的所有清单项初始化combox
    void InsertComboxItem(QString customer); // 添加新的客户到第二非空白项
    void RemoveComboxItem(QString customer); // 删除指定客户项

    void print();

private slots:
    void onClearButtonClicked();
    void createNewBill(QString no,
                       QString variety,
                       QString detail_code,
                       QString price,
                       QString customer,
                       QString comment);
    void onLineEditTextChanged(const QString& keyword);
    void onBillPressed(const QModelIndex &index);
    void QuitApplication();
    void printTable(QPrinter *printer);

    void on_comboCustomer_currentIndexChanged(int index);
    void on_comboCustomer_currentIndexChanged(const QString &arg1);
    void on_btnPreview_clicked();
    void on_tableBills_clicked(const QModelIndex &index);
    void on_btnDelCustomer_clicked();
};

#endif // MAINWINDOW_H
