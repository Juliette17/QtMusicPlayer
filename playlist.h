#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>
#include <QVariant>
#include <QMediaPlaylist>
#include "myitem.h"

class Song;

class Playlist : public MyItem
{
public:
    Playlist();
    Playlist (QString name);
    QString getPlaylistName() const { return playlistName; }
    void setPlaylistName(QString playlistName) { this->playlistName = playlistName; }
    std::vector<Song*> getSongs() {return songs; }
    void addToPlaylist(Song* s) { songs.push_back(s); }
    QMediaPlaylist* getMediaPlaylist() { return tracks; }

    MyItem* getParent() const override;
    QString getDataToDisplay() const override;
    void setDataToDisplay(QString) override;

private:
    std::vector<Song*> songs;
    QString playlistName;
    QMediaPlaylist* tracks;
};

#endif // PLAYLIST_H
