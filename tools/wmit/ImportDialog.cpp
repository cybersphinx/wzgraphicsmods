/*
	Copyright 2010 Warzone 2100 Project

	This file is part of WMIT.

	WMIT is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WMIT is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with WMIT.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ImportDialog.hpp"
#include "ui_ImportDialog.h"

#include <QFileDialog>

ImportDialog::ImportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ImportDialog)
{
	ui->setupUi(this);
}

QString ImportDialog::modelFilePath() const
{
	return ui->le_fileName->text();
}

QString ImportDialog::textureFilePath() const
{
	return ui->le_textureFName->text();
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

void ImportDialog::on_tb_seekFileName_clicked()
{
	fileDialog = new QFileDialog(this,
								 tr("Select File to open"),
								 QDir::currentPath(),
								 tr("All Compatible (*.wzm *.pie *.3ds *.obj);;"
									"WZM models (*.wzm);;"
									"PIE models (*.pie);;"
									"3DS files (*.3ds);;"
									"OBJ files (*.obj)"));
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->exec();
	if (fileDialog->result() == QDialog::Accepted)
	{
		ui->le_fileName->setText(fileDialog->selectedFiles().first());
	}
}

void ImportDialog::on_tb_seekTextureFName_clicked()
{
	fileDialog = new QFileDialog(this,
								 tr("Select File to open"),
								 QDir::currentPath(),
								 tr("WZ Compatible (*.png);;"
									"WMIT Compatible (*.bmp *.jpg *.jpeg *.png)"));
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->exec();
	if (fileDialog->result() == QDialog::Accepted)
	{
		ui->le_textureFName->setText(fileDialog->selectedFiles().first());
	}
}
