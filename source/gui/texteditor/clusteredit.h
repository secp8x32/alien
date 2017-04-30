#ifndef CLUSTEREDIT_H
#define CLUSTEREDIT_H

#include <QTextEdit>
#include <QVector3D>

#include "model/entities/cellto.h"

class Cell;
class ClusterEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit ClusterEdit(QWidget *parent = 0);

    void updateCluster (CellTO cell);
    void requestUpdate ();

signals:
    void clusterDataChanged (CellTO cell);

protected:
    void keyPressEvent (QKeyEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent (QMouseEvent* e);
    void wheelEvent (QWheelEvent* e);

private:

    void updateDisplay ();

    qreal generateNumberFromFormattedString (QString s);
    QString generateFormattedRealString (QString s);
    QString generateFormattedRealString (qreal r);

    CellTO _cell;
};

#endif // CLUSTEREDIT_H