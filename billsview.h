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

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    bool viewportEvent(QEvent* e) Q_DECL_OVERRIDE;

private:
    bool m_isSearching;
    bool m_isMousePressed;

    void setupSignalsSlots();
    void setupStyleSheet();

public slots:
    void rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                            const QModelIndex &destinationParent, int destinationRow);

    void rowsMoved(const QModelIndex &parent, int start, int end,
                   const QModelIndex &destination, int row);
public Q_SLOTS:
    void print(QPrinter *printer);
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
