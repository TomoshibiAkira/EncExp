#ifndef DIRECTORIES_H
#define DIRECTORIES_H

#include <QDialog>
#include <QDir>
#include <QString>

namespace Ui {
class Directories;
}

class Directories : public QDialog
{
    Q_OBJECT
    
public:
    explicit Directories(QWidget *parent = 0);
    ~Directories();
    static QList<QDir> getFavDir();
    static QDir getKeyDir();

signals:
    void keyPathDirChanged(const QString& path);
    void updateFavDir();

private:
    Ui::Directories *ui;
    QList<QDir> favDirList;
    QDir keyDir;
    void updateList();
    QString dirDialog();

private slots:
    void commited();
    void add();
    void del();
    void setKeyDir();
};

#endif // DIRECTORIES_H
