/**
  *Marek Felsoci, Arnaud Pinsun
  *
  **/
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

    // initialisation des variables
    this->nomFichier="";
    this->nomPlaylist="";
    this->volume=100;
    this->position=0;
    this->client = NULL;

    QObject::connect(ui->runMpvServer, SIGNAL(clicked(bool)), this, SLOT(startPlayerInstance()));
    QObject::connect(ui->menuOpenMusicFolder, SIGNAL(triggered(bool)), this, SLOT(selectMusicFolder()));
    QObject::connect(ui->actionChooseFolder, SIGNAL(triggered(bool)), this, SLOT(selectMusicFolder()));
    QObject::connect(ui->menuOpenMusicFolder, SIGNAL(triggered(bool)), this, SLOT(selectMusicFolder()));
    QObject::connect(ui->menuExit, SIGNAL(triggered(bool)), this, SLOT(terminate()));
    QObject::connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(terminate()));
    QObject::connect(ui->menuExit, SIGNAL(triggered(bool)), this, SLOT(terminate()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(onMenuHelp()));
    QObject::connect(ui->buttonScanMusicFolder, SIGNAL(clicked(bool)), this, SLOT(musicDiscovery()));
    QObject::connect(this->server, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
}

Interface::~Interface()
{
    delete ui;
    this->player->terminate();
    this->player->wait();
}

void Interface::onMenuHelp()
{
    QMessageBox::about(this, "About Music Player", "This is a simple client-server user interface for MPV music player developed within university studies by Marek Felsoci and Arnaud Pinsun.");
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
    QObject::connect(this->client, SIGNAL(disconnected()), this, SLOT(onClientDisconnect()));
    QObject::connect(this->client, SIGNAL(readyRead()), this, SLOT(listen()));

    this->sendState();
}

void Interface::onClientDisconnect()
{
    this->client = NULL;
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

            //Traitement messages automate
            if( ((kCommand)obj["command"].toInt()) == kLoadFile )
            {
                this->nomFichier = obj["parameters"].toArray()[0].toString();
                this->sendState();
                this->sendCurrentMetadata();
            }

            if( ((kCommand)obj["command"].toInt()) == kLoadPlaylist )
            {
                this->nomPlaylist = obj["parameters"].toArray()[0].toString();
                this->sendState();
            }

            if( ((kCommand)obj["command"].toInt()) == kSetProperty )
            {
                if( obj["parameters"].toArray()[0].toString().compare("mute") == 0 )
                {
                    emit signalClient(kButtonMuet);
                }
                else if ( obj["parameters"].toArray()[0].toString().compare("pause") == 0 )
                {
                    emit signalClient(kButtonPause);
                }
                else if ( obj["parameters"].toArray()[0].toString().compare("volume") == 0 )
                {
                    this->volume = obj["parameters"].toArray()[1].toInt();
                    this->sendState();
                }
                else if ( obj["parameters"].toArray()[0].toString().compare("percent-pos") == 0 )
                {
                    this->position = obj["parameters"].toArray()[1].toInt();
                    this->sendState();
                }
            }

            if( ((kCommand)obj["command"].toInt()) == kShuffle )
            {
                emit signalClient(kButtonAleat);
            }
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
                case kMetadata:
                    this->sendCurrentMetadata();
                    break;
                case kSaveCurrentPlaylist:
                    this->savePlaylist(obj["parameters"].toArray());
                    break;
                case kGetCurrentPlaylistContent:
                    this->replyToClient(kCurrentPlaylistContent, this->loadPlaylistContent(obj["parameters"].toArray()[0].toString()));
                    break;
            }
        }


        //signal to the state machine


        qDebug() << "Received JSON: " + QString::fromUtf8(data.data(), data.length());
    }
}

QJsonArray Interface::loadPlaylistContent(QString playlist)
{
    QFile playlistFile(playlist);
    QStringList * name(new QStringList);

    if(!playlistFile.open(QIODevice::ReadOnly)) return QJsonArray();

    QTextStream stream(&playlistFile);

    if(stream.status() != QTextStream::Ok) return QJsonArray();

    while(!stream.atEnd())
    {
        QString line = stream.readLine();

        if(!line.startsWith("#"))
            name->append(line);
    }

    playlistFile.close();

    return QJsonArray::fromStringList(*name);
}

void Interface::savePlaylist(QJsonArray songs)
{
    if(this->currentMusicFolder.isEmpty()) return;

    QDateTime now = QDateTime::currentDateTime();

    QString filename = this->currentMusicFolder + "/MyPlaylist-" + now.toString("hh-mm-ss-dd-MM-YYYY") + ".m3u";

    QFile playlistFile(filename);

    qDebug() << filename;

    if(!playlistFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;

    QTextStream stream(&playlistFile);

    if(stream.status() != QTextStream::Ok) return;

    stream << "#EXTM3U" << endl;

    foreach (QJsonValue path, songs)
    {
        TagLib::FileRef file(path.toString().toStdString().data());
        TagLib::String title = file.tag()->title();
        int duration = file.audioProperties()->lengthInSeconds();

        stream << "#EXTINF:" << QString::number(duration) << "," << QString(title.toCString(true)) << endl;
        stream << path.toString() << endl;
    }

    playlistFile.close();
}

void Interface::replyToClient(kTransfer type, QJsonArray data)
{
    QJsonObject cmd;

    cmd.insert("type", type);
    cmd.insert("data", data);

    QByteArray bytes = QJsonDocument(cmd).toJson(QJsonDocument::Compact) + "\n";

    if(this->client != NULL)
    {
        std::cout << "je veux envoyer un truc" << std::endl;
        this->client->write(bytes.data(), bytes.length());
        this->client->flush();

        std::cout << "Socket reply sent." << std::endl;
    }
}

void Interface::message(phase p){

    this->etat = p;

    switch(p){
        case kPhaseInitial :
            qDebug()<<"phase init";
        break;

        case kPhaseMuet:
            qDebug()<<"etat muet";
        break;

        case kPhaseAleat:
            qDebug()<<"etat aleat";
        break;

        case kPhasePause:
            qDebug()<<"etat pause";
        break;

        case kPhaseMuetAleat:
            qDebug()<<"etat muet-aleat";
        break;

        case kPhaseMuetPause:
            qDebug()<<"etat muet-pause";
        break;

        case kPhaseAleatPause:
            qDebug()<<"etat aleat-pause";
        break;

        case kPhaseMuetAleatPause:
            qDebug()<<"etat muet-aleat-pause";
        break;
    }
    this->sendState();

}

// format des messages transmis : etat_automate / volume / position / musique lue / playlist courante
void Interface::sendState(){
    QJsonArray currentState;
    currentState << this->etat;
    currentState << this->volume;
    currentState << this->position;
    currentState << this->nomFichier;
    currentState << this->nomPlaylist;

    this->replyToClient(kCurrentState,currentState);
}

void Interface::sendCurrentMetadata()
{
    TagLib::FileRef file(this->nomFichier.toStdString().data());
    TagLib::String title = file.tag()->title();
    TagLib::String artist = file.tag()->artist();
    TagLib::String album = file.tag()->album();
    TagLib::String genre = file.tag()->genre();

    qDebug() << title.toCString(true);

    QJsonArray parameters;

    parameters << title.toCString(true) << artist.toCString(true) << album.toCString(true) << genre.toCString(true);

    this->replyToClient(kMetadata, parameters);
}
