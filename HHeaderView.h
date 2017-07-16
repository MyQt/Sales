#ifndef HHEADERVIEW_H
#define HHEADERVIEW_H

#include <QHeaderView>

class  HHeaderModel;

class HHeaderItemDelegate;
class HHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    HHeaderView(Qt::Orientation orientation, QWidget * parent = 0);
    ~HHeaderView();

    void initHeaderView(HHeaderModel *pModel);
    void setSectionSize(HHeaderModel *pModel, int width);
    void setQuit();

    int sectionSizes(int lIndex, int sectionCount);
    
    //! 算出当前鼠标位置所在的index
    QModelIndex indexAt(const QPoint &pos) const;

    //! 判断该单元格是否按住
    bool isItemPress(int row, int col);

public slots:
    void onSectionResized(int logicalIndex, int oldSize, int newSize);

protected:
    virtual QSize sectionSizeFromContents(int logicalIndex) const;
    virtual void paintEvent(QPaintEvent *event);
    //virtual int sectionSizeHint(int logicalIndex);

//    virtual void mouseMoveEvent(QMouseEvent *event);
//    virtual bool event ( QEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
//    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    void sortUp(int row, int col);
    void sortDown(int row, int col);
private:
    QModelIndex m_hoverIndex;
    //! 保存按下的列
    QModelIndex m_pressIndex;
    //! 保存所有item的矩形 key:row  key1:col
    QMap<int, QMap<int, QRect> > m_itemRectMap;
private:

    bool m_bQuit; //! 不允许继续绘制item
    HHeaderItemDelegate* m_pItemDelegate;
};

#endif // HHEADERVIEW_H
