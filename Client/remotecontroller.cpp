#include "remotecontroller.h"
#include "ui_remotecontroller.h"

RemoteController::RemoteController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RemoteController),
    me(new QLocalSocket)
{
    ui->setupUi(this);
    this->joinReceiver();

    QObject::connect(this->me, SIGNAL(readyRead()), this, SLOT(listener()));
    QObject::connect(ui->buttonPlayPause, SIGNAL(clicked(bool)), this, SLOT(onPlayPause()));
}

RemoteController::~RemoteController()
{
    delete ui;
}

void RemoteController::joinReceiver()
{
    if(!this->me->isOpen())
    {
        this->me->connectToServer(ui->editServerName->text());
    }
}

void RemoteController::listener()
{
    while(this->me->canReadLine())
    {
        QByteArray data = this->me->readLine().trimmed();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if(obj.contains("response"))
        {

            //response treatement
        }

        qDebug() << "Received JSON: " + QString::fromUtf8(data.data(), data.length());
    }
}

void RemoteController::sendCommand(kCommand command, QJsonArray parameters)
{
    this->joinReceiver();

    QJsonObject cmd;

    cmd.insert("command", command);
    cmd.insert("parameters", parameters);

    QByteArray bytes = QJsonDocument(cmd).toJson(QJsonDocument::Compact) + "\n";

    if(this->me != NULL)
    {
        this->me->write(bytes.data(), bytes.length());
        this->me->flush();

        std::cout << "Socket command sent." << std::endl;
    }
}

void RemoteController::onPlayPause()
{
    this->sendCommand(kLoadFile, QJsonArray() << "/home/marek/QWE/test.mp3");
}
