#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDesktopWidget>
#include <QTime>

#include <QShortcut>
#include <QtConcurrent>

#define api "https://api.iokzy.com/inc/ldg_seackm3u8s.php"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);
    //关联退出
     connect(this,SIGNAL(quit()),&load,SLOT(hide()));
    //窗口居中
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);
    //初始化播放器
       //动态添加播放控件
       video = new QVideoWidget;
       video->setStyleSheet("background:black;");
       ui->box_video->addWidget(video);
       video->setMouseTracking(true);
       video->show();

       //初始化播放器
        player = new QMediaPlayer;
        player->setVolume(100);
        player->setVideoOutput(video);
        player->setVolume(100);

        playlist = new QMediaPlaylist;
        player->setPlaylist(playlist);
        playlist->playbackModeChanged(QMediaPlaylist::Sequential);



       //关联播放进度条
       connect(ui->sliderProgress,SIGNAL(sliderReleased()),this,SLOT(sliderProgressReleased()));
       connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));

       //视频长度状态发生改变
       connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));

       //表示当前媒体的打开状态已更改
       connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));


       //表示当前媒体的播放状态已更改
       connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(stateChanged(QMediaPlayer::State)));

       //回车全屏
      connect(new QShortcut(QKeySequence(Qt::Key_Enter),this), SIGNAL(activated()), this, SLOT(on_pushButton_full_clicked()));
      connect(new QShortcut(QKeySequence(16777220),this), SIGNAL(activated()), this, SLOT(on_pushButton_full_clicked()));

      //快进快退

        connect(new QShortcut(QKeySequence(Qt::Key_Left),this), SIGNAL(activated()), this, SLOT(decseek()));
        connect(new QShortcut(QKeySequence(Qt::Key_Right),this), SIGNAL(activated()), this, SLOT(addseek()));

     //音量+-

       connect(new QShortcut(QKeySequence(Qt::Key_Up),this), SIGNAL(activated()), this, SLOT(volumeUp()));
       connect(new QShortcut(QKeySequence(Qt::Key_Down),this), SIGNAL(activated()), this, SLOT(volumeDown()));

     //空格 播放/暂停
      connect(new QShortcut(QKeySequence(Qt::Key_Space),this), SIGNAL(activated()), this, SLOT(on_pushButton_paly_clicked()));

      //ESC取消全屏
      connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()), this, SLOT(on_pushButton_full_clicked()));

      //注册监视对象
      video->installEventFilter(this); installEventFilter(this);

     //初始化
       ui->comboBox_id->hide();

      ui->lineEdit_name->setFocus();

      //定时器
       m_timer = new QTimer;
       m_timer->setSingleShot(false);
      // m_timer->start(1000);
       connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TimerTimeOut()
{
    if(isFullScreen()){
         //隐藏鼠标
          video->setCursor(Qt::BlankCursor);
          ui->box_control->hide();
      }else{
          if(m_timer->isActive())m_timer->stop();
    }
}

//监视对象,处理回车消息
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == video){

    /*处理鼠标消息 */
    if (event->type() == QEvent::MouseMove){
         //重启定时器
         m_timer->start(5000);
          if(ui->box_control->isHidden()){
              video->setCursor(Qt::ArrowCursor);
              ui->box_control->show();
          }

        /*
          QMouseEvent  *mouseEvent = static_cast<QMouseEvent *>(event);

          if(mouseEvent->y()>video->height()-ui->box_control->height()){
                ui->box_control->show();
          }else{
               if(isFullScreen())ui->box_control->hide();
          }
*/
            //  qDebug()<<"switchControl:"<<mouseEvent->x()<<","<<mouseEvent->y();

       }else if(event->type() == QEvent::MouseButtonDblClick){

            on_pushButton_full_clicked();

          //  qDebug()<<"switchControl:";


    }else if(event->type() == QEvent::MouseButtonRelease){

        // ui->sliderProgress->setFocus();

    }

  //线程搜索影片列表结束
  }else if(event->type() ==QEvent::User+1){
          ui->comboBox_name->clear();
          ui->comboBox_part->clear();
    for(int i=0;i<vid.size();i++){
        ui->comboBox_name->addItem(vname[i],vid[i]);
    }
      //echoload(false);
      qDebug() << __FUNCTION__  <<"线程结束";

   //线程搜索影片详情结束

}else if(event->type() ==QEvent::User+2){

        ui->comboBox_part->clear();
        playlist->clear();
        for(int i=0;i<vurl.size();i++){
            QStringList list =vurl[i].split("#");
            QStringList video;

            foreach (QString s, list) {
                //第30集$https://index.m3u8$ckm3u8
                 video=s.split("$");
                if(video.size()>1){
                     playlist->addMedia(QUrl(video[1]));
                     ui->comboBox_part->addItem(video[0],video[1]);
                }
            }
        }

        //循坏结束
        echoload(false);
        ui->textEdit->setHtml(vdes);
}
    /*处理按键消息 */
    return QWidget::eventFilter(target, event);
}

void MainWindow::echoload(bool echo){

    if(echo){load.show();}else{emit quit();}
}

