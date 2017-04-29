#include "player.h"

QString mpvCommands[] = {"seek", "set_property", "get_property", "playlist-next", "playlist-prev", "loadfile", "loadlist", "playlist-shuffle", "quit"};

Player::Player(QString serverName, QString fileName, QObject * parent) : QThread(parent), mpv(new QLocalSocket)
{
    this->player = serverName;
    this->file = fileName;
}

Player::~Player()
{
    delete this->mpv;
}

void Player::run()
{
    QStringList toRun;

    toRun << "--input-ipc-server=/tmp/" + this->player << this->file;

    QProcess instance;

    instance.start("mpv", toRun);
    instance.waitForFinished(-1);
}

void Player::sendCommand(kCommand command, QJsonArray parameters)
{
    if(!this->mpv->isOpen())
    {
        this->mpv->connectToServer("/tmp/" + this->player);
    }

    QJsonObject cmd;

    parameters.insert(0, mpvCommands[command]);

    cmd.insert("command", parameters);

    QByteArray bytes = QJsonDocument(cmd).toJson(QJsonDocument::Compact) + "\n";

    if(this->mpv != NULL)
    {
        this->mpv->write(bytes.data(), bytes.length());
        this->mpv->flush();

        std::cout << "Socket command sent." << std::endl;
    }
}
