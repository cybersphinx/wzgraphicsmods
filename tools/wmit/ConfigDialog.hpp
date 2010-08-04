#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QDialog>

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
public:
    ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_HPP
