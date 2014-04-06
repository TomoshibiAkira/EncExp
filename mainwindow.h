#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <EncryptionModel.h>
#include "encryptor.h"
#include "decryptor.h"
#include "progressbar.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    enum ActionMode
    {
        ENCRYPT,
        DECRYPT
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateleftPathBox(QModelIndex index);
    void updaterightPathBox(QModelIndex index);
    
private:
    Ui::MainWindow *ui;
    EncryptionModel *model;
    EncryptionModel *pathModel;
    QItemSelectionModel *leftSelection, *rightSelection;
    bool usesGzip;
    bool filenameEncryption;
    bool move;
    ActionMode mode;
    Encryptor* encryptor;
    Decryptor* decryptor;
    QStringList Filelist;
    QThread *encThread, *decThread;
    progressbar *bar;
    void startEncrypt(QString outPath);
    void startDecrypt(QString outPath);
    void collectFiles(QModelIndexList files, const QString& o);

private slots:
    void leftIndexChanged(QModelIndex index);
    void leftIndexChanged();
    void leftIndexChanged(QAction* action);
    void leftIndexChanged(int indexRow);
    void rightIndexChanged(QModelIndex index);
    void rightIndexChanged();
    void rightIndexChanged(QAction* action);
    void rightIndexChanged(int indexRow);

    void algorithmSelected(QAction* action);
    void modeSetting(QAction* action);
    void dirSetting();
    void LtoRFiles();
    void RtoLFiles();
    void pauseThread(bool stat);
    void cancelThread();
    void endProcessing();

    void updateFavButton();

    void aboutWindow();

};

#endif // MAINWINDOW_H
