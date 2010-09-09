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
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include "WZM.hpp"

class TransformDock;
class ConfigDialog;
class ImportDialog;
class UVEditor;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private slots:
	void s_fileOpen();
	void on_actionUVEditor_toggled(bool );
	void on_actionOpen_triggered();
	void on_actionConfig_triggered();
	void on_actionTransformWidget_toggled(bool );

private:
	Ui::MainWindow* ui;
	ConfigDialog* configDialog;
	ImportDialog* importDialog;
	TransformDock* transformDock;
	UVEditor* m_UVEditor;

	WZM model;
};

#endif // MAINWINDOW_HPP
