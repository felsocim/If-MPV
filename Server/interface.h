#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <QDebug>
#include <QJsonArray>
#include <QFileDialog>
#include <QDirIterator>
#include <QStringList>
#include <QFileInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QList>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include "player.h"
#include "shared.h"
#include "automate.h"

namespace Ui
{
    class Interface;
}

class Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit Interface(QWidget *parent = 0);
    ~Interface();
    void launchServer();
    void replyToClient(kTransfer type, QJsonArray data);
    void sendState();
    void sendCurrentMetadata();

private:
    Ui::Interface *ui;
    Player * player;
    bool nowPlaying;
    QString currentMusicFolder;
    QStringList * musicFileList;
    QStringList * playlists;
    QLocalServer * server;
    QLocalSocket * client;
    //  Variables concernant l'état de MPV
    phase etat;
    int volume;
    int position;
    QString nomFichier;
    QString nomPlaylist;

signals:
    void signalClient(kCommand);

private slots:
    void startPlayerInstance();
    void selectMusicFolder();
    void terminate();
    void musicDiscovery();
    void receiver(QJsonObject object);
    void propertyStatus(int property, QJsonValue value);
    void incomingConnection();
    void listen();
    void onClientDisconnect();
    // slot concernant l'automate
    void message(phase);
};

#endif // INTERFACE_H
