#ifndef SONG_H
#define SONG_H

#include "playlist.h"
#include "myitem.h"

#include <QUrl>

class Song : public MyItem
{
public:
    Song();
    Song(QUrl loc);
    QUrl getLocation() { return location; }
    void setLocation(QUrl location) { this->location = location; }
    void setCategory(Playlist*);

    MyItem* getParent() const override;
    QString getDataToDisplay() const override;
    void setDataToDisplay(QString) override;

private:
    QUrl location;
    Playlist* category;
};

#endif // SONG_H
