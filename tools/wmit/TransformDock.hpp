#ifndef TRANSFORMDOCK_HPP
#define TRANSFORMDOCK_HPP

#include <QDockWidget>

namespace Ui {
    class TransformDock;
}

class TransformDock : public QDockWidget {
    Q_OBJECT
public:
    TransformDock(QWidget *parent = 0);
    ~TransformDock();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TransformDock *ui;

private slots:

};

#endif // TRANSFORMDOCK_HPP
