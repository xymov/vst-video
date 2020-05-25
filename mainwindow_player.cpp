#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCompleter>
#include <QFileDialog>
#include <QInputDialog>

void MainWindow::initPlayer()
{
    // 动态添加播放控件
    video = new QVideoWidget;
    video->setStyleSheet("background:black;");
    video->setMouseTracking(true);
    //video->setAttribute(Qt::WA_OpaquePaintEvent);
    // video->setUpdatesEnabled(false);

    setVideoMode(Qt::KeepAspectRatio);
    video->show();

    ui->box_player->addWidget(video);
    ui->box_player->addWidget(ui->box_control);

    // 初始化播放器
    player = new QMediaPlayer;
    player->setVideoOutput(video);
    player->setVolume(100);                                    // 默认音量最大
    playlist = new QMediaPlaylist;
    playlist->playbackModeChanged(QMediaPlaylist::Sequential); // 顺序播放模式
    player->setPlaylist(playlist);

    // 关联播放进度条
    connect(ui->sliderProgress, SIGNAL(sliderReleased()), this, SLOT(sliderProgressReleased()));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));

    // 视频长度状态发生改变
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChange(qint64)));

    // 当前媒体的打开状态已更改
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    // 当前媒体的播放状态已更改
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(stateChanged(QMediaPlayer::State)));

    connect(playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(playerlist_currentIndexChanged(int)));

    // 播放器右键菜单
    video->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(video, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(PlayMenu(const QPoint &)));

    video->installEventFilter(this);

    ui->box_video->installEventFilter(this);

    ui->box_control->installEventFilter(this);

    // 控制板隐藏定时器
    m_timer = new QTimer;
    m_timer->setSingleShot(false);
    // m_timer->start(1000);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut()));

    // 音量调节
    _volume_slider = new QSlider(Qt::Vertical, ui->tab_player);
    _volume_slider->setMinimumSize(30, 100);
    _volume_slider->setMaximumSize(30, 100);
    _volume_slider->setMinimum(0);
    _volume_slider->setMaximum(100);
    _volume_slider->setPageStep(2);
    _volume_slider->setValue(100);
    _volume_slider->setTickPosition(QSlider::NoTicks);
    _volume_slider->move(ui->pushButton_paly->width() + ui->pushButton_front->width() + ui->pushButton_next->width() + 15 * 3 + ui->pushButton_sound->width() / 2, ui->box_video->height() + ui->sliderProgress->height());
    _volume_slider->hide();

    // 显示音量的标签
    _volume_value = new QLabel(ui->tab_player);
    _volume_value->setMinimumSize(100, 30);
    _volume_value->setStyleSheet("color:green;");
    _volume_value->hide();

    connect(_volume_slider, SIGNAL(valueChanged(int)), this, SLOT(on_volume_slider_valueChanged(int)));
    connect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChange(int)));

    // 音量显/隐定时器
    _volume_slider_timer = new QTimer;
    _volume_slider_timer->setInterval(3000);
    connect(_volume_slider_timer, SIGNAL(timeout()), this, SLOT(hideVolumeSlider()));

    _volume_slider->installEventFilter(this);

    ui->pushButton_sound->installEventFilter(this);

    ui->label->hide();
    ui->label_VideoName->hide();
    ui->player_control_part->hide();
}

void MainWindow::playlist_currentIndexChanged(int index)
{
    ui->comboBox_part->setCurrentIndex(index);
}

void MainWindow::on_volume_slider_valueChanged(int value)
{
    if(!_volume_slider->isVisible())
        return;

    player->setVolume(value);

    _volume_value->move(ui->pushButton_paly->width() + ui->pushButton_front->width() + ui->pushButton_next->width() + 15 * 3 - 8, _volume_slider->y() - 5 + (100 - value) * 80 / 100);

    if(!_volume_value->isVisible())
        _volume_value->show();

    _volume_value->setText(QString::asprintf("%d", value));

    // 重启隐藏定时器 3s
    _volume_slider_timer->start();
}

void MainWindow::hideVolumeSlider()
{
    _volume_slider->hide();
    _volume_value->hide();

    _volume_slider_timer->stop();
}

void MainWindow::showVolumeSlider()
{
    _volume_slider->show();
    _volume_value->show();
    _volume_slider->setFocus();

    _volume_slider_timer->start();
}