//播放器媒体状态被改变
void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:ui->status->setText("状态未知");break;
    case QMediaPlayer::NoMedia:ui->status->setText("没有打开媒体");break;
    case QMediaPlayer::LoadingMedia :ui->status->setText("正在加载...");ui->comboBox_part->setCurrentIndex(playlist->currentIndex());break;
    case QMediaPlayer::LoadedMedia:ui->status->setText("准备就绪");player->play();break;
    case QMediaPlayer::StalledMedia:ui->status->setText("正在缓冲..."); echoload(true);break;
    case QMediaPlayer::BufferingMedia:ui->status->setText("正在缓冲...");break;
    case QMediaPlayer::BufferedMedia:ui->status->setText("正在播放"); echoload(false);;break;
    case QMediaPlayer::EndOfMedia:ui->status->setText("媒体播放结束");break;
    case QMediaPlayer::InvalidMedia:ui->status->setText("无法播放当前媒体");break;
    }
}
//播放器媒体状态被改变
void MainWindow::stateChanged(QMediaPlayer::State state)
{
    QString play="QPushButton{border-image:url(://rc/play_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/play_on.png) -0px 0px no-repeat;}";
    QString pause="QPushButton{border-image:url(://rc/pause_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/pause_on.png) -0px 0px no-repeat;}";

    switch (state) {
     case QMediaPlayer::PlayingState:ui->status->setText("正在播放"); ui->pushButton_paly->setStyleSheet(pause);break;
     case QMediaPlayer::PausedState:ui->status->setText("已暂停");ui->pushButton_paly->setStyleSheet(play);break;
     case QMediaPlayer::StoppedState:ui->status->setText("已停止");ui->pushButton_paly->setStyleSheet(play);break;
    }
}

//播放器视频长度状态发生改变
void MainWindow::durationChange(qint64 playtime)
{
    ui->sliderProgress->setMaximum(playtime);
    QTime t(0,0,0);
    t = t.addMSecs(playtime);
    STimeDuration = t.toString("hh:mm:ss");
}

//播放器进度被改变
void MainWindow::positionChange(qint64 p)
{
    if (!ui->sliderProgress->isSliderDown()) {
     ui->sliderProgress->setValue(p);
     setSTime(p);
    }
}

//进度条被移动
void MainWindow::on_sliderProgress_sliderMoved(int position)
{
   setSTime(position);
}

//进度条鼠标释放滑块
void MainWindow::sliderProgressReleased()
{
    player->setPosition(ui->sliderProgress->value());

}

//设置进度标签
void MainWindow::setSTime(qint64 v)
{
    //时间转换
    QTime t(0,0,0);
    t = t.addMSecs(v);
    QString STimeElapse = t.toString("HH:mm:ss");
    //设置进度标签
    ui->labelTimeVideo->setText(STimeElapse + "/" + STimeDuration);
    //设置提示文本
    ui->sliderProgress->setToolTip(STimeElapse);
}

//音量+-
void MainWindow::volumeUp()
{
        player->setMuted(false);
        player->setVolume(player->volume()+1);
}

void MainWindow::volumeDown()
{
    player->setMuted(false);
    player->setVolume(player->volume()-1);
}

//快退快进
void MainWindow::decseek()
{
    player->setPosition(player->position() - 5000);
}

void MainWindow::addseek()
{
    player->setPosition(player->position() + 5000);
}


//播放按钮被单击
void MainWindow::on_pushButton_paly_clicked()
{
    if(player->state()==QMediaPlayer::PlayingState){

     player->pause();

  }else{

     player->play();

  }
   qApp->processEvents();

}
//控制条下集按钮被单击
void MainWindow::on_pushButton_next_clicked()
{
   playlist->next();
}


//控制条静音按钮被单击
void MainWindow::on_pushButton_sound_clicked()
{
    QString sound="QPushButton{border-image:url(://rc/sound_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/sound_on.png) -0px 0px no-repeat;}";
    QString mute="QPushButton{border-image:url(://rc/mute_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/mute_on.png) -0px 0px no-repeat;}";

    if(player->isMuted()){
        player->setMuted(false);
        ui->pushButton_sound->setStyleSheet(sound);
    }else{
        player->setMuted(true);
        ui->pushButton_sound->setStyleSheet(mute);
    }
}
//控制条全屏按钮被单击
void MainWindow::on_pushButton_full_clicked()

{

    QString full="QPushButton{border-image:url(://rc/full_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/full_on.png) -0px 0px no-repeat;}";
    QString general="QPushButton{border-image:url(://rc/general_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/general_on.png) -0px 0px no-repeat;}";


 if(isFullScreen()){
      ui->box_control->show();
      ui->box_search->show();
      showNormal();
       m_timer->stop();
       video->setCursor(Qt::ArrowCursor);  //显示正常鼠标
       ui->pushButton_full->setStyleSheet(full);

 }else{
         video->setFocus();
         ui->box_control->hide();
         ui->box_search->hide();
         m_timer->start(5000);

         //video->setCursor(Qt::BlankCursor);  //隐藏鼠标
         showFullScreen();
         ui->pushButton_full->setStyleSheet(general);
 }

}
//搜索被单击

void MainWindow::on_Button_search_clicked()
{
    if(ui->lineEdit_name->text()!=""){
      echoload(true);
      QFuture<void> f1 =QtConcurrent::run(this,&MainWindow::ThreadFunc,true,ui->lineEdit_name->text());
      //f1.waitForFinished();
 }
}

void MainWindow::ThreadFunc(bool type,QString word){

    if (word=="")return;
   //获取影片信息
   getvideo(type,api,word);

  //发送线程退出消息
   int user=type?1:2;
   QEvent event (QEvent::Type(QEvent::User+user));
   QApplication::postEvent(this ,new QEvent(event));

   //qDebug() << __FUNCTION__  << QThread::currentThreadId() << QThread::currentThread();
}

//影片名被改变
void MainWindow::on_comboBox_name_currentIndexChanged(int index)
{
    load.show();
    player->stop();
    ui->comboBox_part->clear();
    QtConcurrent::run(this,&MainWindow::ThreadFunc,false,ui->comboBox_name->itemData(index).toString());

}
//剧集被改变
void MainWindow::on_comboBox_part_currentIndexChanged(int index)
{

     playlist->setCurrentIndex(index);
     player->play();
}
