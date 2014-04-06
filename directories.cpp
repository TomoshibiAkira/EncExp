#include "directories.h"
#include "ui_directories.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

using namespace std;

Directories::Directories(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Directories)
{
    ui->setupUi(this);

    favDirList = getFavDir();
    updateList();

    keyDir = getKeyDir();
    ui->keydirLine->setText(QDir::toNativeSeparators(keyDir.path()));

    connect (ui->okButton, SIGNAL(clicked()), this, SLOT(commited()));
    connect (ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect (ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect (ui->deleteButton, SIGNAL(clicked()), this, SLOT(del()));
    connect (ui->expButton, SIGNAL(clicked()), this, SLOT(setKeyDir()));
}

QString Directories::dirDialog()
{
    return QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     QDir::rootPath(),
                                                     QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks));
}

void Directories::setKeyDir()
{
    QString path = dirDialog();
    if (!path.isEmpty())
    {
        keyDir.setPath(path);
        ui->keydirLine->setText(QDir::toNativeSeparators(keyDir.path()));
    }
}

void Directories::updateList()
{
    ui->listWidget->clear();
    foreach (const QDir& favDir, favDirList)
    {
        ui->listWidget->addItem(QDir::toNativeSeparators(favDir.absolutePath()));
    }
}

void Directories::add()
{
    QDir tmp(dirDialog());
    favDirList.removeAll(tmp);
    favDirList.append(tmp);
    updateList();
}

void Directories::del()
{
    foreach (const QListWidgetItem* item, ui->listWidget->selectedItems())
    {
        QString path = item->text();
        favDirList.removeOne(path);
    }
    updateList();
}

void Directories::commited()
{
    QSettings dir("Settings.ini", QSettings::IniFormat);
    dir.beginGroup("Favorite_Dir");
    dir.remove("");
    int i = 0;
    foreach (const QDir& favDir, favDirList)
    {
        i++;
        dir.setValue(QString("FAV%1").arg(i), favDir.path());
    }
    dir.endGroup();

    dir.beginGroup("Key_Dir");
    QDir tmp(QDir::fromNativeSeparators(ui->keydirLine->text()));
    if (tmp.absolutePath() == keyDir.absolutePath())
        dir.setValue("Master", QDir::fromNativeSeparators(tmp.path()));
    else
    {
        if (!tmp.exists())
            tmp.mkpath(tmp.absolutePath());
        dir.setValue("Master", QDir::fromNativeSeparators(tmp.path()));
    }
    dir.endGroup();
    close();

    emit keyPathDirChanged(QDir::fromNativeSeparators(tmp.path()));
    emit updateFavDir();
}

QList<QDir> Directories::getFavDir()
{
    QList<QDir> dirList;
    QSettings dir("Settings.ini", QSettings::IniFormat);
    dir.beginGroup("Favorite_Dir");
    foreach (const QString& favDir, dir.childKeys())
    {
        dirList.append(QDir(QDir::fromNativeSeparators(dir.value(favDir).toString())));
    }
    dir.endGroup();
    return dirList;
}

QDir Directories::getKeyDir()
{
    QDir key;
    QSettings dir("Settings.ini", QSettings::IniFormat);
    dir.beginGroup("Key_Dir");
    if (dir.value("Master").toString() == "")
        dir.setValue("Master", "key/");
    key.setPath(QDir::fromNativeSeparators(dir.value("Master").toString()));
    if (!key.exists())
        key.mkpath(key.absolutePath());
    dir.endGroup();
    return key;
}

Directories::~Directories()
{
    delete ui;
}
