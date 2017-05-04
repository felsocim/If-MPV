#ifndef REMOTECONTROLLER_H
#define REMOTECONTROLLER_H

#include <iostream>
#include <QMainWindow>
#include <QLocalSocket>
#include <QtConcurrent/QtConcurrent>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QDataStream>
#include <QByteArray>
#include <QStringList>
#include <QModelIndex>
#include "../Server/shared.h"

namespace Ui {
class RemoteController;
}

class RemoteController : public QMainWindow
{
    Q_OBJECT

public:
    explicit RemoteController(QWidget *parent = 0);
    ~RemoteController();

private:
    Ui::RemoteController *ui;
    QLocalSocket * me;
    void joinReceiver();
    void sendCommand(kCommand command, QJsonArray parameters);
    bool isPlaylist;
    bool isPlaying;
    bool isShuffle;
    bool isMute;
private slots:
    void listener();
    void onPlayPause();
    void showNowPlaying();
    void showAllMusic();
    void showPlaylists();
    void onSongSelection(QModelIndex item);
    void onPlaylistSelection(QModelIndex item);
    void onPrevious();
    void onNext();
    void onShuffle();
    void onMute();
    void onVolumeChange(int val);
    void onPositionChange(int val);
};

#endif // REMOTECONTROLLER_H