// 播放器媒体状态被改变
void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
    case QMediaPlayer::UnknownMediaStatus:
        ui->status->setText("状态未知");
        show_loading(false);
        break;
    case QMediaPlayer::NoMedia:
        ui->status->setText("无效视频");
        show_loading(false);
        break;
    case QMediaPlayer::LoadingMedia:
        ui->status->setText("正在加载");
        ui->sliderProgress->setEnabled(false);
        if (isFullScreen())
        {
            show_loading(true);
        }
        ui->comboBox_part->setCurrentIndex(playlist->currentIndex());
        qDebug() << "openUrl：" << player->currentMedia().canonicalUrl().toString();
        break;
    case QMediaPlayer::LoadedMedia:
        ui->status->setText("准备就绪");
        show_loading(false);
        break;
    case QMediaPlayer::StalledMedia:
        ui->status->setText("正在缓冲");
        if (isFullScreen())
        {
            show_loading(true);
        }
        break;
    case QMediaPlayer::BufferingMedia:
        ui->status->setText("正在缓冲");
        break;
    case QMediaPlayer::BufferedMedia:
        ui->status->setText("正在播放");
        show_loading(false);
        break;
    case QMediaPlayer::EndOfMedia:
        ui->status->setText("播放结束");
        show_loading(false);
        break;
    case QMediaPlayer::InvalidMedia:
        ui->status->setText("加载失败");
        show_loading(false);
        break;
    }
}

// 播放器媒体状态被改变
void MainWindow::stateChanged(QMediaPlayer::State state)
{
    QString play = "QPushButton{border-image:url(://resource/img/play_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/play_on.svg) -0px 0px no-repeat;}";
    QString pause = "QPushButton{border-image:url(://resource/img/pause_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/pause_on.svg) -0px 0px no-repeat;}";

    switch (state)
    {
    case QMediaPlayer::PlayingState:
        ui->status->setText("正在播放");
        ui->pushButton_paly->setStyleSheet(pause);
        break;
    case QMediaPlayer::PausedState:
        ui->status->setText("播放暂停");
        ui->pushButton_paly->setStyleSheet(play);
        break;
    case QMediaPlayer::StoppedState:
        ui->status->setText("播放停止");
        ui->pushButton_paly->setStyleSheet(play);
        break;
    }
}

// 播放器视频长度状态发生改变
void MainWindow::durationChange(qint64 playtime)
{
    app.live = false;
    ui->sliderProgress->setMaximum(static_cast<int>(playtime));
    QTime t(0, 0, 0);
    t = t.addMSecs(static_cast<int>(playtime));
    STimeDuration = t.toString("hh:mm:ss");
    ui->sliderProgress->setEnabled(true);
}

// 播放器进度改变
void MainWindow::positionChange(qint64 p)
{
    if (app.live)
        return;

    if (!ui->sliderProgress->isSliderDown())
    {
        ui->sliderProgress->setValue(static_cast<int>(p));
        setSTime(p);
    }

    // 播放记录
    MinWriteNotes(ui->comboBox_part->currentIndex());
}

// 进度条被移动
void MainWindow::on_sliderProgress_sliderMoved(int position)
{
    setSTime(position);
}

// 进度条鼠标释放滑块
void MainWindow::sliderProgressReleased()
{
    player->setPosition(ui->sliderProgress->value());
}

// 设置进度标签
void MainWindow::setSTime(qint64 v)
{
    if (app.live)
        return;

    // 时间转换
    QTime t(0, 0, 0);
    t = t.addMSecs(static_cast<int>(v));
    QString STimeElapse = t.toString("HH:mm:ss");

    // 设置进度标签
    ui->labelTimeVideo->setText(STimeElapse + "/" + STimeDuration);

    // 设置进度标签
    // ui->labelTimeDuration->setText(STimeDuration+"/"+);

    // 设置提示文本
    ui->sliderProgress->setToolTip(STimeElapse);
}

// 音量+-
void MainWindow::volumeUp()
{
    player->setMuted(false);
    player->setVolume(player->volume() + 1);

    showVolumeSlider();

    _volume_slider->setValue(player->volume());
}

void MainWindow::volumeDown()
{
    player->setMuted(false);
    player->setVolume(player->volume() - 1);

    showVolumeSlider();

    _volume_slider->setValue(player->volume());
}

// 快退快进
void MainWindow::decseek()
{
    if (app.live)
        return;

    player->setPosition(player->position() - 5000);
}

void MainWindow::addseek()
{
    if (app.live)
        return;

    player->setPosition(player->position() + 5000);
}

// 播放按钮被单击
void MainWindow::on_pushButton_paly_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
    }
    else
    {
        player->play();
    }
}

