#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);

     //配置设置
     set.sourcePath=QCoreApplication::applicationDirPath()+"/source.txt";

     set.livePath=QCoreApplication::applicationDirPath()+"/live.txt";

     set.cache=QCoreApplication::applicationDirPath()+"/cache/";

     set.nopic="://rc/timg.jpeg";



     //窗口居中
     move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);

     //默认大小
     resize(QSize(800,600));

     //最大化
     setWindowState(Qt::WindowMaximized);


     //程序初始

     init();

     createSource();

}


MainWindow::~MainWindow()
{
    delete ui;
    exit(0);
}


//初始化工作
void MainWindow::init(){


    //检查资源文件
    if(!isFileExist(set.sourcePath)){
                QFile file(set.sourcePath);
                 file.open(QIODevice::ReadWrite);
                 file.write("OK资源,https://api.iokzy.com/inc/ldg_seackm3u8s.php\n最大资源,http://www.zdziyuan.com/inc/s_ldgm3u8_sea.php");
                 file.close();
            }
    //检查缓存目录
     if(!isDirExist(set.cache,true)){
         //QMessageBox::warning(nullptr, "提示", "创建缓存目录(cache)失败!",QMessageBox::Yes);
     }

     //qDebug()<<sourcePath<<cache;

  /*       控件初始化     */


                  //动态添加播放控件
                  video = new QVideoWidget;
                  video->setStyleSheet("background:black;");
                  ui->box_video->addWidget(video);
                  video->setMouseTracking(true);
                  video->setAttribute(Qt::WA_OpaquePaintEvent);

                  video->show();


                  //初始化播放器
                   player = new QMediaPlayer;


                   player->setVideoOutput(video);
                   playlist = new QMediaPlaylist;
                   playlist->playbackModeChanged(QMediaPlaylist::Sequential);   //顺序播放模式
                   player->setVolume(100);                                      //默认音量最大
                   player->setPlaylist(playlist);

                  // video->setUpdatesEnabled(false);

                   //图片列表框

            ui->listWidget->setIconSize(QSize(210,210));//设置图标大小
             ui->listWidget->setGridSize(QSize(240,240));       //设置item大小
            ui->listWidget->setResizeMode(QListView::Adjust);   //自动适应布局
            ui->listWidget->setViewMode(QListView::IconMode);  //大图标模式
            ui->listWidget->setMovement(QListView::Static);      //禁止拖动
            ui->listWidget->setSpacing(10);                    //间距
            ui->listWidget->horizontalScrollBar()->setDisabled(true);  //不显示横向滚动条


             //搜索表格

               //数据模型
                student_model = new QStandardItemModel();
                student_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("更新日期")));
                student_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("资源站")));
                student_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("视频ID")));
                student_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("视频分类")));
                student_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("视频名称")));

                //绑定
                ui->search_list->setModel(student_model);

                //设置表格的各列的宽度值
                 ui->search_list->setColumnWidth(0,200);
                 ui->search_list->setColumnWidth(1,150);
                 ui->search_list->setColumnWidth(2,150);
                 ui->search_list->setColumnWidth(3,150);
                 ui->search_list->setColumnWidth(4,400);


                ui->search_list->horizontalHeader()->setStretchLastSection(true);//自动适应布局
                ui->search_list->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);  //表头信息显示居左
                ui->search_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection); // 只能单选
                ui->search_list->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选中时为整行选中
                ui->search_list->setEditTriggers(QAbstractItemView::NoEditTriggers);     //不可编辑



           //定时器
            m_timer = new QTimer;
            m_timer->setSingleShot(false);
           // m_timer->start(1000);
            connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut()));

           ui->value_Slider->hide();                      //音量调节隐藏
           ui->info_pic->setAlignment(Qt::AlignCenter);  //视频信息图片居中
           ui->page_info->setText("");                   //页数信息默认清空



            /*  各种信号 与 槽    */

                        //关联退出信号
                        connect(this,SIGNAL(quit()),&load,SLOT(close()));

                        //关联播放进度条
                        connect(ui->sliderProgress,SIGNAL(sliderReleased()),this,SLOT(sliderProgressReleased()));
                        connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));

                        //视频长度状态发生改变
                        connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));

                        //表示当前媒体的打开状态已更改
                        connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

                        connect(player,SIGNAL(volumeChanged(int)),this,SLOT(volumeChange(int)));


                        //表示当前媒体的播放状态已更改
                        connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(stateChanged(QMediaPlayer::State)));

                       //注册监视对象
                            this->installEventFilter(this);

                            ui->search_name->installEventFilter(this);

                            video->installEventFilter(this);

                            ui->listWidget->installEventFilter(this);

                            ui->pushButton_sound->installEventFilter(this);

                            ui->value_Slider->installEventFilter(this);

}

