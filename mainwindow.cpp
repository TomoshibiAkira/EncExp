#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "directories.h"
#include "about.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    encryptor = new Encryptor(Encryptor::AES_256, Directories::getKeyDir().absolutePath());
    decryptor = new Decryptor(Directories::getKeyDir().absolutePath());

    encThread = new QThread(this);
    decThread = new QThread(this);
    connect (encThread, SIGNAL(started()), encryptor, SLOT(encrypt()));
    connect (decThread, SIGNAL(started()), decryptor, SLOT(decrypt()));
    connect (encryptor, SIGNAL(finished()), this, SLOT(endProcessing()));
    connect (decryptor, SIGNAL(finished()), this, SLOT(endProcessing()));
    encryptor->moveToThread(encThread);
    decryptor->moveToThread(decThread);

    QActionGroup* encryptionMethod = new QActionGroup(this);
    QAction* methodAES256 = new QAction("AES-&256", this);
    methodAES256->setData(0);
    methodAES256->setCheckable(true);
    methodAES256->setChecked(true);
    QAction* methodRSA = new QAction("&RSA", this);
    methodRSA->setData(1);
    methodRSA->setCheckable(true);
    encryptionMethod->addAction(methodAES256);
    encryptionMethod->addAction(methodRSA);

    QActionGroup* encdec = new QActionGroup(this);
    QAction* encAction = new QAction("&Encrypt", this);
    encAction->setData(0);
    encAction->setCheckable(true);
    encAction->setChecked(true);
    QAction* decAction = new QAction("&Decrypt", this);
    decAction->setData(1);
    decAction->setCheckable(true);
    encdec->addAction(encAction);
    encdec->addAction(decAction);
    QAction* moveAction = new QAction("&Move", this);
    moveAction->setCheckable(true);

    QToolButton* methodButton = new QToolButton;
    methodButton->setText("Encryption Method");
    methodButton->setPopupMode(QToolButton::InstantPopup);
    methodButton->addActions(encryptionMethod->actions());

    QToolButton* settingButton = new QToolButton;
    settingButton->setText("Encryption Settings");
    settingButton->setPopupMode(QToolButton::InstantPopup);
    settingButton->addActions(ui->menuEncryption_Settings->actions());

    ui->leftFavButton->setPopupMode(QToolButton::InstantPopup);
    ui->rightFavButton->setPopupMode(QToolButton::InstantPopup);
    updateFavButton();

    ui->menuEncrypt_Algorithm->addActions(encryptionMethod->actions());
    ui->menu_Action_Mode->addActions(encdec->actions());
    ui->menu_Action_Mode->addSeparator();
    ui->menu_Action_Mode->addAction(moveAction);

    ui->toolBar->addActions(encdec->actions());
    ui->toolBar->addAction(moveAction);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(methodButton);
    ui->toolBar->addWidget(settingButton);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->action_Directories);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->action_Exit);

    connect (ui->action_Exit,SIGNAL(triggered()), this, SLOT(close()));
    connect (ui->actionAbout_This, SIGNAL(triggered()), this, SLOT(aboutWindow()));
    connect (ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect (ui->action_Directories, SIGNAL(triggered()), this, SLOT(dirSetting()));
    connect (moveAction, SIGNAL(toggled(bool)), encryptor, SLOT(setMove(bool)));
    connect (ui->action_Uses_Gzip, SIGNAL(toggled(bool)), encryptor, SLOT(setGzip(bool)));
    connect (ui->action_Filename_Encryption, SIGNAL(toggled(bool)), encryptor, SLOT(setFilenameEncryption(bool)));
    connect (encryptionMethod, SIGNAL(triggered(QAction*)), this, SLOT(algorithmSelected(QAction*)));
    connect (encdec, SIGNAL(triggered(QAction*)), this, SLOT(modeSetting(QAction*)));

    model = new EncryptionModel;
    model->setRootPath("");
    model->setFilter(QDir::AllEntries | QDir::NoDot | QDir::AllDirs);

    pathModel = new EncryptionModel;
    pathModel->setRootPath("");
    pathModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::AllDirs);

    leftSelection = new QItemSelectionModel(model);
    rightSelection = new QItemSelectionModel(model);

    ui->leftView->setModel(model);
    ui->leftView->setRootIndex(model->index(QDir::rootPath()));
    ui->leftView->setSelectionModel(leftSelection);
    ui->rightView->setModel(model);
    ui->rightView->setRootIndex(model->index(QDir::rootPath()));
    ui->rightView->setSelectionModel(rightSelection);
    ui->leftPathBox->setModel(pathModel);
    ui->rightPathBox->setModel(pathModel);

    ui->leftPathBox->view()->setRootIndex(pathModel->index(QDir::rootPath()).parent());
    ui->leftPathBox->setRootModelIndex(pathModel->index(QDir::rootPath()).parent());
    ui->leftPathBox->setCurrentIndex(0);
    ui->leftPath->setText(QDir::toNativeSeparators(QDir::rootPath()));
    ui->rightPathBox->view()->setRootIndex(pathModel->index(QDir::rootPath()).parent());
    ui->rightPathBox->setRootModelIndex(pathModel->index(QDir::rootPath()).parent());
    ui->rightPathBox->setCurrentIndex(0);
    ui->rightPath->setText(QDir::toNativeSeparators(QDir::rootPath()));

    connect (ui->leftView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(leftIndexChanged(QModelIndex)));
    connect (ui->leftBackButton,SIGNAL(clicked()), this, SLOT(leftIndexChanged()));
    connect (ui->rightView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rightIndexChanged(QModelIndex)));
    connect (ui->rightBackButton, SIGNAL(clicked()), this, SLOT(rightIndexChanged()));
    connect (ui->leftFavButton, SIGNAL(triggered(QAction*)), this, SLOT(leftIndexChanged(QAction*)));
    connect (ui->rightFavButton, SIGNAL(triggered(QAction*)), this, SLOT(rightIndexChanged(QAction*)));
    connect (ui->leftPathBox, SIGNAL(activated(int)), this, SLOT(leftIndexChanged(int)));
    connect (ui->rightPathBox, SIGNAL(activated(int)), this, SLOT(rightIndexChanged(int)));

    connect (ui->LtoRButton, SIGNAL(clicked()), this, SLOT(LtoRFiles()));
    connect (ui->RtoLButton, SIGNAL(clicked()), this, SLOT(RtoLFiles()));

    mode = ENCRYPT;
}

