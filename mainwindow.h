#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "billdata.h"
#include "billmodel.h"
#include "billsview.h"
#include "billwidgetdelegate.h"
#include "dbmanager.h"
#include "newdlg.h"

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
    BillModel* m_deletedBillsModel;
    QSortFilterProxyModel* m_proxyModel;
    QModelIndex m_currentSelectedBillProxy;
    QModelIndex m_selectedBillBeforeSearchingInSource;
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
    void removeBillFromDB(const QModelIndex& billIndex);
    void removeBillByCustomer(const QString customer);
    void deleteBill(const QModelIndex& billIndex, bool isFromUser=true);
    void InitCombox(); // 根据加载的所有清单项初始化combox
    void InsertComboxItem(QString customer); // 添加新的客户到第一项
    void RemoveComboxItem(QString customer); // 删除指定客户项
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

    void on_comboCustomer_currentIndexChanged(int index);
    void on_comboCustomer_currentIndexChanged(const QString &arg1);
};

#endif // MAINWINDOW_H
