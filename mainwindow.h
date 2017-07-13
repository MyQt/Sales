#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "billdata.h"
#include "billmodel.h"
#include "billview.h"
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

    BillView* m_billView;
    BillModel* m_billModel;
    BillModel* m_deletedBillsModel;
    QSortFilterProxyModel* m_proxyModel;
    QModelIndex m_currentSelectedBillProxy;
    QModelIndex m_selectedBillBeforeSearchingInSource;
    QQueue<QString> m_searchQueue;
    DBManager* m_dbManager;

    int m_currentVerticalScrollAreaRange;
    int m_mousePressX;
    int m_mousePressY;
    int m_textEditLeftPadding;
    int m_billCounter;
    int m_trashCounter;
    int m_layoutMargin;
    int m_billListWidth;
    bool m_canMoveWindow;
    bool m_canStretchWindow;
    bool m_isTemp;
    bool m_isListViewScrollBarHidden;
    bool m_isContentModified;
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
    void moveBillToTop();
    void selectBill(const QModelIndex& billIndex);
    void selectFirstBill();
    void saveBillToDB(const QModelIndex& billIndex);
    void removeBillFromDB(const QModelIndex& billIndex);
    void deleteBill(const QModelIndex& billIndex, bool isFromUser=true);
private slots:
    void onClearButtonClicked();
    void createNewBill(QString no,
                       QString variety,
                       QString detail_code,
                       QString price,
                       QString customer,
                       QString comment,
                       QDateTime creationDateTime);
    void onLineEditTextChanged(const QString& keyword);
    void onBillPressed(const QModelIndex &index);
    void QuitApplication();

};

#endif // MAINWINDOW_H
