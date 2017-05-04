#include "remotecontroller.h"
#include "ui_remotecontroller.h"

RemoteController::RemoteController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RemoteController),
    me(new QLocalSocket)
{
    ui->setupUi(this);
    this->joinReceiver();
    ui->frameAllMusic->hide();
    ui->framePlaylists->hide();

    QObject::connect(this->me, SIGNAL(readyRead()), this, SLOT(listener()));
    QObject::connect(ui->buttonPlayPause, SIGNAL(clicked(bool)), this, SLOT(onPlayPause()));
    QObject::connect(ui->actionNowPlaying, SIGNAL(triggered(bool)), this, SLOT(showNowPlaying()));
    QObject::connect(ui->actionAllMusic, SIGNAL(triggered(bool)), this, SLOT(showAllMusic()));
    QObject::connect(ui->actionPlaylists, SIGNAL(triggered(bool)), this, SLOT(showPlaylists()));
    QObject::connect(ui->listMusic, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onSongSelection(QModelIndex)));
    QObject::connect(ui->listPlaylists, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onPlaylistSelection(QModelIndex)));
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
        QByteArray bytes = this->me->readLine().trimmed();

        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        QJsonObject obj = doc.object();

        QJsonArray data = obj["data"].toArray();

        switch( ((kTransfer)obj["type"].toInt()) )
        {
            case kMusicList:
                ui->listMusic->clear();
                for(int i = 0; i < data.size(); i++)
                {
                    ui->listMusic->addItem(data[i].toString());
                }
                break;
            case kPlaylistList:
                ui->listPlaylists->clear();
                for(int i = 0; i < data.size(); i++)
                {
                    ui->listPlaylists->addItem(data[i].toString());
                }
                break;
        }

        qDebug() << "Received JSON: " + QString::fromUtf8(bytes.data(), bytes.length());
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

void RemoteController::showNowPlaying()
{
    ui->frameAllMusic->hide();
    ui->framePlaylists->hide();
    ui->frameNowPlaying->show();
}

void RemoteController::showAllMusic()
{
    ui->frameNowPlaying->hide();
    ui->framePlaylists->hide();

    this->sendCommand(kGetMusicList, QJsonArray() << true);

    ui->frameAllMusic->show();
}

void RemoteController::onSongSelection(QModelIndex item)
{
    std::cout << "Selected: " + item.data().toString().toStdString() << std::endl;
    this->sendCommand(kLoadFile, QJsonArray() << item.data().toString());
}

void RemoteController::showPlaylists()
{
    ui->frameNowPlaying->hide();
    ui->frameAllMusic->hide();

    this->sendCommand(kGetPlaylists, QJsonArray() << true);

    ui->framePlaylists->show();
}

void RemoteController::onPlaylistSelection(QModelIndex item)
{
    std::cout << "Selected playlist: " + item.data().toString().toStdString() << std::endl;
    this->sendCommand(kLoadPlaylist, QJsonArray() << item.data().toString());
}
