﻿#include "global.h"
#include "BottomWidget.h"
#include <QHBoxLayout>
#include "MusicPlayer/musicPlayer.h"
#include "BesMessageBox.h"
#include "SettingManager.h"

BottomWidget::BottomWidget(QWidget *parent)
    :QWidget(parent)
{
    this->setMouseTracking(true);//详见 BesFramelessWidget.h 注释
    initLayout();
    initEntity();
    initConnection();
}

BottomWidget::~BottomWidget()
{

}

void BottomWidget::initLayout()
{
    QWidget* bottomWidgetContainer = new QWidget(this);
    bottomWidgetContainer->setObjectName("bottomWidgetContainer");
    bottomWidgetContainer->setMouseTracking(true);//详见 BesFramelessWidget.h 注释

    btnPreSong = new BesButton(bottomWidgetContainer);
    btnPlayAndPause = new BesButton(bottomWidgetContainer);
    btnNextSong = new BesButton(bottomWidgetContainer);
    btnPreSong->setObjectName("btnPreSong");
    btnPlayAndPause->setObjectName("btnPlayAndPause");
    btnNextSong->setObjectName("btnNextSong");
    btnPreSong->setFocusPolicy(Qt::NoFocus);
    btnPlayAndPause->setFocusPolicy(Qt::NoFocus);
    btnNextSong->setFocusPolicy(Qt::NoFocus);

    labelTimeCurrent = new QLabel(bottomWidgetContainer);
    labelTimeEnding = new QLabel(bottomWidgetContainer);
    labelTimeCurrent->setText("00:00");
    labelTimeEnding->setText("00:00");
    labelTimeCurrent->setObjectName("labelTimeCurrent");
    labelTimeEnding->setObjectName("labelTimeEnding");

    sliderSong = new QSlider(bottomWidgetContainer);
    sliderSound= new QSlider(bottomWidgetContainer);
    sliderSong->setObjectName("sliderSong");
    sliderSound->setObjectName("sliderSound");
    sliderSong->setOrientation(Qt::Horizontal);
    sliderSound->setOrientation(Qt::Horizontal);
    sliderSound->setMaximumWidth(100);
    sliderSong->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    sliderSound->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    btnSound = new BesButton(bottomWidgetContainer);
    btnPlayMode = new BesButton(bottomWidgetContainer);
    btnDesktopLyric = new BesButton(bottomWidgetContainer);
    btnSound->setObjectName("btnSound");
    btnPlayMode->setObjectName("btnPlayMode");
    btnDesktopLyric->setObjectName("btnDesktopLyric");
    btnSound->setFocusPolicy(Qt::NoFocus);
    btnPlayMode->setFocusPolicy(Qt::NoFocus);
    btnDesktopLyric->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout* hLayout = new QHBoxLayout(bottomWidgetContainer);
    hLayout->addSpacerItem(new QSpacerItem(15,20,QSizePolicy::Fixed,QSizePolicy::Fixed));
    hLayout->addWidget(btnPreSong);
    hLayout->addSpacerItem(new QSpacerItem(15,20,QSizePolicy::Fixed,QSizePolicy::Fixed));
    hLayout->addWidget(btnPlayAndPause);
    hLayout->addSpacerItem(new QSpacerItem(15,20,QSizePolicy::Fixed,QSizePolicy::Fixed));
    hLayout->addWidget(btnNextSong);
    hLayout->addSpacerItem(new QSpacerItem(15,20,QSizePolicy::Fixed,QSizePolicy::Fixed));
    hLayout->addWidget(labelTimeCurrent);
    hLayout->addWidget(sliderSong);
    hLayout->addWidget(labelTimeEnding);
    hLayout->addWidget(btnSound);
    hLayout->addWidget(sliderSound);
    hLayout->addWidget(btnPlayMode);
    hLayout->addWidget(btnDesktopLyric);

    QVBoxLayout* vLayoutContainer = new QVBoxLayout(this);
    vLayoutContainer->setMargin(0);
    vLayoutContainer->addWidget(bottomWidgetContainer);

    setLayout(vLayoutContainer);
}

void BottomWidget::initEntity()
{
    sliderSong->setRange(0,1000);
    sliderSound->setRange(0,128);
    AdjustingPos = false;
    posAdjust = 0;

    musicPlayer = new MusicPlayer(this);
    musicPlayer->setNotifyInterval(33);

    bInMakingMode = false;
}