//
void  MainWindow::createSource()

{
      ui->tree_source->reset();
      ui->tree_source->setEditTriggers(QAbstractItemView::NoEditTriggers);   //不可编辑
      getclass(set.sourcePath);getlive(set.livePath);
      model = new QStandardItemModel(ui->tree_source);//创建模型
      ui->tree_source->setModel(model);
      model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("资源列表"));

      for ( int i=0; i<type.size(); i++ )

      {

          model->setItem(i,0,new QStandardItem(type.value(i).name));

          if(type.value(i).name!="直播列表"){ui->search_source->addItem(type[i].name);}

           foreach (Nameinfo var,type[i].type)
           {
                 model->item(i)->appendRow(new QStandardItem(var.name));
           }

       }




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

//监视对象
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{

     if(target == video)

   {

      /*处理播放器鼠标移动消息 */

       if (event->type() == QEvent::MouseMove){
         //重启定时器
         m_timer->start(5000);
          if(ui->box_control->isHidden()){
              video->setCursor(Qt::ArrowCursor);
              ui->box_control->show();
          }


      /*处理播放器鼠标双击消息 */

         }else if(event->type() == QEvent::MouseButtonDblClick){

            on_pushButton_full_clicked();
         }


    //处理播放器消息

    }else if(event->type() ==QEvent::KeyPress && ui->tabWidget->currentIndex()==1){

                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

                //回车消息
                if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220) {

                         on_pushButton_full_clicked();

                         return  true;
               //取消消息
                }else if(keyEvent->key() == Qt::Key_Escape){

                    if(isFullScreen()){switchFullScreen(false);return true;}

                 //空格消息
                }else if(keyEvent->key() == Qt::Key_Space){

                          on_pushButton_paly_clicked();

                           return  true;


                 }else if(keyEvent->key() == Qt::Key_Left){

                          decseek();  return  true;

                  }else if(keyEvent->key() == Qt::Key_Left){

                          addseek();  return  true;

                  }else if(keyEvent->key() == Qt::Key_Up){
                       volumeUp();  return  true;

                  }else if(keyEvent->key() == Qt::Key_Down){

                       volumeDown();  return  true;

                }




      //处理搜索回车消息
      }else if(event->type() ==QEvent::KeyPress && (target ==ui->search_name) ){

                           QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

                            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220)
                            {
                                    on_search_ok_clicked();
                                    return true;
                            }

       //处理浏览器或搜索表格回车消息
       }else if(event->type() ==QEvent::KeyPress && (target ==ui->listWidget || target ==ui->search_list)){

                                                 QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

                                                  if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220)
                                                  {
                                                             on_info_play_clicked();
                                                             return true;
                                                   }



   //线程搜索下载图片结束
   }else if(event->type() ==QEvent::User){

        //取关联数据
         QStringList v=ui->comboBox_name->itemData(ui->comboBox_name->currentIndex()).toString().split("|");

         qDebug()<<"图片下载："<<v;

        //设置预览图片

        QString file=set.cache+toHash(v.value(0))+"_"+v.value(1)+".jpg";

        if(!isFileExist(file)){file=set.nopic;}

        QPixmap pixmap(file);

        ui->info_pic->setAlignment(Qt::AlignCenter);  //图片居中

        ui->info_pic->setPixmap(pixmap);


  //线程搜索影片列表结束      
  }else if(event->type() ==QEvent::User+1){
        int row=0;QString word;
        student_model->removeRows(0,student_model->rowCount()); ui->comboBox_name->clear();
        for(int i=0;i<vSearch.size();i++)
        {
            for(int i2=0;i2<vSearch.value(i).id.size();i2++,row++){
             word=vSearch.value(i).api+"|"+vSearch.value(i).id.value(i2);
             ui->comboBox_name->addItem(vSearch.value(i).name.value(i2),word);
             student_model->setItem(row, 0, new QStandardItem(vSearch.value(i).last.value(i2)));
             student_model->setItem(row, 1, new QStandardItem(vSearch.value(i).sname));
             student_model->setItem(row, 2, new QStandardItem(vSearch.value(i).id.value(i2)));
             student_model->setItem(row, 3, new QStandardItem(vSearch.value(i).tname.value(i2)));
             student_model->setItem(row, 4, new QStandardItem(vSearch.value(i).name.value(i2)));
             student_model->item(row,4)->setForeground(QBrush(QColor(255, 0, 0)));
            }
        }

        echoload(false);

   //线程搜索影片详情结束

}else if(event->type() ==QEvent::User+2){

       ui->comboBox_part->clear(); ui->info_des->clear();

        for(int i=0;i<vInfo.video.size();i++){
            QStringList list =vInfo.video.value(i).split("#");
            QStringList v;

            //下载图片

            if(!isFileExist(set.cache+toHash(vInfo.api)+"_"+vInfo.id.value(i)+".jpg"))
            {

               QtConcurrent::run(this,&MainWindow::ThreadFunc,0,vInfo.pic.value(i)+"|"+vInfo.api+"|"+vInfo.id.value(i));

            }

            foreach (QString s, list) {
                //第30集$https://index.m3u8$ckm3u8  
                v=s.trimmed().split("$");
                 if(v.size()==1){
                      ui->comboBox_part->addItem("高清",v.value(0));
                 }else if(v.size()==2){
                     ui->comboBox_part->addItem("高清",v.value(0));
                 }else if(v.size()==3){
                     ui->comboBox_part->addItem(v.value(0),v.value(1));
                 }
            }
            ui->info_des->setHtml(todes(vInfo,i));


        }

       echoload(false);

    //浏览下载图片结束
    }else if(event->type()>(QEvent::User+2)){

        int key=event->type()-QEvent::User-3;

         createListWidget(ui->listWidget,key,false);

       // qDebug()<<key;

      //return true;


        //音量按钮鼠标移动事件
     }else if(target == ui->pushButton_sound){

        if (event->type() == QEvent::HoverEnter) ui->value_Slider->show();

    }else if(event->type() == QEvent::Leave and target == ui->value_Slider){

        ui->value_Slider->hide();


 //图标列表框双击
    }else if(event->type() == QEvent::MouseButtonDblClick and target == ui->listWidget){

       qDebug()<<"图标列表框双击";


  //end
    }


    /*处理按键消息 */
    return QWidget::eventFilter(target, event);
}