/* TODO:
 * Collect filenames and processing
 */

void MainWindow::collectFiles(QModelIndexList files, const QString& o)
{
    Filelist.clear();
    foreach (const QModelIndex& file, files)
    {
        if (!model->isDir(file) && file.sibling(file.row(), 4).data().toString() == o)
            Filelist.append(QDir::fromNativeSeparators(model->filePath(file)));
    }
    Filelist.removeDuplicates();
}

void MainWindow::LtoRFiles()
{
    if (mode == ENCRYPT)
    {
        collectFiles( leftSelection->selectedRows(), "No");
        if (!Filelist.isEmpty())
            startEncrypt(QDir::fromNativeSeparators(model->filePath(ui->rightView->rootIndex())));
    }
    else
    {
        collectFiles( leftSelection->selectedRows(), "Yes");
        if (!Filelist.isEmpty())
            startEncrypt(QDir::fromNativeSeparators(model->filePath(ui->leftView->rootIndex())));
    }
}

void MainWindow::RtoLFiles()
{

}

void MainWindow::startEncrypt(QString outPath)
{
    encThread->quit();
    encryptor->setOutputPath(outPath);
    encryptor->addFiles(Filelist);

    bar = new progressbar();
    bar->show();
    connect (encryptor, SIGNAL(updateAllProgress(QString)), bar, SLOT(updateAll(QString)));
    connect (encryptor, SIGNAL(updateCurrentFile(QString)), bar, SLOT(updateFile(QString)));
    connect (encryptor, SIGNAL(updateCurrentProgress(QString)), bar, SLOT(updateCurrent(QString)));
    connect (encryptor, SIGNAL(updateStatus(QString)), bar, SLOT(updateStatus(QString)));

    connect (bar, SIGNAL(pauseThread(bool)), this, SLOT(pauseThread(bool)));
    connect (bar, SIGNAL(cancelThread()), this, SLOT(cancelThread()));

    encThread->start();
}

void MainWindow::startDecrypt(QString outPath)
{
    decThread->quit();
    decryptor->setOutputPath(outPath);
    decryptor->addFiles(Filelist);

    bar = new progressbar();
    bar->show();
    connect (decryptor, SIGNAL(updateAllProgress(QString)), bar, SLOT(updateAll(QString)));
    connect (decryptor, SIGNAL(updateCurrentFile(QString)), bar, SLOT(updateFile(QString)));
    connect (decryptor, SIGNAL(updateCurrentProgress(QString)), bar, SLOT(updateCurrent(QString)));
    connect (decryptor, SIGNAL(updateStatus(QString)), bar, SLOT(updateStatus(QString)));

    connect (bar, SIGNAL(pauseThread(bool)), this, SLOT(pauseThread(bool)));
    connect (bar, SIGNAL(cancelThread()), this, SLOT(cancelThread()));

    decThread->start();
}

void MainWindow::endProcessing()
{
    disconnect (bar, 0, 0, 0);
    bar->close();
    delete bar;
    encryptor->clearFiles();
    decryptor->clearFiles();
}

void MainWindow::pauseThread(bool stat)
{
    if  (stat)
    {
        if (encThread->isRunning()) encThread->wait();
        if (decThread->isRunning()) decThread->wait();
    }
    else
    {
        encThread->start();
        decThread->start();
    }
}

void MainWindow::cancelThread()
{
    if (encThread->isRunning()) encThread->quit();
    if (decThread->isRunning()) decThread->quit();
    endProcessing();
}

/*
 * Update Favbutton.
 */