void BottomWidget::initConnection()
{
    connect(sliderSong,SIGNAL(sliderPressed()),this, SLOT(onSliderSongPressed()));
    connect(sliderSong,SIGNAL(sliderMoved(int)),this, SLOT(onSliderSongMoved(int)));
    connect(sliderSong,SIGNAL(sliderReleased()),this, SLOT(onSliderSongReleased()));

    connect(sliderSound,SIGNAL(valueChanged(int)),musicPlayer,SLOT(setVolume(int)));
    connect(sliderSound,SIGNAL(sliderReleased()),this, SLOT(onSliderSoundReleased()));

    connect(musicPlayer, SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));
    connect(musicPlayer,SIGNAL(volumeChanged(int)),this,SLOT(volumeChanged(int)));

    connect(btnPlayAndPause, SIGNAL(clicked(bool)), this, SLOT(onPlayOrPause()));

    connect(musicPlayer, SIGNAL(errorOccur(int,QString)),this,SLOT(onErrorOccurs(int,QString)));

    sliderSound->setValue(SettingManager::GetInstance().data().volume);
}

void  BottomWidget::reloadMusic(QString musicPath)
{
    if(musicPlayer->state() != MusicPlayer::StoppedState )
        musicPlayer->stop();

    musicPlayer->setMusicPath(musicPath);

    if(musicPlayer->state() == MusicPlayer::PlayingState)
        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_pause.png\");}");
    else
        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_play.png\");}");
}

void BottomWidget::play()
{
    if(musicPlayer->getMusicPath().size() != 0)
    {
        musicPlayer->play();

        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_pause.png\");}");
    }
}

void BottomWidget::pause()
{
    musicPlayer->pause();

    if(musicPlayer->state() == MusicPlayer::PlayingState)
        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_pause.png\");}");
    else
        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_play.png\");}");
}

//从开头开始播放
void BottomWidget::playFromBegin()
{
    if(musicPlayer->getMusicPath().size() != 0)
    {
        musicPlayer->play();

        musicPlayer->seek(0);

        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_pause.png\");}");
    }
}

//自动判断播放还是暂停
void BottomWidget::autoPlayOrPause()
{
    if(musicPlayer->state() == MusicPlayer::PlayingState)
        pause();
    else
        play();
}

void BottomWidget::stop()
{
    if(musicPlayer->state() != MusicPlayer::StoppedState )
    {
        musicPlayer->stop();
        setStyleSheet("QPushButton#btnPlayAndPause{background-image:url(\":/resource/image/btn_play.png\");}");
     }
}

void BottomWidget::seek(quint64 pos)
{
     if(musicPlayer->state() != MusicPlayer::StoppedState )
         musicPlayer->seek(pos);
}

void BottomWidget::seekForward(quint64 step)
{
    if(musicPlayer->state() != MusicPlayer::StoppedState )
        musicPlayer->forwordSeek(step);
}

void BottomWidget::seekBackward(quint64 step)
{
    if(musicPlayer->state() != MusicPlayer::StoppedState )
        musicPlayer->backwardSeek(step);
}

//进入制作模式
void BottomWidget::enterMakingMode()
{
    bInMakingMode = true;
}

//退出制作模式
void BottomWidget::exitMakingMode()
{
    bInMakingMode = false;

}


void BottomWidget::durationChanged(qint64 duration)
{
    int ms = duration % 1000;
    duration = duration/1000;
    int s = duration % 60;
    int m = duration/60;

    QString timeLabel;
    timeLabel.sprintf("%.2d:%.2d.%.3d",m, s, ms);

    labelTimeEnding->setText(timeLabel);

}

void BottomWidget::positionChanged(int position)
{
    if(!AdjustingPos)
    {
        int pecentOfThousand = int(1.0 * position / musicPlayer->duration() * 1000);
        sliderSong->setValue(pecentOfThousand);
    }

    int ms = position % 1000;
    position = position/1000;
    int s = position % 60;
    int m = position/60;

    QString timeLabel;
    timeLabel.sprintf("%.2d:%.2d.%.3d",m, s, ms);

    labelTimeCurrent->setText(timeLabel);

}

void BottomWidget::onSliderSongMoved(int position)
{
    if(bInMakingMode)
        return;

    posAdjust = musicPlayer->duration() * position / 1000;
}

void BottomWidget::onSliderSongPressed()
{
    if(bInMakingMode)
        return ;

    AdjustingPos = true;
}

void BottomWidget::onSliderSongReleased()
{
    if(bInMakingMode)
        return ;

    AdjustingPos = false;
    musicPlayer->seek(posAdjust);
}

void BottomWidget::volumeChanged(int volume)
{
    sliderSound->setValue(volume);
}

void BottomWidget::onSliderSoundReleased()
{
    //每次结束音量的改变，需要即时保存
    SettingManager::GetInstance().data().volume = sliderSound->value();

    SettingManager::GetInstance().saveSettingData();  //出错不报提示 //TODOTODO 考虑是否和配置页面配置分开储存
}


void BottomWidget::onPlayOrPause()
{
    if(musicPlayer->state() == MusicPlayer::PlayingState)
        pause();
    else
        play();
}

void BottomWidget::onErrorOccurs(int code, QString strErr)
{
    Q_UNUSED(code)
    BesMessageBox::information(tr("提示"),
        tr("播放音频时发生错误，请尝试使用别的音频文件")+ "\n\n" + tr("出错细节:")+ strErr);
}


