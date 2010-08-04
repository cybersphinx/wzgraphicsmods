#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

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
};

#endif // MAINWINDOW_HPP