void MainWindow::updateFavButton()
{
    QList<QDir> dirList = Directories::getFavDir();
    QList<QAction*> leftfavs, rightfavs;
    foreach (const QDir& dir, dirList)
    {
        QAction* leftaction = new QAction(dir.dirName(), this);
        leftaction->setData(QDir::fromNativeSeparators(dir.absolutePath()));
        QAction* rightaction = new QAction(dir.dirName(), this);
        rightaction->setData(QDir::fromNativeSeparators(dir.absolutePath()));
        leftfavs.append(leftaction);
        rightfavs.append(rightaction);
    }
    ui->leftFavButton->addActions(leftfavs);
    ui->rightFavButton->addActions(rightfavs);
}

/*
 * Settings.
 */

void MainWindow::modeSetting(QAction *action)
{
    switch (action->data().toInt())
    {
        case 0:
        {
            mode = ENCRYPT;
            break;
        }
        case 1:
        {
            mode = DECRYPT;
            break;
        }
    }
}

void MainWindow::algorithmSelected(QAction *action)
{
    switch (action->data().toInt())
    {
        case 0:
        {
            encryptor->setAlgorithm(Encryptor::AES_256);
            break;
        }
        case 1:
        {
            encryptor->setAlgorithm(Encryptor::RSA);
            break;
        }
    }
}

void MainWindow::dirSetting()
{
    Directories* settings = new Directories(this);
    connect (settings, SIGNAL(keyPathDirChanged(const QString&)), encryptor, SLOT(setKeyPath(const QString&)));
    connect (settings, SIGNAL(updateFavDir()), this, SLOT(updateFavButton()));
    settings->exec();
    disconnect (settings, SIGNAL(keyPathDirChanged(const QString&)), encryptor, SLOT(setKeyPath(const QString&)));
    disconnect (settings, SIGNAL(updateFavDir()), this, SLOT(updateFavButton()));
    delete settings;
}

/*
 * Left listview update SLOTS.
 */

void MainWindow::leftIndexChanged(QModelIndex index)
{
    if (index.sibling(index.row(), 0).data() == "..")
    {
        leftIndexChanged();
        return;
    }
    if (model->isDir(index))
    {
        ui->leftView->setRootIndex(index.sibling(index.row(), 0));
        updateleftPathBox(index.sibling(index.row(), 0));
    }
}

void MainWindow::leftIndexChanged()
{
    ui->leftView->setRootIndex(ui->leftView->rootIndex().parent());
    updateleftPathBox(ui->leftView->rootIndex());
}

void MainWindow::leftIndexChanged(QAction *action)
{
    ui->leftView->setRootIndex(model->index(action->data().toString()));
    updateleftPathBox(model->index(action->data().toString()));
}

void MainWindow::leftIndexChanged(int indexRow)
{
    QString pathIndex = pathModel->filePath(ui->leftPathBox->view()->currentIndex());
    ui->leftView->setRootIndex(model->index(pathIndex));
    updateleftPathBox(model->index(pathIndex));
}

/*
 * Right listview update SLOTS.
 */

void MainWindow::rightIndexChanged(QModelIndex index)
{
    if (index.sibling(index.row(), 0).data() == "..")
    {
        rightIndexChanged();
        return;
    }
    if (model->isDir(index))
    {
        ui->rightView->setRootIndex(index.sibling(index.row(), 0));
        updaterightPathBox(index.sibling(index.row(), 0));
    }
}

void MainWindow::rightIndexChanged()
{
    ui->rightView->setRootIndex(ui->rightView->rootIndex().parent());
    updaterightPathBox(ui->rightView->rootIndex());
}

void MainWindow::rightIndexChanged(QAction *action)
{
    ui->rightView->setRootIndex(model->index(action->data().toString()));
    updaterightPathBox(model->index(action->data().toString()));
}

void MainWindow::rightIndexChanged(int indexRow)
{
    QString pathIndex = pathModel->filePath(ui->rightPathBox->view()->currentIndex());
    ui->rightView->setRootIndex(model->index(pathIndex));
    updaterightPathBox(model->index(pathIndex));
}

/*
 * Pathbox update SLOTS.
 */

void MainWindow::updateleftPathBox(QModelIndex index)
{
    QString dirName = model->filePath(index);
    ui->leftPath->setText(QDir::toNativeSeparators(dirName));
    index = pathModel->index(dirName);
    if (pathModel->canFetchMore(index))
        pathModel->fetchMore(index);
    ui->leftPathBox->view()->setRootIndex(index.parent());
    ui->leftPathBox->setRootModelIndex(index.parent());
    ui->leftPathBox->setCurrentIndex(index.row());
}

void MainWindow::updaterightPathBox(QModelIndex index)
{
    QString dirName = model->filePath(index);
    ui->rightPath->setText(QDir::toNativeSeparators(dirName));
    index = pathModel->index(dirName);
    if (pathModel->canFetchMore(index))
        pathModel->fetchMore(index);
    ui->rightPathBox->view()->setRootIndex(index.parent());
    ui->rightPathBox->setRootModelIndex(index.parent());
    ui->rightPathBox->setCurrentIndex(index.row());
}

/*
 * About.
 */

void MainWindow::aboutWindow()
{
    about *tmp = new about();
    tmp->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
