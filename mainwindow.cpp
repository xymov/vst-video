#include "mainwindow.h"
#include "ui_mainwindow.h"
#define  nopic "://rc/timg.jpeg"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);

     //窗口居中
     move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);

     //默认大小
     resize(QSize(800,600));

     //最大化
     setWindowState(Qt::WindowMaximized);

     //程序初始

    init();

     createSource(sourcePath);

}


MainWindow::~MainWindow()
{
    delete ui;
}




//初始化工作
void MainWindow::init(){

    sourcePath=QCoreApplication::applicationDirPath()+"/source.txt";

    cache=QCoreApplication::applicationDirPath()+"/cache";

    //检查资源文件
    if(!isFileExist(sourcePath)){
                QFile file(sourcePath);
                 file.open(QIODevice::ReadWrite);
                 file.write("OK资源,https://api.iokzy.com/inc/ldg_seackm3u8s.php\n最大资源,http://www.zdziyuan.com/inc/s_ldgm3u8_sea.php");
                 file.close();
            }
    //检查缓存目录
     if(!isDirExist(cache,true)){
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

           ui->value_Slider->hide();                  //音量调节隐藏
           //ui->lineEdit_name->setFocus();         //搜索框获得焦点

           ui->info_pic->setAlignment(Qt::AlignCenter);  //视频信息图片居中

           ui->page_info->setText("");





            /*  各种信号 与 槽    */

                        //关联退出信号
                        connect(this,SIGNAL(quit()),&load,SLOT(quit()));


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

                            video->installEventFilter(this); this->installEventFilter(this);

                            ui->listWidget->installEventFilter(this);

                            ui->pushButton_sound->installEventFilter(this);

                            ui->value_Slider->installEventFilter(this);

}

//
void  MainWindow::createSource(QString sourcePath){

      ui->tree_source->reset();
      getclass(sourcePath);
      model = new QStandardItemModel(ui->tree_source);//创建模型
      ui->tree_source->setModel(model);
      model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("资源列表"));
      QMap<QString,SourceInfo>::iterator it; //遍历map
      int i=0;
      for ( it = type.begin(); it != type.end(); ++it,++i ) {
         ui->search_source->addItem(it->name,it->api);
          model->setItem(i,0,new QStandardItem(it.key()));
           foreach (Nameinfo var,it.value().type) {
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
    if(target == video){

    /*处理鼠标移动消息 */

    if (event->type() == QEvent::MouseMove){
         //重启定时器
         m_timer->start(5000);
          if(ui->box_control->isHidden()){
              video->setCursor(Qt::ArrowCursor);
              ui->box_control->show();
          }

   /*处理鼠标双击消息 */

       }else if(event->type() == QEvent::MouseButtonDblClick){

            on_pushButton_full_clicked();

          //  qDebug()<<"switchControl:";

  /*处理鼠标离开消息 */
    }else if(event->type() == QEvent::MouseButtonRelease){

        // ui->sliderProgress->setFocus();

    }

  //线程搜索影片列表结束
  }else if(event->type() ==QEvent::User+1){
        int row=0;
        student_model->removeRows(0,student_model->rowCount()); ui->comboBox_name->clear();
        for(int i=0;i<vSearch.size();i++){
            for(int i2=0;i2<vSearch.value(i).id.size();i2++,row++){
             ui->comboBox_name->addItem(vSearch.value(i).name.value(i2),vSearch.value(i).id.value(i2));
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

       // ui->edit_des->clear();

        playlist->clear();

        for(int i=0;i<vInfo.video.size();i++){
            QStringList list =vInfo.video.value(i).split("#");
            QStringList video;

            foreach (QString s, list) {
                //第30集$https://index.m3u8$ckm3u8
                 video=s.split("$");
                if(video.size()>1){
                     playlist->addMedia(QUrl(video[1]));
                     ui->comboBox_part->addItem(video[0],video[1]);

                }else if(video.size()==1){
                    playlist->addMedia(QUrl(s));
                    ui->comboBox_part->addItem("高清",s);

                }
            }
            ui->info_des->setHtml(todes(vInfo,i));

            //QtConcurrent::run(this,&MainWindow::ThreadFunc,1,vSearch.id.value(0));


        }

        //循坏结束
        echoload(false);
        //ui->edit_des ->setHtml(vdes);


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
    case QMediaPlayer::StalledMedia:ui->status->setText("正在缓冲..."); echoload(true);break;
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
       showNormal();
       ui->box_control->show();
       m_timer->stop();
       video->setCursor(Qt::ArrowCursor);  //显示正常鼠标
       ui->pushButton_full->setStyleSheet(full);
      // ui->tabWidget->setStyleSheet("");


        on_pushButton_playlist_clicked();
        on_pushButton_playlist_clicked();


 }else{
        video->setFocus();
        ui->box_control->hide();
        ui->box_source->hide();
        ui->box_info->hide();
        ui->box_page->hide();
        ui->tabWidget->findChildren<QTabBar*>().at(0)->hide();

 //ui->tabWidget->set

         m_timer->start(5000);
         //video->setCursor(Qt::BlankCursor);  //隐藏鼠标
         ui->pushButton_full->setStyleSheet(general);
         ui->tabWidget->setStyleSheet("border:0;");
         showFullScreen();

 }


}
//搜索被单击

void MainWindow::on_Button_search_clicked()
{


      //QtConcurrent::run(this,&MainWindow::ThreadFunc,1,ui->lineEdit_name->text());

      //QFuture<void> f1 ; f1.waitForFinished();

}

void MainWindow::ThreadFunc(int tp,QString word){
   QString sname,id,api;
     if (word=="")return;
      QEvent event (QEvent::Type(QEvent::User+tp));

    switch(tp){
      case 1:

         search(word);

         QApplication::postEvent(this ,new QEvent(event));
         break;

      case 2:
          sname=student_model->item(word.toInt(),1)->text();
          id=student_model->item(word.toInt(),2)->text();
          api=type.value(sname).api;
          getvideo(tp,api,id);

          QApplication::postEvent(this ,new QEvent(event));

          break;

    case 3:

    UrlRequestImg(vInfo.pic.value(word.toInt()),toHash(vInfo.api)+"_"+vInfo.id.value(word.toInt()));

    QEvent event (QEvent::Type(QEvent::User+tp+word.toInt()));

    QApplication::postEvent(this ,new QEvent(event));

    break;



    }


}



//影片名被改变
void MainWindow::on_comboBox_name_currentIndexChanged(int index)
{

    on_listWidget_currentRowChanged(index);
    //load.show();
    //player->stop();
    //ui->comboBox_part->clear();
   // QtConcurrent::run(this,&MainWindow::ThreadFunc,2,ui->comboBox_name->itemData(index).toString());

}
//剧集被改变
void MainWindow::on_comboBox_part_currentIndexChanged(int index)
{

     playlist->setCurrentIndex(index);
     player->play();
}

//切换列表显示
void MainWindow::on_pushButton_playlist_clicked()
{

    if(ui->box_source->isHidden()){

        ui->box_control->show();
        ui->box_source->show();
        ui->box_info->show();
        ui->box_page->show();
        ui->tabWidget->findChildren<QTabBar*>().at(0)->show();
        ui->tabWidget->setStyleSheet("");

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
       getpageinfo(1);

}


void MainWindow::createListWidget(QListWidget *listWidget,int key,bool create=false){

    if(!create && listWidget->count()<=key)return;

    if(key>=vInfo.id.size() || key>=vInfo.name.size())return;


    QString file=cache+"/"+toHash(vInfo.api)+"_"+vInfo.id.value(key)+".jpg";

    if(!isFileExist(file)){file=nopic;}

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
 //QString file=vInfo.id.value(200);

 if(key!=-1){

     QString file="./cache/"+toHash(vInfo.api)+"_"+vInfo.id.value(key)+".jpg";
     if(!isFileExist(file)){file=nopic;}

     QPixmap pixmap(file);ui->info_pic->setPixmap(pixmap);

     ui->info_pic->setAlignment(Qt::AlignCenter);  //图片居中

      ui->info_des->setHtml(todes(vInfo,key));

     loadMedia(key);


}

   // qDebug()<<currentRow;

}


//取分页信息
void MainWindow::getpageinfo (int page){


     QString id,row,api;

     //取当前选择项
     QModelIndex index=ui->tree_source->currentIndex();

     if(index.row()<0){return;}

    //取选择项目文本
     row=index.parent().data().toString().trimmed();

     //取关联分类ID
     if(row==""){
         id="";
         row=index.data().toString().trimmed();
     }else{

         id=type.value(row).type.value(index.row()).id;
     }

     //取关联api地址
     api=type.value(row).api;

      ui->listWidget->clear(); isDirExist(cache,true);

      getvideo(4,api,id,QString::number(page));


       ui->comboBox_name->clear();ui->comboBox_part->clear();

      for (int i=0;i<vInfo.id.size();i++) {
        //qDebug()<<i;

        ui->comboBox_name->addItem(vInfo.name.value(i),vInfo.id.value(i));

         QString file="./cache/"+toHash(vInfo.api)+"_"+vInfo.id.value(i)+".jpg";

         createListWidget(ui->listWidget,i,true);

         if(!isFileExist(file)){

           QtConcurrent::run(this,&MainWindow::ThreadFunc,3,QString::number(i));

         }

     }

      if(ui->comboBox_name->count()==0){
         //echoload(false);
         QMessageBox::warning(nullptr, "提示", "未找到任何资源!",QMessageBox::Yes);
      }

    //connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(On_listWidgetItem(QListWidgetItem*)));

      QString str=QString("页数：%1/%3，视频：%2/%4").arg(vInfo.page).arg(vInfo.pagesize).arg(vInfo.pagecount).arg(vInfo.recordcount);

      ui->page_info->setText(str);

      ui->page_edit->setValidator(new QIntValidator(1, vInfo.pagecount.toInt(), this));    //限定输入范围
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
       playlist->clear();ui->comboBox_part->clear();
       QStringList list =vInfo.video.value(key).split("#");
        foreach (QString s, list) {
            //第30集$https://index.m3u8$ckm3u8
             QStringList v=s.split("$");
            if(v.size()>1){
                 playlist->addMedia(QUrl(v.value(1)));
                 ui->comboBox_part->addItem(v.value(0),v.value(1));
            }else if(v.size()==1){
                playlist->addMedia(QUrl(s));
                ui->comboBox_part->addItem("高清",s);
            }
        }
}


//进入播放选项卡并播放视频
void MainWindow::on_info_play_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
     player->play();
}

//上一个视频
void MainWindow::on_info_front_clicked()
{
    int index= ui->comboBox_name->currentIndex();
    if(index>=0){
         ui->comboBox_name->setCurrentIndex(index-1);
    }

}

//下一个视频
void MainWindow::on_info_next_clicked()
{
    int index= ui->comboBox_name->currentIndex();

    if(index+1<ui->comboBox_name->count()){

         ui->comboBox_name->setCurrentIndex(index+1);
    }
}

void MainWindow::on_search_ok_clicked()
{

    if(ui->search_name->text()!=""){
       echoload(true);
       QtConcurrent::run(this,&MainWindow::ThreadFunc,1,ui->search_name->text());
    }

}

void MainWindow::on_search_list_pressed(const QModelIndex &index)
{

    echoload(true);
    ui->comboBox_name->setCurrentIndex(index.row());
    QtConcurrent::run(this,&MainWindow::ThreadFunc,2,QString::number(index.row()));


}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    //浏览
    if(index==0){
         ui->box_page->show();

         if(ui->listWidget->count()!=ui->comboBox_name->count())getpageinfo(1);

    //播放
    }else if(index==1){

        ui->box_page->show();

    //搜索
    }else if(index==2){

        ui->box_page->hide();

        if(student_model->rowCount()!=ui->comboBox_name->count())on_search_ok_clicked();

    }
}
