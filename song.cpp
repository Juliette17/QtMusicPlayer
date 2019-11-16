#include "song.h"
#include <QFileInfo>

Song::Song()
{
}

Song::Song(QUrl loc) : location(loc)
{
}

MyItem* Song::getParent() const
{
    return category;
}

QString Song::getDataToDisplay() const
{
    return QFileInfo(location.path()).fileName();
}

void Song::setCategory(Playlist* p)
{
    category = p;
}

void Song::setDataToDisplay(QString fileName)
{

}
