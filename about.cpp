#include "about.h"
#include "ui_about.h"
#include <QMovie>

about::about(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);

    QMovie* movie = new QMovie(":/ICON/ABOUT.gif");
    ui->DOGE->setMovie(movie);
    movie->start();

    setFixedSize(size());

    connect (ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

about::~about()
{
    delete ui;
}
