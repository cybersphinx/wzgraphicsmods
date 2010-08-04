#include "TeamColoursDock.hpp"
#include "ui_TeamColoursDock.h"

TeamColoursDock::TeamColoursDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TeamColoursDock)
{
    ui->setupUi(this);
}

TeamColoursDock::~TeamColoursDock()
{
    delete ui;
}

void TeamColoursDock::changeEvent(QEvent *e)
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
