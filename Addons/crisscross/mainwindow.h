#ifndef CrissCrossWindow_H
#define CrissCrossWindow_H

#include <QMainWindow>

namespace Ui {
    class CrissCrossWindow;
}

struct TPoint {
    int x;
    int y;
};


class CrissCrossWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CrissCrossWindow(QWidget *parent = 0);
    ~CrissCrossWindow();
    void setSize(int i);
    void setGamer(bool b);
    void setIgnore(bool b);

    bool checkIgnore();
    void defend(int x, int y);
    int whoWon();
    int lastX();
    int lastY();
    TPoint helpHim();
public slots:
    void showAbout();
    void showHelp();

    void setHard();
    void setSimple();
private:
    void keyPressEvent(QKeyEvent *event);
    Ui::CrissCrossWindow *ui;
};

#endif // CrissCrossWindow_H
