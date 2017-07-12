﻿#ifndef BillView_H
#define BillView_H

#include <QListView>
#include <QScrollArea>

class BillView : public QListView
{
    Q_OBJECT

    friend class tst_BillView;

public:
    explicit BillView(QWidget* parent = Q_NULLPTR);
    ~BillView();

    void animateAddedRow(const QModelIndex &parent, int start, int end);
    void setSearching(bool isSearching);
    void setCurrentRowActive(bool isActive);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    bool viewportEvent(QEvent* e) Q_DECL_OVERRIDE;

private:
    bool m_isScrollBarHidden;
    bool m_isSearching;
    bool m_isMousePressed;
    int m_rowHeight;

    void setupSignalsSlots();
    void setupStyleSheet();

public slots:
    void rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                            const QModelIndex &destinationParent, int destinationRow);

    void rowsMoved(const QModelIndex &parent, int start, int end,
                   const QModelIndex &destination, int row);

private slots:
    void init();

protected slots:
    void rowsInserted(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) Q_DECL_OVERRIDE;

signals:
    void viewportPressed();

};

#endif // BillView_H
