#include "draw.h"
#include "window.h"

#include <QtWidgets>
#include <QFileDialog>

Window::Window()
{
    /*
     * void skin_core_infill(int interval_num,
     *                       float space,
     *                       float shrinkDistance,
     *                       float angle_start,
     *                       float angle_delta,
     *                       qreal LaserPower=-1.0,
     *                       qreal ScanSpeed=-1.0);
     */
    drawArea = new draw;
    model = new XJRP::SLCModel;
    //用来显示层的SpinBox
    LayerNum = new QSpinBox;
    LayerNum->setRange(0,0);
    LayerNum->setSpecialValueText(tr("0 (No Model)"));
    connect(LayerNum, SIGNAL(valueChanged(int)),
            this, SLOT(LayerChanged()));
    LayerNumLabel = new QLabel(tr("Layer &Number:"));
    LayerNumLabel->setBuddy(LayerNum);
    //用来设置参数的LineEdit
    interval_numEdit = new QLineEdit(tr("2"));
    interval_numLabel = new QLabel(tr("Interval &Number:"));
    interval_numLabel->setBuddy(interval_numEdit);
    spaceEdit = new QLineEdit(tr("0.1"));
    spaceLabel = new QLabel(tr("Space:"));
    spaceLabel->setBuddy(spaceEdit);
    shrinkDistanceEdit = new QLineEdit(tr("0.1"));
    shrinkDistanceLabel = new QLabel(tr("Shrink &Distance:"));
    shrinkDistanceLabel->setBuddy(shrinkDistanceEdit);
    angle_startEdit = new QLineEdit(tr("0"));
    angle_startLabel = new QLabel(tr("Angle &Start:"));
    angle_startLabel->setBuddy(angle_startEdit);
    angle_deltaEdit = new QLineEdit(tr("90"));
    angle_deltaLabel = new QLabel(tr("angle &Delta:"));
    angle_deltaLabel->setBuddy(angle_deltaEdit);
    LaserPowerEdit = new QLineEdit(tr("4000"));
    LaserPowerLabel = new QLabel(tr("Laser &Power:"));
    LaserPowerLabel->setBuddy(LaserPowerEdit);
    ScanSpeedEdit = new QLineEdit(tr("4000"));
    ScanSpeedLabel = new QLabel(tr("Scan &Speed:"));
    ScanSpeedLabel->setBuddy(ScanSpeedEdit);
    //按钮
    open = new QPushButton(tr("&open"));
    save = new QPushButton(tr("&save"));
    clear = new QPushButton(tr("&clear"));
    infill = new QPushButton(tr("&infill"));
    connect(open,SIGNAL(clicked()),
            this,SLOT(openTrigger()));
    connect(save,SIGNAL(clicked()),
            this,SLOT(saveTrigger()));
    connect(clear,SIGNAL(clicked()),
            this,SLOT(clearTrigger()));
    connect(infill,SIGNAL(clicked()),
            this,SLOT(infillTrigger()));
    //布局
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(drawArea, 0, 0, 1, 14);
    mainLayout->addWidget(open,2, 0);
    mainLayout->addWidget(clear,2,1);
    mainLayout->addWidget(LayerNumLabel, 2, 2);
    mainLayout->addWidget(LayerNum, 2, 3);
    mainLayout->addWidget(infill, 2, 4);
    mainLayout->addWidget(save, 2, 5);
    mainLayout->addWidget(interval_numLabel,3,0);
    mainLayout->addWidget(interval_numEdit,3,1);
    mainLayout->addWidget(spaceLabel,3,2);
    mainLayout->addWidget(spaceEdit,3,3);
    mainLayout->addWidget(shrinkDistanceLabel,3,4);
    mainLayout->addWidget(shrinkDistanceEdit,3,5);
    mainLayout->addWidget(angle_startLabel,3,6);
    mainLayout->addWidget(angle_startEdit,3,7);
    mainLayout->addWidget(angle_deltaLabel,3,8);
    mainLayout->addWidget(angle_deltaEdit,3,9);
    mainLayout->addWidget(LaserPowerLabel,3,10);
    mainLayout->addWidget(LaserPowerEdit,3,11);
    mainLayout->addWidget(ScanSpeedLabel,3,12);
    mainLayout->addWidget(ScanSpeedEdit,3,13);
    setLayout(mainLayout);
    //LayerChanged();
    setWindowTitle(tr("SLC Read And Infill"));
    infill->hide();
    save->hide();
}

void Window::LayerChanged()
{
    drawArea->setLayer(model->layerAtIndex(LayerNum->value()-1));
}

void Window::openTrigger()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open a slc file"),tr("/select .slc file please"),tr("slicing file(*.slc)"));
    if(fileName.length()==0)
        return;
    if(model->empty())
        this->model->readFile(fileName);
    else
    {
        XJRP::SLCModel tem;
        tem.readFile(fileName);
        this->model->merge(tem);
    }
    drawArea->setModel(*(this->model));
    LayerNum->setRange(1,this->model->size());
    LayerNum->setSpecialValueText(tr("1 (Layer Number)"));
    infill->show();
}
void Window::saveTrigger()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save a XJ file"),tr("/select .XJ file please"),tr("slicing file(*.XJ)"));
    if(fileName.length()==0)
        return;
//    QFile file(fileName);
//    for(const XJRP::Layer & L : *(this->model))
//    {
//        file<<L;
//    }
}
void Window::clearTrigger()
{
    this->model->clear();
    LayerNum->setRange(0,0);
    LayerNum->setSpecialValueText(tr("0 (No Model)"));
    infill->hide();
    save->hide();
}
void Window::infillTrigger()
{
    infill->show();
    this->model->skin_core_infill(interval_numEdit->text().toInt(),
                                  spaceEdit->text().toFloat(),
                                  shrinkDistanceEdit->text().toFloat(),
                                  angle_startEdit->text().toFloat(),
                                  angle_deltaEdit->text().toFloat(),
                                  LaserPowerEdit->text().toDouble(),
                                  ScanSpeedEdit->text().toDouble());
    save->show();
}