void MainWindow::echoload(bool echo){

    if(echo){load.show();}else{load.hide();}
}

//播放器媒体状态被改变
void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:ui->status->setText("状态未知");break;
    case QMediaPlayer::NoMedia:ui->status->setText("没有打开媒体");break;
    case QMediaPlayer::LoadingMedia :ui->status->setText("正在加载...");ui->comboBox_part->setCurrentIndex(playlist->currentIndex());break;
    case QMediaPlayer::LoadedMedia:ui->status->setText("准备就绪");break;
    case QMediaPlayer::StalledMedia:ui->status->setText("正在缓冲..."); if(isFullScreen()){echoload(true);}break;
    case QMediaPlayer::BufferingMedia:ui->status->setText("正在缓冲...");break;
    case QMediaPlayer::BufferedMedia:ui->status->setText("正在播放"); echoload(false);break;
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

    QModelIndex index= ui->tree_source->currentIndex();
    if(index.data().toString()=="直播列表" || index.parent().data().toString()=="直播列表"){
        ui->labelTimeVideo->setText("直播模式");
    }else{
    ui->labelTimeVideo->setText(STimeElapse + "/" + STimeDuration);
    //设置提示文本
    ui->sliderProgress->setToolTip(STimeElapse);

    }
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
  // qApp->processEvents();

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