void MainWindow::on_pushButton_front_clicked()
{
    if (playlist->currentIndex() == 0)
    {
        on_info_front_clicked();
    }
    else
    {
        playlist->previous();
    }
}

// 控制条下集按钮被单击
void MainWindow::on_pushButton_next_clicked()
{
    if (playlist->currentIndex() + 1 == playlist->mediaCount())
    {
        on_info_next_clicked();
    }
    else
    {
        playlist->next();
        ui->player_control_part->setCurrentIndex(playlist->currentIndex());

        if(!app.live)
            ui->comboBox_part->setCurrentIndex(playlist->currentIndex());
    }
}

// 控制条静音按钮被单击
void MainWindow::on_pushButton_sound_clicked()
{
    QString sound = "QPushButton{border-image:url(://resource/img/volume-up_out.svg) 0px 0px no-repeat;}QPushButton:hover{://resource/img/volume-up_on.svg) -0px 0px no-repeat;}";
    QString mute = "QPushButton{border-image:url(://resource/img/volume-off_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/volume-off_on.svg) -0px 0px no-repeat;}";

    if (player->isMuted())
    {
        player->setMuted(false);
        ui->pushButton_sound->setStyleSheet(sound);
    }
    else
    {
        player->setMuted(true);
        ui->pushButton_sound->setStyleSheet(mute);
    }
}

// 切换全屏状态
void MainWindow::switchFullScreen(bool cfull)
{
    QString full = "QPushButton{border-image:url(://resource/img/full_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/full_on.svg) -0px 0px no-repeat;}";
    QString general = "QPushButton{border-image:url(://resource/img/general_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/general_on.svg) -0px 0px no-repeat;}";

    video->setFocus();

    if (cfull)
    {
        // 保存全屏前状态
        app.playlist = !ui->box_source->isHidden();
        app.windowState = this->windowState();
        ui->box_source->hide();
        //ui->box_info->hide();
        //ui->box_page->hide();
        ui->box_control->hide();
        ui->tabWidget->findChildren<QTabBar *>().at(0)->hide();
        m_timer->start(2000);
        //video->setCursor(Qt::BlankCursor);  // 隐藏鼠标
        ui->pushButton_full->setStyleSheet(general);
        ui->tabWidget->setStyleSheet("border:none;");
        ui->titlebar->hide();
        showFullScreen();
    }
    else
    {
        ui->tabWidget->setStyleSheet(app.playlist ? "" : "border:none;");
        ui->pushButton_full->setStyleSheet(full);
        ui->box_control->show();
        m_timer->stop();
        video->setCursor(Qt::ArrowCursor); // 显示正常鼠标
        showNormal();

        ui->titlebar->show();
        if (app.playlist)
        {
            ui->box_source->show();
            //ui->box_info->show();
            //ui->box_page->show();
            ui->tabWidget->findChildren<QTabBar *>().at(0)->show();
            setWindowState(app.windowState);
        }
    }
}

// 控制条全屏按钮被单击
void MainWindow::on_pushButton_full_clicked()
{
    switchFullScreen(!isFullScreen());
}

// 切换列表显示
void MainWindow::on_pushButton_playlist_clicked()
{
    if (ui->box_source->isHidden())
    {
        ui->tabWidget->setStyleSheet("");
        ui->box_control->show();
        ui->box_source->show();
        //ui->box_info->show();
        //ui->box_page->show();
        ui->tabWidget->findChildren<QTabBar *>().at(0)->show();
        // 取消置顶
        //hide();setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);show();
    }
    else
    {
        ui->box_source->hide();
        //ui->box_info->hide();
        //ui->box_page->hide();
        ui->tabWidget->findChildren<QTabBar *>().at(0)->hide();
        ui->tabWidget->setStyleSheet("border:0;");
        // 窗口置顶
        //hide();setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);show();

        m_timer->start(3000);
    }
}

void MainWindow::volumeChange(int value)
{
    _volume_slider->setValue(value);
}

void MainWindow::on_value_Slider_valueChanged(int value)
{
    player->setVolume(value);
}

