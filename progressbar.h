#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

namespace Ui {
class progressbar;
}

class progressbar : public QWidget
{
    Q_OBJECT

signals:
    void pauseThread(bool stat);
    void cancelThread();
    
public:
    explicit progressbar(QWidget *parent = 0);
    ~progressbar();

public slots:
    void updateCurrent(QString stat);
    void updateAll(QString stat);
    void updateFile (QString file);
    void updateStatus (const QString& stat);

private slots:
    void pause(bool stat);
    void cancel();
    
private:
    Ui::progressbar *ui;

};

#endif // PROGRESSBAR_H