//切换全屏状态
void  MainWindow::switchFullScreen(bool cfull){



    QString full="QPushButton{border-image:url(://rc/full_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/full_on.png) -0px 0px no-repeat;}";
    QString general="QPushButton{border-image:url(://rc/general_out.png) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://rc/general_on.png) -0px 0px no-repeat;}";

     video->setFocus();

    if(cfull){

         //保存全屏前状态
         set.playlist=!ui->box_source->isHidden();

         set.windowState=this->windowState();


        ui->box_source->hide();
        ui->box_info->hide();


        ui->box_page->hide();
        ui->box_control->hide();
        ui->tabWidget->findChildren<QTabBar*>().at(0)->hide();

         m_timer->start(5000);
         //video->setCursor(Qt::BlankCursor);  //隐藏鼠标
         ui->pushButton_full->setStyleSheet(general);
         ui->tabWidget->setStyleSheet("border:0;");
         showFullScreen();


    }else{

        ui->tabWidget->setStyleSheet("");
        ui->pushButton_full->setStyleSheet(full);
        ui->box_control->show();
        m_timer->stop();
        video->setCursor(Qt::ArrowCursor);  //显示正常鼠标

        showNormal();

        if(set.playlist){
            ui->box_source->show();
            ui->box_info->show();
             ui->box_page->show();
             ui->tabWidget->findChildren<QTabBar*>().at(0)->show();
             setWindowState(set.windowState);
        }


    }


}



//控制条全屏按钮被单击
void MainWindow::on_pushButton_full_clicked()
{


switchFullScreen(!isFullScreen());


}


void MainWindow::ThreadFunc(int tp,QString word){
   QString sname,id,api;int index;QStringList v;
     if (word=="")return;



     //搜索下载图片
     if(tp==0){

         v=word.split("|");

         UrlRequestImg(v.value(0),toHash(v.value(1))+"_"+v.value(2));

         qDebug()<<word;



         QEvent event (QEvent::Type(QEvent::User+0));

         QApplication::postEvent(this ,new QEvent(event));

     }else if(tp==1){



           v=word.split("|");
           search(v.value(0),v.value(1).toInt());
         QEvent event (QEvent::Type(QEvent::User+1));
         QApplication::postEvent(this ,new QEvent(event));


     }else if(tp==2){
         //取关联数据
         index=ui->comboBox_name->currentIndex();
         v=ui->comboBox_name->itemData(index).toString().split("|");
         api=v.value(0);id=v.value(1);
         getvideo(tp,api,id);

         QEvent event (QEvent::Type(QEvent::User+2));
         QApplication::postEvent(this ,new QEvent(event));

     }else if(tp==3){

         UrlRequestImg(vInfo.pic.value(word.toInt()),toHash(vInfo.api)+"_"+vInfo.id.value(word.toInt()));

         QEvent event (QEvent::Type(QEvent::User+tp+word.toInt()));

         QApplication::postEvent(this ,new QEvent(event));


     }

}



//影片名被改变
void MainWindow::on_comboBox_name_currentIndexChanged(int index)
{
 QString sname,id,api,itemData;


    if(index!=-1){
       //取关联数据

        QStringList v=ui->comboBox_name->itemData(index).toString().split("|");
        api=v.value(0);id=v.value(1);

        //检查id是否一致
        //qDebug()<<v;

        if(vInfo.id.value(index).toInt()==id.toInt()){

            ui->info_des->setHtml(todes(vInfo,index));
            loadMedia(index);


        }else{

            QtConcurrent::run(this,&MainWindow::ThreadFunc,2,QString::number(index));


        }


        //设置预览图片

        QString file=set.cache+toHash(api)+"_"+id+".jpg";

        if(!isFileExist(file)){file=set.nopic;}

        QPixmap pixmap(file);ui->info_pic->setPixmap(pixmap);

        ui->info_pic->setAlignment(Qt::AlignCenter);  //图片居中

   }

}
//剧集被改变
void MainWindow::on_comboBox_part_currentIndexChanged(int index)
{
   if(index>0)playlist->setCurrentIndex(index);


}

//切换列表显示
void MainWindow::on_pushButton_playlist_clicked()
{

    if(ui->box_source->isHidden()){
       ui->tabWidget->setStyleSheet("");
        ui->box_control->show();
        ui->box_source->show();
        ui->box_info->show();
        ui->box_page->show();
        ui->tabWidget->findChildren<QTabBar*>().at(0)->show();


    }else{

        ui->box_source->hide();
        ui->box_info->hide();
        ui->box_page->hide();
        ui->tabWidget->findChildren<QTabBar*>().at(0)->hide();
        ui->tabWidget->setStyleSheet("border:0;");
   }
}

void MainWindow::volumeChange(int value){

 ui->value_Slider->setValue(value);

}

void MainWindow::on_value_Slider_valueChanged(int value)
{

    player->setVolume(value);
}





//树形框项目被选择

