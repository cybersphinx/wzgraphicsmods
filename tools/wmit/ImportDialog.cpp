#include "ImportDialog.hpp"
#include "ui_ImportDialog.h"

ImportDialog::ImportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ImportDialog)
{
	ui->setupUi(this);
}

void ImportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
		ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
