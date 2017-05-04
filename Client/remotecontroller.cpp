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
    ui->frameCurrentPlaylist->hide();
    ui->framePreferences->hide();
    this->isPlaying = false;
    this->isMute = false;
    this->isShuffle = false;
    this->isPlaylist = false;

    QObject::connect(this->me, SIGNAL(readyRead()), this, SLOT(listener()));
    QObject::connect(ui->buttonPlayPause, SIGNAL(clicked(bool)), this, SLOT(onPlayPause()));
    QObject::connect(ui->buttonBack, SIGNAL(clicked(bool)), this, SLOT(onPrevious()));
    QObject::connect(ui->buttonNext, SIGNAL(clicked(bool)), this, SLOT(onNext()));
    QObject::connect(ui->buttonShuffle, SIGNAL(clicked(bool)), this, SLOT(onShuffle()));
    QObject::connect(ui->buttonMute, SIGNAL(clicked(bool)), this, SLOT(onMute()));
    QObject::connect(ui->buttonConnectToRemoteServer, SIGNAL(clicked(bool)), this, SLOT(onConnectSynchronize()));
    QObject::connect(ui->buttonSaveCurrent, SIGNAL(clicked(bool)), this, SLOT(onCurrentSave()));
    QObject::connect(ui->buttonShowCurrent, SIGNAL(clicked(bool)), this, SLOT(showCurrentPlaylist()));
    QObject::connect(ui->sliderVolume, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChange(int)));
    QObject::connect(ui->sliderPosition, SIGNAL(valueChanged(int)), this, SLOT(onPositionChange(int)));
    QObject::connect(ui->actionNowPlaying, SIGNAL(triggered(bool)), this, SLOT(showNowPlaying()));
    QObject::connect(ui->actionAllMusic, SIGNAL(triggered(bool)), this, SLOT(showAllMusic()));
    QObject::connect(ui->actionPlaylists, SIGNAL(triggered(bool)), this, SLOT(showPlaylists()));
    QObject::connect(ui->actionPreferences, SIGNAL(triggered(bool)), this, SLOT(showPreferences()));
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
            case kMetadata:
                this->ui->labelMediaTitle->setText(data[0].toString());
                this->ui->labelMediaArtist->setText(data[1].toString());
                this->ui->labelMediaAlbum->setText(data[2].toString());
                this->ui->labelMediaGenre->setText(data[3].toString());
                break;
            case kCurrentState:
                this->majAutomate( (phase)data[0].toInt() );
                this->ui->sliderVolume->setValue( data[1].toInt() );
                this->ui->labelVolume->setText(QString::number(data[1].toInt())+"%");
                this->ui->sliderPosition->setValue(data[2].toInt());
                this->setNomFichier( data[3].toString() );
                this->setNomPlaylist( data[4].toString() );
                break;
            case kCurrentPlaylistContent:
                ui->listCurrentPlaylist->clear();
                for(int i = 0; i < data.size(); i++)
                {
                    ui->listCurrentPlaylist->addItem(data[i].toString());
                }
                break;
        }

        qDebug() << "Received JSON: " + QString::fromUtf8(bytes.data(), bytes.length());
    }
}

