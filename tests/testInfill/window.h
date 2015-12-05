#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "slcmodel.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QSpinBox;
class QPushButton;
class QLineEdit;
QT_END_NAMESPACE
class draw;

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window();

private slots:
    void LayerChanged();
    void openTrigger();
    void saveTrigger();
    void clearTrigger();
    void infillTrigger();
private:
    XJRP::SLCModel *model;

    draw *drawArea;

    QLabel *LayerNumLabel;
    QSpinBox *LayerNum;

    QLabel *interval_numLabel;
    QLineEdit *interval_numEdit;

    QLabel *spaceLabel;
    QLineEdit *spaceEdit;

    QLabel *shrinkDistanceLabel;
    QLineEdit *shrinkDistanceEdit;

    QLabel *angle_startLabel;
    QLineEdit *angle_startEdit;

    QLabel *angle_deltaLabel;
    QLineEdit *angle_deltaEdit;

    QLabel *LaserPowerLabel;
    QLineEdit *LaserPowerEdit;

    QLabel *ScanSpeedLabel;
    QLineEdit *ScanSpeedEdit;

    QPushButton *open;
    QPushButton *save;
    QPushButton *clear;
    QPushButton *infill;
    QPushButton *centering;
};
//! [0]

#endif // WINDOW_H

