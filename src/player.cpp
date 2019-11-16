#include "player.h"
#include "ui_player.h"

#include "categorymodel.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>

Player::Player(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);

    m_player = new QMediaPlayer(this);
    m_player->setAudioRole(QAudio::MusicRole);

    categoryModel = new CategoryModel(this);
    categoryModel->addCategory(new Playlist("second"));

    m_playlist = categoryModel->getCategories()[0]->getMediaPlaylist();
    m_player->setPlaylist(m_playlist);

    connect(m_player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);

    ui->treeView->setModel(categoryModel);

    ui->songsView->setModel(categoryModel);
    ui->songsView->setRootIndex(categoryModel->index(0, 0, QModelIndex()));
    qDebug() << "first root" << categoryModel->index(0, 0, QModelIndex());

    connect(ui->songsView, &QAbstractItemView::activated, this, &Player::jump);

    ui->categoriesView->setModel(categoryModel);
    ui->categoriesView->setCurrentIndex(categoryModel->index(0, 0, QModelIndex()));
    ui->categoriesView->setRootIndex(QModelIndex());

    connect(ui->categoriesView, &QAbstractItemView::activated, this, &Player::categoryChanged);

    ui->m_slider->setRange(0, m_player->duration() / 1000);
    connect(ui->m_slider, &QSlider::sliderMoved, this, &Player::seek);

    ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->m_playButton, &QAbstractButton::clicked, this, &Player::playClicked);

    ui->m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->m_stopButton->setEnabled(false);
    connect(ui->m_stopButton, &QAbstractButton::clicked, m_player, &QMediaPlayer::stop);

    ui->m_nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(ui->m_nextButton, &QAbstractButton::clicked, m_playlist, &QMediaPlaylist::next);

    ui->m_previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(ui->m_previousButton, &QAbstractButton::clicked, m_playlist, &QMediaPlaylist::previous);

    ui->m_muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    connect(ui->m_muteButton, &QAbstractButton::clicked, this, &Player::muteClicked);

    ui->m_volumeSlider->setRange(0, 100);
    connect(ui->m_volumeSlider, &QSlider::valueChanged, this, &Player::onVolumeSliderValueChanged);

    connect(ui->openButton, &QPushButton::clicked, this, &Player::on_actionOpen_triggered);

    setState(m_player->state());
    setVolume(m_player->volume());
    setMuted(this->isMuted());

    connect(this, &Player::play, m_player, &QMediaPlayer::play);
    connect(this, &Player::pause, m_player, &QMediaPlayer::pause);
    connect(this, &Player::changeVolume, m_player, &QMediaPlayer::setVolume);
    connect(this, &Player::changeMuting, m_player, &QMediaPlayer::setMuted);

    connect(m_player, &QMediaPlayer::stateChanged, this, &Player::setState);
    connect(m_player, &QMediaPlayer::volumeChanged, this, &Player::setVolume);
    connect(m_player, &QMediaPlayer::mutedChanged, this, &Player::setMuted);
}

Player::~Player()
{
    delete ui;
}

void Player::on_actionOpen_triggered()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    QStringList supportedMimeTypes = m_player->supportedMimeTypes();
    if (!supportedMimeTypes.isEmpty()) {
        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());
}

void Player::categoryChanged(const QModelIndex &index)
{
    if (index.isValid()) {
       QModelIndex i = categoryModel->index(index.row(), 0, QModelIndex());
       ui->categoriesView->setCurrentIndex(index);
       Playlist* p = static_cast<Playlist*>(index.internalPointer());
       m_playlist = p->getMediaPlaylist();
       m_player->setPlaylist(m_playlist);
       qDebug() << Q_FUNC_INFO << index << i << p->getDataToDisplay();
       ui->songsView->setRootIndex(index);
//       ui->songsView->setCurrentIndex(categoryModel->index(0, 0, index));
      // qDebug() << Q_FUNC_INFO << p->getDataToDisplay();
       categoryModel->categoryChanged();
    }
}

void Player::on_actionNew_triggered()
{
    QDialog* newCategory = new QDialog(this, Qt::Dialog);

}

void Player::addToPlaylist(const QList<QUrl> &urls)
{
    for (QUrl url: urls) {
        m_playlist->addMedia(url);
        categoryModel->addNewSong(url);
    }
}

void Player::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    ui->m_slider->setMaximum(m_duration);
}

void Player::positionChanged(qint64 progress)
{
    if (!ui->m_slider->isSliderDown())
        ui->m_slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());

        qDebug() << Q_FUNC_INFO << index.row() << static_cast<MyItem*>(index.internalPointer())->getParent();
        m_player->play();
        ui->songsView->setCurrentIndex(index);
    }
}

void Player::playlistPositionChanged(int currentItem)
{
    //ui->m_playlistView->setCurrentIndex(j_playlistModel->index(currentItem, 0));
    ui->songsView->setCurrentIndex(categoryModel->index(currentItem, 0, ui->categoriesView->currentIndex()));
}

void Player::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->m_labelDuration->setText(tStr);
}

QMediaPlayer::State Player::state() const
{
    return m_playerState;
}

void Player::setState(QMediaPlayer::State state)
{
    if (state != m_playerState) {
        m_playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            ui->m_stopButton->setEnabled(false);
            ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        case QMediaPlayer::PlayingState:
            ui->m_stopButton->setEnabled(true);
            ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            ui->m_stopButton->setEnabled(true);
            ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        }
    }
}

int Player::volume() const
{
    qreal linearVolume =  QAudio::convertVolume(ui->m_volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);

    return qRound(linearVolume * 100);
}

void Player::setVolume(int volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    ui->m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

bool Player::isMuted() const
{
    return m_playerMuted;
}

void Player::setMuted(bool muted)
{
    if (muted != m_playerMuted) {
        m_playerMuted = muted;

        ui->m_muteButton->setIcon(style()->standardIcon(muted
                ? QStyle::SP_MediaVolumeMuted
                : QStyle::SP_MediaVolume));
    }
}

void Player::playClicked()
{
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
}

void Player::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}

void Player::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}


