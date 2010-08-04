#ifndef TEAMCOLOURSDOCK_HPP
#define TEAMCOLOURSDOCK_HPP

#include <QDockWidget>

namespace Ui {
    class TeamColoursDock;
}

class TeamColoursDock : public QDockWidget {
    Q_OBJECT
public:
    TeamColoursDock(QWidget *parent = 0);
    ~TeamColoursDock();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TeamColoursDock *ui;
};

#endif // TEAMCOLOURSDOCK_HPP