void RemoteController::majAutomate(phase p){
    QIcon icon_pause(":/size24/media-playback-pause.png");

    switch(p){
        case kPhaseInitial :
            this->isPlaying = true;
            ui->buttonPlayPause->setIcon(icon_pause);
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
    this->sendCommand(kSetProperty, QJsonArray() << "pause" << (this->isPlaying));
    this->isPlaying = !this->isPlaying;

    if(this->isPlaying)
    {
        QIcon icon(":/size24/media-playback-pause.png");
        ui->buttonPlayPause->setIcon(icon);
    }
    else
    {
        QIcon icon(":/size24/media-playback-start.png");
        ui->buttonPlayPause->setIcon(icon);
    }
}

void RemoteController::showNowPlaying()
{
    ui->frameAllMusic->hide();
    ui->framePlaylists->hide();
    ui->framePreferences->hide();
    ui->frameCurrentPlaylist->hide();
    ui->frameNowPlaying->show();
}

void RemoteController::showAllMusic()
{
    ui->frameNowPlaying->hide();
    ui->framePlaylists->hide();
    ui->framePreferences->hide();

    this->sendCommand(kGetMusicList, QJsonArray() << true);

    ui->frameAllMusic->show();
}

void RemoteController::onSongSelection(QModelIndex item)
{
    if(this->isPlaylist) ui->listCurrentPlaylist->clear();
    std::cout << "Selected: " + item.data().toString().toStdString() << std::endl;
    this->sendCommand(kLoadFile, QJsonArray() << item.data().toString());
    ui->listCurrentPlaylist->addItem(item.data().toString());
    this->isPlaying = true;
    this->isPlaylist = false;
    QIcon icon(":/size24/media-playback-pause.png");
    ui->buttonPlayPause->setIcon(icon);
    this->nomFichier = item.data().toString();
}

void RemoteController::showPlaylists()
{
    ui->frameNowPlaying->hide();
    ui->frameAllMusic->hide();
    ui->framePreferences->hide();

    this->sendCommand(kGetPlaylists, QJsonArray() << true);

    ui->framePlaylists->show();
}

void RemoteController::showPreferences()
{
    ui->frameNowPlaying->hide();
    ui->frameAllMusic->hide();
    ui->framePlaylists->hide();
    ui->framePreferences->show();
}

void RemoteController::onPlaylistSelection(QModelIndex item)
{
    std::cout << "Selected playlist: " + item.data().toString().toStdString() << std::endl;
    this->sendCommand(kLoadPlaylist, QJsonArray() << item.data().toString());
    this->isPlaying = true;
    this->isPlaylist = true;
    this->nomPlaylist = item.data().toString();
    this->sendCommand(kGetCurrentPlaylistContent, QJsonArray() << item.data().toString());
}

void RemoteController::onPrevious()
{
    if(this->isPlaylist)
    {
        this->sendCommand(kPlaylistPrev, QJsonArray() << "weak");
        this->isPlaylist = true;
    }
}

void RemoteController::onNext()
{
    if(this->isPlaylist)
    {
        this->sendCommand(kPlaylistNext, QJsonArray() << "weak");
        this->isPlaylist = true;
    }
}

void RemoteController::onShuffle()
{
    if(this->isPlaylist)
    {
        this->sendCommand(kShuffle, QJsonArray() << "null");
        this->isPlaylist = true;
    }
}

void RemoteController::onMute()
{
    this->sendCommand(kSetProperty, QJsonArray() << "mute" << (!this->isMute));
    this->isMute = !this->isMute;

    if(this->isMute)
    {
        QIcon icon(":/size24/audio-volume-high.png");
        this->ui->buttonMute->setIcon(icon);
    }
    else
    {
        QIcon icon(":/size24/audio-volume-muted.png");
        this->ui->buttonMute->setIcon(icon);
    }
}

void RemoteController::onVolumeChange(int val)
{
    this->ui->labelVolume->setText(QString::number(val) + "%");
    this->sendCommand(kSetProperty, QJsonArray() << "volume" << val);
}

void RemoteController::onPositionChange(int val)
{
    this->sendCommand(kSetProperty, QJsonArray() << "percent-pos" << val);
}

void RemoteController::onConnectSynchronize()
{
    this->joinReceiver();
}

void RemoteController::setNomFichier(QString s)
{
    this->nomFichier = s;
    this->sendCommand(kGetMetadata, QJsonArray() << "null");
}

void RemoteController::setNomPlaylist(QString s)
{
    this->nomPlaylist = s;
}

void RemoteController::onCurrentSave()
{
    QStringList * current(new QStringList);
    int limit = ui->listCurrentPlaylist->count();

    for(int i = 0; i < limit; i++)
    {
        current->append(ui->listCurrentPlaylist->item(i)->text());
    }

    this->sendCommand(kSaveCurrentPlaylist, QJsonArray::fromStringList(*current));
}

void RemoteController::showCurrentPlaylist()
{
    ui->frameNowPlaying->hide();
    ui->frameAllMusic->hide();
    ui->framePlaylists->hide();
    ui->framePreferences->hide();
    ui->frameCurrentPlaylist->show();
}
