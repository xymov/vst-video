#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDesktopWidget>
#include <QTime>
#include <QShortcut>

#define api "https://api.iokzy.com/inc/ldg_seackm3u8s.php"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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


       //关联播放进度条
       connect(ui->sliderProgress,SIGNAL(sliderReleased()),this,SLOT(sliderProgressReleased()));
       connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));

       //视频长度状态发生改变
       connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));

       //表示当前媒体的状态已更改
       connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

       //回车全屏
      connect(new QShortcut(QKeySequence(Qt::Key_Enter),this), SIGNAL(activated()), this, SLOT(on_pushButton_full_clicked()));

      connect(new QShortcut(QKeySequence(16777220),this), SIGNAL(activated()), this, SLOT(on_pushButton_full_clicked()));


      connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()), this, SLOT(on_pushButton_full_clicked()));



      //注册监视对象
       video->installEventFilter(this);



     // connect(video,SIGNAL(moveEvent(QMoveEvent*)),this,SLOT(switchControl(QMouseEvent*)));


       ui->comboBox_id->hide();
       ui->lineEdit_name->setFocus();

}


MainWindow::~MainWindow()
{
    delete ui;
}

//监视对象,处理回车消息
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == video){


    /*处理鼠标消息 */
    if (event->type() == QEvent::MouseMove){

          QMouseEvent  *mouseEvent = static_cast<QMouseEvent *>(event);

          if(mouseEvent->y()>video->height()-ui->box_control->height()){
                ui->box_control->show();
          }else{
               if(isFullScreen())ui->box_control->hide();
          }

        //  qDebug()<<"switchControl:"<<mouseEvent->x()<<","<<mouseEvent->y();


       }else if(event->type() == QEvent::MouseButtonDblClick){

          on_pushButton_full_clicked();

          //  qDebug()<<"switchControl:";
    }




 }
    /*处理按键消息 */
    return QWidget::eventFilter(target, event);
}




//播放器媒体状态被改变
void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:ui->status->setText("状态未知");break;
    case QMediaPlayer::NoMedia:ui->status->setText("没有打开媒体");break;
    case QMediaPlayer::LoadingMedia:ui->status->setText("正在加载...");break;
    case QMediaPlayer::LoadedMedia:ui->status->setText("准备就绪");break;
    case QMediaPlayer::StalledMedia:ui->status->setText("正在缓冲...");break;
    case QMediaPlayer::BufferingMedia:ui->status->setText("正在缓冲...");break;
    case QMediaPlayer::BufferedMedia:ui->status->setText("缓冲完成");break;
    case QMediaPlayer::EndOfMedia:ui->status->setText("媒体播放结束");break;
    case QMediaPlayer::InvalidMedia:ui->status->setText("无法播放当前媒体");break;
    default: break;
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


//播放按钮被单击
void MainWindow::on_pushButton_paly_clicked()
{
    QString play="QPushButton{border-image:url(://rc/play_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/play_on.png) -0px 0px no-repeat;}";
    QString pause="QPushButton{border-image:url(://rc/pause_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/pause_on.png) -0px 0px no-repeat;}";

    if(QMediaPlayer::PlayingState==player->state()){

     player->pause();
     //动态修改样式
     ui->pushButton_paly->setStyleSheet(play);

     ui->status->setText("已暂停");

  }else{

     player->play();

     ui->pushButton_paly->setStyleSheet(pause);

     ui->status->setText("播放中");
  }


   qApp->processEvents();

}
//控制条下集按钮被单击
void MainWindow::on_pushButton_next_clicked()
{
    //player->pause();
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
 if(isFullScreen()){

      ui->box_control->show();
      ui->box_search->show();
      showNormal();


 }else{

         //ui->box_control->hide();
         ui->box_search->hide();
         showFullScreen();
 }

}

//搜索被单击

void MainWindow::on_Button_search_clicked()
{
 if(ui->lineEdit_name->text()!=""){
    ui->comboBox_name->clear();
    ui->comboBox_part->clear();
    ui->textEdit->clear();
    getname(ui->comboBox_name,api,ui->lineEdit_name->text());


 }
}
//影片名被改变
void MainWindow::on_comboBox_name_currentIndexChanged(int index)
{

 getpart(ui->comboBox_part,ui->textEdit,api,ui->comboBox_name->itemData(index).toString());

}

//剧集被改变
void MainWindow::on_comboBox_part_currentIndexChanged(int index)
{
     player->setMedia(QUrl(ui->comboBox_part->itemData(index).toString()));
     on_pushButton_paly_clicked();

}
