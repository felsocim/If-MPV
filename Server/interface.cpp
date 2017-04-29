#include "interface.h"
#include "ui_interface.h"

Interface::Interface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Interface)
{
    ui->setupUi(this);

    QObject::connect(ui->runMpvServer, SIGNAL(clicked(bool)), this, SLOT(startPlayerInstance(bool)));
}

Interface::~Interface()
{
    delete ui;
    this->player->terminate();
    this->player->wait();
}

void Interface::startPlayerInstance(bool clicked)
{
    qDebug() << "hello";
    this->player = new Player(ui->serverName->text(), ui->fileName->text());
    this->player->start();
}

void Interface::on_volTest_clicked()
{
    this->player->sendCommand(kSetProperty, QJsonArray() << "volume" << 52.0);
}
