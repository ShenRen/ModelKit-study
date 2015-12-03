#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "slcmodel.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;
QT_END_NAMESPACE
class draw;

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(XJRP::SLCModel *m);

private slots:
    void LayerChanged();

private:
    XJRP::SLCModel *model;
    draw *drawArea;
    QLabel *LayerNumLabel;
    QSpinBox *LayerNum;
};
//! [0]

#endif // WINDOW_H

