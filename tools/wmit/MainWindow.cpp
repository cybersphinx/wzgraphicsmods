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

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "ConfigDialog.hpp"
#include "TransformDock.hpp"
#include "ImportDialog.hpp"
#include "UVEditor.hpp"

#include <fstream>
#include <QFileInfo>

#include "Pie.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	configDialog(new ConfigDialog),
	importDialog(new ImportDialog),
	transformDock(new TransformDock),
	m_UVEditor(new UVEditor)
{
	ui->setupUi(this);

	// A work around to add actions in the order we want
	// TODO: Need to check what this does on MAC
	ui->menuBar->clear();
	ui->menuBar->addMenu(ui->menuFile);
	ui->menuBar->addAction(ui->actionConfig);
	ui->menuBar->addMenu(ui->menuTextures);
	ui->menuBar->addMenu(ui->menuTeam_Colours);
	ui->menuBar->addAction(ui->actionTransformWidget);
	ui->menuBar->addAction(ui->actionUVEditor);

	configDialog->hide();

	transformDock->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::TopDockWidgetArea);
	transformDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, transformDock, Qt::Horizontal);

	m_UVEditor->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
	m_UVEditor->hide();
	this->addDockWidget(Qt::RightDockWidgetArea, m_UVEditor, Qt::Horizontal);

	connect(importDialog, SIGNAL(accepted()), this, SLOT(s_fileOpen()));
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::s_fileOpen()
{
	// TODO: cleanup the following possibly
	QFileInfo modelFileNfo(importDialog->modelFilePath());
	std::ifstream f;
	if (modelFileNfo.completeSuffix().compare(QString("wzm"), Qt::CaseInsensitive) == 0)
	{
		f.open(modelFileNfo.absoluteFilePath().toLocal8Bit(), std::ios::in);
		model.read(f);
	}
	else if(modelFileNfo.completeSuffix().compare(QString("pie"), Qt::CaseInsensitive) == 0)
	{
		f.open(modelFileNfo.absoluteFilePath().toLocal8Bit(), std::ios::in);
		int version = pieVersion(f);
		if (version == 3)
		{
			Pie3Model p3;
			p3.read(f);
			model = WZM(p3);
		}
		else if (version == 2)
		{
			Pie2Model p2;
			p2.read(f);
			model = WZM(Pie3Model(p2));
		}
	}
	else if(modelFileNfo.completeSuffix().compare(QString("3ds"), Qt::CaseInsensitive) == 0)
	{
		model.importFrom3DS(std::string(modelFileNfo.absoluteFilePath().toLocal8Bit().constData()));
	}
	else if(modelFileNfo.completeSuffix().compare(QString("obj"), Qt::CaseInsensitive) == 0)
	{
		f.open(modelFileNfo.absoluteFilePath().toLocal8Bit(), std::ios::in);
		model.importFromOBJ(f);
	}
	ui->centralWidget->clearRenderList();
	ui->centralWidget->addToRenderList(&model);

	// HACK FIXME: Temporary nastiness
	model.setTextureManager(static_cast<IGLTextureManager*>(ui->centralWidget));

	model.setRenderTexture(importDialog->textureFilePath().toStdString());
}

void MainWindow::on_actionConfig_triggered()
{
	configDialog->show();
}

void MainWindow::on_actionTransformWidget_toggled(bool show)
{
	show ? transformDock->show() : transformDock->hide();
}

void MainWindow::on_actionOpen_triggered()
{
	importDialog->exec();
}

void MainWindow::on_actionUVEditor_toggled(bool show)
{
	show? m_UVEditor->show() : m_UVEditor->hide();
}
