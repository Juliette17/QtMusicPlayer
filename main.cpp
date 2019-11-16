#include "player.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Music Player");
    QCoreApplication::setOrganizationName("joltusek");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    Player p;
    p.resize(600,400);
    p.show();

    return a.exec();
}
