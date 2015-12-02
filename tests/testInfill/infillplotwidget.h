#ifndef INFILLPLOTWIDGET_H
#define INFILLPLOTWIDGET_H

#include <QWidget>
#include "layer.h"

class InfillPlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InfillPlotWidget(QWidget *parent = 0);

    void setLayer (const XJRP::Layer &layer);

protected:
    void paintEvent (QPaintEvent *e);

private:
    XJRP::Layer layer;
};

#endif // INFILLPLOTWIDGET_H