// 打开 本地
void MainWindow::on_action_open_triggered()
{
    // 选择多个文件
    QString curPath = QDir::currentPath();    // 获取系统当前目录
    QString dlgTitle = "选择多个文件";    // 对话框标题
    QString filter = "视频文件(*.mp4 *.avi *.wmv *.mpg *.mpeg *.m3u8 *.rmvb *.3gp *.mov *.swf *.flv);;音频文件(*.mp3 *. *.wav *.flac *.ogg *.ape *.cda *.mid *.voc);;所有文件(*.*)"; // 文件过滤器
    QStringList fileList = QFileDialog::getOpenFileNames(this, dlgTitle, curPath, filter);
    if (fileList.count() == 0)
        return;

    playlist->clear();

    for (int i = 0; i < fileList.count(); i++)
    {
        playlist->addMedia(QUrl::fromLocalFile(fileList.value(i)));
    }

    player->play();
}

void MainWindow::on_action_openurl_triggered()
{
    QString dlgTitle = "打开URL";
    QString txtLabel = "请输入URL地址";
    QString defaultInput = "";
    QLineEdit::EchoMode echoMode = QLineEdit::Normal; // 正常文字输入
    //QLineEdit::EchoMode echoMode=QLineEdit::Password;// 密码输入
    bool ok = false;

    QString text = QInputDialog::getText(this, dlgTitle, txtLabel, echoMode, defaultInput, &ok);

    if (ok && !text.isEmpty() && !text.isEmpty())
    {
        player->setMedia(QUrl(text));
        player->play();
    }
}

// 亮度+-
void MainWindow::on_action_brightness_add_triggered()
{
    video->setBrightness(video->brightness() + 10);
}

void MainWindow::on_action_brightness_sub_triggered()
{
    video->setBrightness(video->brightness() - 10);
}

// 对比度 +-
void MainWindow::on_action_contrast_add_triggered()
{
    video->setContrast(video->contrast() + 10);
}

void MainWindow::on_action_contrast_sub_triggered()
{
    video->setContrast(video->contrast() - 10);
}

// 饱和度
void MainWindow::on_action_Saturation_add_triggered()
{
    video->setSaturation(video->saturation() + 10);
}

void MainWindow::on_action_Saturation_sub_triggered()
{
    video->setSaturation(video->saturation() - 10);
}

// 视频缩放处理
void MainWindow::on_action_videosize_IgnoreAspectRatio_triggered()
{
    setVideoMode(Qt::IgnoreAspectRatio);
}

void MainWindow::on_action_videosize_KeepAspectRatio_triggered()
{
    setVideoMode(Qt::KeepAspectRatio);
}

void MainWindow::on_action_videosize_KeepAspectRatioByExpanding_triggered()
{
    setVideoMode(Qt::KeepAspectRatioByExpanding);
}

void MainWindow::setVideoMode(Qt::AspectRatioMode mode)
{
    switch (mode)
    {
    case Qt::IgnoreAspectRatio: // 铺满
        ui->action_videosize_IgnoreAspectRatio->setChecked(true);
        ui->action_videosize_KeepAspectRatio->setChecked(false);
        ui->action_videosize_KeepAspectRatioByExpanding->setChecked(false);
        break;
    case Qt::KeepAspectRatio: // 缩放
        ui->action_videosize_IgnoreAspectRatio->setChecked(false);
        ui->action_videosize_KeepAspectRatio->setChecked(true);
        ui->action_videosize_KeepAspectRatioByExpanding->setChecked(false);
        break;
    case Qt::KeepAspectRatioByExpanding: // 拉伸
        ui->action_videosize_IgnoreAspectRatio->setChecked(false);
        ui->action_videosize_KeepAspectRatio->setChecked(false);
        ui->action_videosize_KeepAspectRatioByExpanding->setChecked(true);
        break;
    }

    video->setAspectRatioMode(mode);
}

// 播放器弹出菜单
void MainWindow::PlayMenu(const QPoint &pos)
{
    Q_UNUSED(pos);
    ui->menu_play->exec(QCursor::pos());
}

// 设置
void MainWindow::on_pushButton_setting_clicked()
{
    ui->menu_play->exec(QCursor::pos());
}

void MainWindow::TimerTimeOut()
{
    // if(isFullScreen()){

    if (player->state() == QMediaPlayer::PlayingState && ui->box_source->isHidden())
    {
        // 隐藏鼠标
        video->setCursor(Qt::BlankCursor);
        ui->box_control->hide();
        ui->titlebar->hide();
    }
    else
    {
        if (m_timer->isActive())
            m_timer->stop();
    }
}

void MainWindow::on_player_control_part_currentIndexChanged(int index)
{
     ui->comboBox_part->setCurrentIndex(index);
}

void MainWindow::on_label_VideoName_linkActivated(const QString &link)
{
    Q_UNUSED(link)

    ui->tabWidget->setCurrentWidget(ui->tab_detail);
}

