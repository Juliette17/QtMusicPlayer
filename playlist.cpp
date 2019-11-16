#include "playlist.h"

Playlist::Playlist()
{
    tracks = new QMediaPlaylist();
}

Playlist::Playlist(QString name) : playlistName(name)
{
    tracks = new QMediaPlaylist();
}

MyItem* Playlist::getParent() const
{
    return nullptr;
}

QString Playlist::getDataToDisplay() const
{
    return playlistName;
}

void Playlist::setDataToDisplay(QString playlistName)
{
    this->playlistName = playlistName;
}