void MainWindow::on_tree_source_pressed(const QModelIndex &index)
{
        Q_UNUSED(index);

    if(index.data().toString()=="直播列表" || index.parent().data().toString()=="直播列表"){

       int row=index.row();

       ui->tabWidget->setCurrentIndex(1);

       if(index.parent().data().toString()=="直播列表"){ row=index.parent().row();}

       //qDebug()<<playlist->mediaCount()<<row;

       if(playlist->mediaCount()!=type.value(row).type.size())
       {
            playlist->clear();
            foreach(Nameinfo var,type.value(row).type)
            {
                playlist->addMedia(QUrl(var.id));
            }
            player->play();

        }else{

              playlist->setCurrentIndex(index.row());

        }

         video->setUpdatesEnabled(false);

         ui->page_info->setText("频道："+QString::number(index.row()+1)+"/"+QString::number(playlist->mediaCount()));

         ui->labelTimeVideo->setText("直播模式");

         ui->sliderProgress->setEnabled(false);



    }else{
         video->setUpdatesEnabled(true);
          ui->sliderProgress->setEnabled(true);
         getpageinfo(1);
    }

}


void MainWindow::createListWidget(QListWidget *listWidget,int key,bool create=false){

    if(!create && listWidget->count()<=key)return;

    if(key>=vInfo.id.size() || key>=vInfo.name.size())return;


    QString file=set.cache+"/"+toHash(vInfo.api)+"_"+vInfo.id.value(key)+".jpg";

    if(!isFileExist(file)){file=set.nopic;}

    QPixmap pixmap(file); QMutex mutex;

    QListWidgetItem *item = new QListWidgetItem;

    QWidget *widget = new QWidget;
    QVBoxLayout *widgetLayout = new QVBoxLayout;
    QLabel *imageLabel = new QLabel;

    QLabel *txtLabel = new QLabel;
    QString name=vInfo.name[key];
    if(name.size()>30){
        name=name.mid(0, 20)+"...";
       txtLabel->setToolTip(vInfo.name.value(key));
    }

    //QLabel *txtLabel = new QLabel(tr("%1").arg(vInfo.name[key]));

    txtLabel->setText(name);

     widget->setLayout(widgetLayout);
     widgetLayout->setMargin(0);
     widgetLayout->setSpacing(0);
     widgetLayout->addWidget(imageLabel);
     widgetLayout->addWidget(txtLabel);

      //头像裁剪
            if(pixmap.width()>227||pixmap.height()>227)
            {
                pixmap=pixmap.scaled(234,234,Qt::KeepAspectRatio);
            }
            imageLabel->setPixmap(pixmap);

             //imageLabel->setScaledContents(true);  //图片缩放
             imageLabel->setAlignment(Qt::AlignCenter);  //图片居中
            txtLabel->setFixedHeight(60);
            txtLabel->setWordWrap(true);
            txtLabel->setAlignment(Qt::AlignCenter);    //文本居中

            item->setSizeHint(QSize(240,240));

           mutex.lock();

           if(create){

               listWidget->addItem(item);

               listWidget->setSizeIncrement(240,240);       //当用户重新定义窗口尺寸的时候，窗口会以此为基准
               listWidget->setItemWidget(item,widget);

           }else{

                if(listWidget->count()>key){
                   QListWidgetItem *pItem = listWidget->item(key);
                   listWidget->setItemWidget(pItem,widget);
             }

           }

          mutex.unlock();

};



//列表被选择
void MainWindow::on_listWidget_currentRowChanged(int key)
{

     ui->comboBox_name->setCurrentIndex(key);

}


