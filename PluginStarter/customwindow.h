#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include <QtCore>
#include <QtGui>

class CustomWindow :
        public QWidget
{
    Q_OBJECT;
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle);
    Q_PROPERTY(bool stayOnTop READ isStayOnTop WRITE setStayOnTop);
public:
    explicit CustomWindow(QWidget *centralComponent,
                          const QString &moduleName,
                          bool resizableX, bool resizableY,QString pref);
public slots:
    void setWindowTitle(const QString &);
    void show();
    void close();
    void setVisible(bool visible);
    void showMinimized();
    void setPrefix(QString pref);
        ; //Set prefix for files;
    QString windowTitle();
    void setStayOnTop(bool v);
    bool isStayOnTop();
signals:
    void closed();
protected:
    void closeEvent(QCloseEvent *);
private:
    class CustomWindowPrivate *d;
    QString s_moduleName,prefix;
};


#endif // CUSTOMWINDOW_H
