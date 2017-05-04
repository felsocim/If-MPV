#ifndef SHARED_H
#define SHARED_H

enum kCommand
{
    kSeek,
    kSetProperty,
    kGetProperty,
    kObserveProperty,
    kPlaylistNext,
    kPlaylistPrev,
    kLoadFile,
    kLoadPlaylist,
    kShuffle,
    kQuit,
    kGetMusicList
};

enum kProperty
{
    kVolume,
    kTime,
    kSwitch,
    kMute
};

enum kTransfer
{
    kMusicList,
    kPlaylistList,
    kRadioList
};

#endif // SHARED_H
