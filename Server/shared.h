#ifndef SHARED_H
#define SHARED_H

enum phase {
    kPhaseInitial,
    kPhaseMuet,
    kPhaseAleat,
    kPhasePause,
    kPhaseMuetAleat,
    kPhaseMuetPause,
    kPhaseAleatPause,
    kPhaseMuetAleatPause
};

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
    kGetMusicList,
    kGetPlaylists,
    kButtonMuet,
    kButtonPause,
    kButtonAleat
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
    kRadioList,
    kCurrentState
};

#endif // SHARED_H
