#include "TransformDock.hpp"
#include "ui_TransformDock.h"

TransformDock::TransformDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TransformDock)
{
    ui->setupUi(this);
}

TransformDock::~TransformDock()
{
    delete ui;
}

void TransformDock::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

