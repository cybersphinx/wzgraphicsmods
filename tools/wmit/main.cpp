#include <QtGui/QApplication>
//#include <QCoreApplication>
#include <QtGui/QMessageBox>
#include "MainWindow.hpp"

#ifdef __GNUC__
# ifdef WARNMORE
#  pragma  GCC diagnostic warning "-Weffc++"
#  pragma  GCC diagnostic warning "-Wconversion"
# endif
#endif

int main(int argc, char *argv[])
{
	//	if (command line mode)
	//		QCoreApplication a(argc, argv);
	//		TODO: Command line functionality
	//	else // gui mode1
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	QMessageBox msgBox;

	msgBox.setText("This program is in very early stages of development.");
	msgBox.setInformativeText("It might kill your computer, eat your babies, etc.\n See COPYING for license, terms and conditions.");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();

	return a.exec();
}
