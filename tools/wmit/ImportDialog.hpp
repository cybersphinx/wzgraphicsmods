#ifndef IMPORTDIALOG_HPP
#define IMPORTDIALOG_HPP

#include <QDialog>

namespace Ui {
	class ImportDialog;
}

class ImportDialog : public QDialog {
    Q_OBJECT
public:
    ImportDialog(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private:
	Ui::ImportDialog *ui;
};

#endif // IMPORTDIALOG_HPP