//取分页信息
void MainWindow::getpageinfo (int page){

    int row;QString id,api;

    //取当前选择项
    QModelIndex index=ui->tree_source->currentIndex();if(index.row()<0){return;}

     echoload(true);

    //取选择项目序号

     row=index.parent().row();

     //取关联分类ID
     if(row==-1){
         id="";
         row=index.row();
     }else{

       id=type.value(row).type.value(index.row()).id;
     }

     //取关联api地址
      api=type.value(row).api;

      ui->listWidget->clear(); isDirExist(set.cache,true);

      getvideo(4,api,id,QString::number(page));


       ui->comboBox_name->clear();ui->comboBox_part->clear();

      for (int i=0;i<vInfo.id.size();i++) {

          ui->comboBox_name->addItem(vInfo.name.value(i),vInfo.api+"|"+vInfo.id.value(i));

           QString file=set.cache+toHash(vInfo.api)+"_"+vInfo.id.value(i)+".jpg";

           createListWidget(ui->listWidget,i,true);

         if(!isFileExist(file)){

            QtConcurrent::run(this,&MainWindow::ThreadFunc,3,QString::number(i));

         }

     }

      if(ui->comboBox_name->count()==0){
         //echoload(false);
         //QMessageBox::warning(nullptr, "提示", "未找到任何资源!",QMessageBox::Yes);
      }

    //connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(On_listWidgetItem(QListWidgetItem*)));

      QString str=QString("页数：%1/%3，视频：%2/%4").arg(vInfo.page).arg(vInfo.pagesize).arg(vInfo.pagecount).arg(vInfo.recordcount);

      ui->page_info->setText(str);

      ui->page_edit->setValidator(new QIntValidator(1, vInfo.pagecount.toInt(), this));    //限定输入范围


      echoload(false);
}


//上页
void MainWindow::on_page_front_clicked()
{
    int page=vInfo.page.toInt()-1;if(page<1){return;}
    getpageinfo(page);
}

//下页
void MainWindow::on_page_next_clicked()
{
    int page=vInfo.page.toInt()+1;if(page>vInfo.pagecount.toInt()){return;}
    getpageinfo(page);
}

//跳页
void MainWindow::on_page_jmp_clicked()
{
 getpageinfo(ui->page_edit->text().toInt());
 qDebug()<<vInfo.api<<vInfo.tid.value(0)<<ui->page_edit->text();

}

//根据列表序号加载媒体
void MainWindow::loadMedia(int key){
       ui->comboBox_part->clear();
       QStringList list =vInfo.video.value(key).split("#");
        foreach (QString s, list) {
            //第30集$https://index.m3u8$ckm3u8

             QStringList v=s.trimmed().split("$");
            if(v.size()==1){
                 ui->comboBox_part->addItem("高清",v.value(0));
            }else if(v.size()==2){
                ui->comboBox_part->addItem("高清",v.value(0));
            }else if(v.size()==3){
                ui->comboBox_part->addItem(v.value(0),v.value(1));
            }
        }
}



//加载播放

void  MainWindow:: loadPlay(bool play){

   if(play)
  {


        player->stop();
        playlist->clear();

          for (int i=0;i<ui->comboBox_part->count();i++)
          {

          playlist->addMedia(QUrl(ui->comboBox_part->itemData(i).toString()));

          }

        player->play();

   }else{

        player->pause();
   }
}



//进入播放选项卡并播放视频
void MainWindow::on_info_play_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
    loadPlay(true);
}

//上一个视频
void MainWindow::on_info_front_clicked()
{

        int index= ui->comboBox_name->currentIndex();
        if(index>0){
             player->stop();
             ui->comboBox_name->setCurrentIndex(index-1);
             loadPlay(true);
        }



}

//下一个视频
void MainWindow::on_info_next_clicked()
{


    int index= ui->comboBox_name->currentIndex();

    if(index+1<ui->comboBox_name->count()){
         player->stop();
         ui->comboBox_name->setCurrentIndex(index+1);
         loadPlay(true);
    }




}



void MainWindow::on_search_ok_clicked()
{
    if(ui->search_name->text().trimmed()==""){

       student_model->removeRows(0,student_model->rowCount());

    }else{
       echoload(true);
       QtConcurrent::run(this,&MainWindow::ThreadFunc,1,ui->search_name->text()+"|"+QString::number(ui->search_source->currentIndex()));
    }

}

void MainWindow::on_search_list_pressed(const QModelIndex &index)
{

     echoload(true);
     ui->info_des->clear();
     ui->comboBox_part->clear();
     ui->comboBox_name->setCurrentIndex(index.row());

}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    //浏览
    if(index==0){

         emit show();

         ui->box_page->show();

         if(ui->listWidget->count()!=ui->comboBox_name->count())getpageinfo(1);

    //播放
    }else if(index==1){
        video->setFocus();
        ui->box_page->show();

    //搜索
    }else if(index==2){


        ui->box_page->hide();

        if(student_model->rowCount()!=ui->comboBox_name->count())on_search_ok_clicked();

    }
}

void MainWindow::on_source_re_clicked()
{
    echoload(true);
    createSource();
    echoload(false);

}


void MainWindow::on_search_source_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    on_search_ok_clicked();

}
