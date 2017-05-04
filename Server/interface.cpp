#include "interface.h"
#include "ui_interface.h"

QStringList allowedExtensions = {"mp3", "wav"};
QStringList allowedPlaylistExtensions = {"m3u"};

Interface::Interface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Interface),
    musicFileList(new QStringList),
    playlists(new QStringList),
    server(new QLocalServer)
{
    ui->setupUi(this);

    QObject::connect(ui->runMpvServer, SIGNAL(clicked(bool)), this, SLOT(startPlayerInstance()));
    QObject::connect(ui->menuOpenMusicFolder, SIGNAL(triggered(bool)), this, SLOT(selectMusicFolder()));
    QObject::connect(ui->actionChooseFolder, SIGNAL(triggered(bool)), this, SLOT(selectMusicFolder()));
    QObject::connect(ui->menuExit, SIGNAL(triggered(bool)), this, SLOT(terminate()));
    QObject::connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(terminate()));
    QObject::connect(ui->buttonScanMusicFolder, SIGNAL(clicked(bool)), this, SLOT(musicDiscovery()));
    QObject::connect(this->server, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
}

Interface::~Interface()
{
    delete ui;
    this->player->terminate();
    this->player->wait();
}

void Interface::startPlayerInstance()
{
    if(ui->serverName->text().isEmpty())
    {
        ui->statusBar->showMessage("Please, provide a server name your clients will connect to.");
        return;
    }

    this->player = new Player("mpvsocket");
    QObject::connect(this->player, SIGNAL(response(QJsonObject)), this, SLOT(receiver(QJsonObject)));
    QObject::connect(this->player, SIGNAL(observer(int,QJsonValue)), this, SLOT(propertyStatus(int,QJsonValue)));
    this->player->start();
    this->launchServer();
}

void Interface::selectMusicFolder()
{
    this->currentMusicFolder = QFileDialog::getExistingDirectory(this, "Select music folder...", "/home", QFileDialog::ShowDirsOnly);
    ui->editSelectedFolder->setText(this->currentMusicFolder);

    if(!this->currentMusicFolder.isEmpty())
    {
        ui->buttonScanMusicFolder->setEnabled(true);
    }
}

void Interface::terminate()
{
    this->player->setRunning(false);
    this->player->terminate();
    this->player->wait();
    QApplication::exit(0);
}

void Interface::musicDiscovery()
{
    if(ui->editSelectedFolder->text().isEmpty())
    {
        ui->statusBar->showMessage("You must provide a valid folder path!", 5000);
        return;
    }

    if(!this->musicFileList->isEmpty())
        this->musicFileList->clear();

    if(!this->playlists->isEmpty())
        this->playlists->clear();

    QDirIterator iterator(this->currentMusicFolder, QDirIterator::Subdirectories);

    ui->buttonScanMusicFolder->setEnabled(false);

    ui->labelFileDiscoveryResults->setText("Music file discovery in progress...");

    while(iterator.hasNext())
    {
        if(allowedExtensions.contains(iterator.fileInfo().suffix(), Qt::CaseInsensitive))
            this->musicFileList->append(iterator.filePath());
        else if (allowedPlaylistExtensions.contains(iterator.fileInfo().suffix(), Qt::CaseInsensitive))
            this->playlists->append(iterator.filePath());

        qDebug() << iterator.next();
    }

    ui->buttonScanMusicFolder->setEnabled(true);

    ui->labelFileDiscoveryResults->setText("Successfully discovered " + QString::number(this->musicFileList->size()) + " music files and " + QString::number(this->playlists->size()) + " playlists.");
}

void Interface::receiver(QJsonObject object)
{

}

void Interface::propertyStatus(int property, QJsonValue value)
{
    ui->labelFileDiscoveryResults->setText(QString::number(value.toInt()));
}

void Interface::launchServer()
{
    QString name = ui->serverName->text();

    QLocalServer::removeServer(name);
    if(!this->server->listen(name))
    {
        ui->statusBar->showMessage("Unable to start server listening!");
    }
}

void Interface::incomingConnection()
{
    if(this->client == NULL) this->client = this->server->nextPendingConnection();

    QObject::connect(this->client, SIGNAL(disconnected()), this->client, SLOT(deleteLater()));
    QObject::connect(this->client, SIGNAL(readyRead()), this, SLOT(listen()));
}

void Interface::listen()
{
    while(this->client->canReadLine())
    {
        QByteArray data = this->client->readLine().trimmed();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if( ((kCommand)obj["command"].toInt()) < kGetMusicList )
        {
            this->player->sendCommand((kCommand)obj["command"].toInt(), obj["parameters"].toArray());
            qDebug() << "Is MPV command: " + QString::fromUtf8(data.data(), data.length());
        }
        else //is this server command
        {
            switch(((kCommand)obj["command"].toInt()))
            {
                case kGetMusicList:
                    this->replyToClient(kMusicList, QJsonArray::fromStringList(*this->musicFileList));
                    break;
                case kGetPlaylists:
                    this->replyToClient(kPlaylistList, QJsonArray::fromStringList(*this->playlists));
                    break;
            }
        }


        //signal to the state machine


        qDebug() << "Received JSON: " + QString::fromUtf8(data.data(), data.length());
    }
}

void Interface::replyToClient(kTransfer type, QJsonArray data)
{
    QJsonObject cmd;

    cmd.insert("type", type);
    cmd.insert("data", data);

    QByteArray bytes = QJsonDocument(cmd).toJson(QJsonDocument::Compact) + "\n";

    if(this->client != NULL)
    {
        this->client->write(bytes.data(), bytes.length());
        this->client->flush();

        std::cout << "Socket reply sent." << std::endl;
    }
}
