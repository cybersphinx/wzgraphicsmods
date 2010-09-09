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
#ifndef IMPORTDIALOG_HPP
#define IMPORTDIALOG_HPP

#include <QDialog>

namespace Ui {
	class ImportDialog;
}

class QFileDialog;

class ImportDialog : public QDialog {
    Q_OBJECT
public:
    ImportDialog(QWidget *parent = 0);
	QString modelFilePath() const;
	QString textureFilePath() const;

protected:
    void changeEvent(QEvent *e);

private:
	Ui::ImportDialog *ui;
	QFileDialog* fileDialog;

private slots:
	void on_tb_seekTextureFName_clicked();
	void on_tb_seekFileName_clicked();
};

#endif // IMPORTDIALOG_HPP
