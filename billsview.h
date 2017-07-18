#ifndef BillsView_H
#define BillsView_H

#include <QScrollArea>
#include <QTableView>
#include <QPrinter>
class BillsView : public QTableView
{
    Q_OBJECT

    friend class tst_BillsView;

public:
    explicit BillsView(QWidget* parent = Q_NULLPTR);
    ~BillsView();

    void initHeaderView();
//    void animateAddedRow(const QModelIndex &parent, int start, int end);
    void setSearching(bool isSearching);
    void setCurrentRowActive(bool isActive);
    void CalcultPrintAreaRow(int h, int &pageCount, QVector<int> &printAreaStartRow, QVector<int> &printAreaHeight);
    void CalcultPrintAreaCol(int w, int &pageCount, QVector<int> &printAreaStartCol, QVector<int> &printAreaWidth);
    int CalcultTableViewWidth();
    int CalcultTableViewHeight();
    void setInputInfo(const QString& customer, const QString& maker, const QString& driver)
    {
        m_customer = customer;
        m_maker = maker;
        m_driver = driver;
    }
protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    bool viewportEvent(QEvent* e) Q_DECL_OVERRIDE;

private:
    bool m_isSearching;
    bool m_isMousePressed;
    QString m_customer;
    QString m_maker;
    QString m_driver;
    void setupSignalsSlots();
    void setupStyleSheet();

public slots:
    void rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                            const QModelIndex &destinationParent, int destinationRow);

    void rowsMoved(const QModelIndex &parent, int start, int end,
                   const QModelIndex &destination, int row);
public slots:
    void prints(QPrinter *printer);
private slots:
    void init();
    void onCurrentRowChanged(const QModelIndex & current, const QModelIndex & previous);
    void onEntered(QModelIndex index);
    void onViewportEntered();
    void onRangeChanged(int min, int max);
protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;

signals:
    void viewportPressed();

};

#endif // BillsView_H
