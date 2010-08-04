#ifndef UVEDITOR_HPP
#define UVEDITOR_HPP

#include <QDockWidget>

namespace Ui {
	class UVEditor;
}

class UVEditor : public QDockWidget {
    Q_OBJECT
public:
    UVEditor(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private:
	Ui::UVEditor* ui;
};

#endif // UVEDITOR_HPP

