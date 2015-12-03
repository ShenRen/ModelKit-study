#include "draw.h"
#include "window.h"

#include <QtWidgets>

Window::Window(XJRP::SLCModel * m)
{
    model= m;

    drawArea = new draw;

    LayerNum = new QSpinBox;
    LayerNum->setRange(1, model->size());
    LayerNum->setSpecialValueText(tr("1 (Layer Number)"));
    connect(LayerNum, SIGNAL(valueChanged(int)),
            this, SLOT(LayerChanged()));

    LayerNumLabel = new QLabel(tr("Layer &Number:"));
    LayerNumLabel->setBuddy(LayerNum);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(drawArea, 0, 0, 1, 4);
    mainLayout->addWidget(LayerNumLabel, 2, 0, Qt::AlignRight);
    mainLayout->addWidget(LayerNum, 2, 1);
    setLayout(mainLayout);
    LayerChanged();
    setWindowTitle(tr("SLC Read And Infill"));

}

void Window::LayerChanged()
{
    drawArea->setLayer(model->layerAtIndex(LayerNum->value()-1));
}
