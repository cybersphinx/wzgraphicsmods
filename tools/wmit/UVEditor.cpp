#include "UVEditor.hpp"
#include "ui_UVEditor.h"

UVEditor::UVEditor(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::UVEditor)
{
	ui->setupUi(this);
}

void UVEditor::changeEvent(QEvent *e)
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
