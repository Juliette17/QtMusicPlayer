#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "categorymodel.h"

QT_BEGIN_NAMESPACE
class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QVideoProbe;
class QVideoWidget;
class QAudioProbe;
QT_END_NAMESPACE

namespace Ui {
class Player;
}

class Player : public QMainWindow
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = 0);
    ~Player();

    void addToPlaylist(const QList<QUrl> &urls);
    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void play();
    void pause();
    void stop();
    void changeVolume(int volume);
    void changeMuting(bool muting);

private slots:
    void on_actionOpen_triggered();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);
    void categoryChanged(const QModelIndex&);

    void playClicked();
    void muteClicked();
    void onVolumeSliderValueChanged();

    void on_actionNew_triggered();

private:
    void updateDurationInfo(qint64 currentInfo);

    Ui::Player *ui;

    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    CategoryModel *categoryModel;

    qint64 m_duration;

    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    bool m_playerMuted = false;
};

#endif // PLAYER_H
