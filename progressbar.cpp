#include "progressbar.h"
#include "ui_progressbar.h"

progressbar::progressbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::progressbar)
{
    ui->setupUi(this);
    connect (ui->pauseButton, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
    connect (ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

void progressbar::updateCurrent(QString stat)
{
    ui->bytesLabel->setText(stat);
}

void progressbar::updateAll(QString stat)
{
    ui->progressLabel->setText(stat);
}

void progressbar::updateFile(QString file)
{
    ui->pathLabel->setText(file);
}

void progressbar::updateStatus(const QString &stat)
{
    ui->currentStatus->setText(stat);
}

void progressbar::pause(bool stat)
{
    emit pauseThread(stat);
}

void progressbar::cancel()
{
    emit cancelThread();
}

progressbar::~progressbar()
{
    delete ui;
}
