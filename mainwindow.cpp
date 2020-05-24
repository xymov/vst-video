#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);

     //窗口居中
     move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);



    /* 圆角矩形
     QDialog * Parent=new QDialog();
     Parent->setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint |Parent->windowFlags());
     Parent->setAttribute(Qt::WA_TranslucentBackground);   //背景透明
     this->setParent(Parent);
     this->setStyleSheet("background-color:gray;border-radius:10px;");
   */

     QDir::setCurrent("./");  //为了兼容单文件打包工具appimage，使用QDir::currentPath(),不然会找不到配置文件
     //配置设置
     app.sourcePath=QDir::currentPath()+"/source.txt";
     app.livePath=QDir::currentPath()+"/live.txt";
     app.notes=QDir::currentPath()+"/notes.txt";
     app.cache=QDir::currentPath()+"/cache/";
     app.nopic="://resource/img/timg.jpeg";

     app.live=false;

     //备份原始窗口参数
     app.Flags=windowFlags();

     qDebug()<<app.cache;



     //程序初始
     init();
     //检查命令行参数
     getCommond();


     //关闭窗口退出应用 相当于Close信号关联Delete信号
     this->setAttribute(Qt::WA_DeleteOnClose,true);



   /*  自定义标题栏    */
        //setWindowFlags(Qt::FramelessWindowHint |windowFlags());


   /*  检查配置   */

     //是否置顶

     if(config.get("set","tophint").toInt()==1){
         setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint|windowFlags());
         ui->action_tophint->setChecked(true);
     }else{
         setWindowFlags(Qt::FramelessWindowHint |windowFlags());
         //ui->action_tophint->setChecked(false);
     }

      //切换主题

       switchtheme(config.get("set","theme").toInt(),false);

         FramelessHelper *pHelper = new FramelessHelper(this);
         pHelper->activateOn(this);  //激活当前窗体
         pHelper->setTitleHeight(50);  //设置窗体的标题栏高度，可拖动高度
         pHelper->setWidgetMovable(true);  //设置窗体可移动
         pHelper->setWidgetResizable(true);  //设置窗体可缩放
         pHelper->setOnlyTitleBarMove(true); //设置是否只标题栏可拖动
         //pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动
         //pHelper->setRubberBandOnResize(true);  //设置橡皮筋效果-可缩放
            //关闭按钮
             ui->pushButton_close->setFixedSize(60, 40);
             //ui->pushButton_close->setIcon(QIcon("://resource/img/close_out.svg"));
             ui->pushButton_close->setIconSize(QSize(14,14));

             //最大化按钮
             ui->pushButton_max->setFixedSize(60, 40);
              //ui->pushButton_max->setIcon(QIcon("://resource/img/normal-size_out.svg"));
               ui->pushButton_max->setIconSize(QSize(16,16));

              //最小化按钮
              ui->pushButton_mini->setFixedSize(60, 40);
               //ui->pushButton_mini->setIcon(QIcon("://resource/img/minimize.svg"));
               ui->pushButton_mini->setIconSize(QSize(16,16));

              //设置按钮
               ui->pushButton_seting->setFixedSize(60, 40);
               //ui->pushButton_seting->setIcon(QIcon("://resource/img/menu.svg"));
               ui->pushButton_seting->setIconSize(QSize(16,16));

  }

//Delete信号
MainWindow::~MainWindow()
{
    seting.close();
    delete video;
    delete player;
    delete ui;
    qDebug()<<"App quit success!";
    exit(0);
}


//检查命令行

void MainWindow:: getCommond(){



    app.arguments=QCoreApplication::arguments();

    QString filename ;

    if(app.arguments.count()>1){

        playlist->clear();
        for (int i=1; i<app.arguments.count();i++)
         {
             playlist->addMedia(QUrl(app.arguments.value(i)));
        }

        ui->box_explore->setCurrentIndex(1);
        on_pushButton_playlist_clicked();
        player->play();

    }else{
        setWindowState(Qt::WindowMaximized);

        //setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog |Qt::WindowStaysOnTopHint);  //无边框，置顶
        on_action_resource_triggered();
    }
}

//初始化工作
void MainWindow::init(){

    //检查资源文件
    if(!isFileExist(app.sourcePath)){
        QFile soucre("://resource/source/source.txt");soucre.copy(app.sourcePath);
         QFile(app.sourcePath).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }

    if(!isFileExist(app.livePath)){
        QFile soucre("://resource/source/live.txt");soucre.copy(app.livePath);
        QFile(app.livePath).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
      }

    //检查缓存目录
     if(!isDirExist(app.cache,true)){
         QMessageBox::warning(nullptr, "提示", "创建缓存目录cache失败，图片显示可能异常，请手工建立!",QMessageBox::Yes);
     }
  /*       控件初始化     */

          //等待动画
             createLoading();
          //播放记录
                renotes();
                connect(ui->menu_notes,SIGNAL(triggered(QAction*)),this,SLOT(menu_action_notes_triggered(QAction*)));


                   //视频设备
                    GVI = new QGraphicsVideoItem();
                    GVI->setAspectRatioMode(Qt::KeepAspectRatio);
                    // 缩放 Qt::KeepAspectRatio,
                    // 铺满 Qt::IgnoreAspectRatio ，
                    // 拉伸 Qt::KeepAspectRatioByExpanding

                   //文字设备
                    GTI = new QGraphicsTextItem;
                    GTI->setDefaultTextColor(Qt::white);
                    QFont font = this->font();
                    font.setPixelSize(20);
                    GTI->setFont(font);

                    //添加进场景
                    scene = new QGraphicsScene();
                    scene->setBackgroundBrush(Qt::black);
                    scene->addItem(GVI);
                    scene->addItem(GTI);


                    // 绑定场景
                     // ui->view->fitInView(GVI,Qt::KeepAspectRatio);
                      ui->view->setScene(scene);
                      ui->view->horizontalScrollBar()->setDisabled(true);  //不显示横向滚动条
                      ui->view->verticalScrollBar()->setDisabled(true);   //不显示纵向滚动条
                      ui->view->setRenderHint(QPainter::SmoothPixmapTransform);  //平滑



                     //获取场景内部viewport区域,
                      viewWidget = ui->view->viewport();
                     //监视鼠标消息
                      viewWidget->installEventFilter(this);


                 /*
                  //动态添加播放控件
                  video = new QVideoWidget;
                  video->setStyleSheet("background:black;");
                  ui->box_player->addWidget(video);
                  ui->box_player->addWidget(ui->box_control);
                  video->setMouseTracking(true);
                  //video->setAttribute(Qt::WA_OpaquePaintEvent);

                */

                 //标题栏菜单关联
                  ui->titlebar->setContextMenuPolicy(Qt::CustomContextMenu); //鼠标右键点击控件时会发送一个customContextMenuRequested信号
                   connect(ui->titlebar,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(TitlebarMenu(const QPoint&)));

                  //播放器右键菜单关联
                  ui->view->setContextMenuPolicy(Qt::CustomContextMenu); //鼠标右键点击控件时会发送一个customContextMenuRequested信号
                  connect(ui->view,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(PlayMenu(const QPoint&)));
                  //浏览器右键菜单管理
                  ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
                  connect(ui->listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(ExploreMenu(const QPoint&)));

                  //搜索右键菜单管理
                  ui->search_list->setContextMenuPolicy(Qt::CustomContextMenu);
                  connect(ui->search_list,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(ExploreMenu(const QPoint&)));

                  //video->show();
                  // 缩放 Qt::KeepAspectRatio,铺满 Qt::IgnoreAspectRatio ，拉伸 Qt::KeepAspectRatioByExpanding  不缩放  Default
                  //setVideoMode(Qt::KeepAspectRatio);



                  //初始化播放器

                    player = new QMediaPlayer(this);

                    player->setVideoOutput(GVI);


                   playlist = new QMediaPlaylist;
                   playlist->playbackModeChanged(QMediaPlaylist::Sequential);   //顺序播放模式
                   player->setVolume(100);                                      //默认音量最大
                   player->setPlaylist(playlist);


                //资源列表
                  ui->tree_source->setEditTriggers(QAbstractItemView::NoEditTriggers);   //不可编辑
                  model = new QStandardItemModel(ui->tree_source);//创建模型
                  ui->tree_source->setModel(model);
                model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("资源列表"));
               // model->setItem(0,0,new QStandardItem("正在刷新.."));

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
                ui->search_list->setColumnWidth(4,100);
                ui->search_list->horizontalHeader()->setStretchLastSection(true);//自动适应布局
                ui->search_list->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);  //表头信息显示居左
                ui->search_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection); // 只能单选
                ui->search_list->setSelectionBehavior(QAbstractItemView::SelectRows);   //设置选中时为整行选中
                ui->search_list->setEditTriggers(QAbstractItemView::NoEditTriggers);     //不可编辑


           //自动隐藏鼠标定时器
            m_timer = new QTimer;
            m_timer->setSingleShot(false);
           // m_timer->start(1000);
            connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerTimeOut()));
           ui->value_Slider->hide();                      //音量调节隐藏
           ui->info_pic->setAlignment(Qt::AlignCenter);  //视频信息图片居中
           ui->page_info->setText("");                   //页数信息默认清空
           ui->menuBar->hide();       //隐藏菜单

           //ui->status->hide();
           /*  各种信号 与 槽    */

                       connect(this,SIGNAL(setshow()),&seting,SLOT(reshow()));
                       connect(this,SIGNAL(quit()),&seting,SLOT(quit()));

                       connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_B),this), SIGNAL(activated()), this, SLOT(on_pushButton_playlist_clicked()));

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

                      connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferStatusChanged(int)));


                        //表示当前媒体的数据已获取
                       // connect(player, SIGNAL(metaDataChanged()), this, SLOT(metaDataChange()));


                       //注册监视对象

                           //获取内部viewport区域
                            expWidget= ui->listWidget->viewport();
                            searchWidget=ui->search_list->viewport();

                            this->installEventFilter(this);

                            searchWidget->installEventFilter(this);

                            expWidget->installEventFilter(this);

                           ui->listWidget->installEventFilter(this);

                           ui->search_list->installEventFilter(this);

                            ui->search_name->installEventFilter(this);

                            ui->box_video->installEventFilter(this);

                            ui->box_control->installEventFilter(this);

                            ui->pushButton_sound->installEventFilter(this);

                            ui->info_pic->installEventFilter(this);

                            this->setMouseTracking(true);

                            resize(QSize(800,600)); //默认大小

}

void MainWindow::TimerTimeOut()
{
    // if(isFullScreen()){

    if(player->state()==QMediaPlayer::PlayingState && ui->box_source->isHidden()){

         //隐藏鼠标
          viewWidget->setCursor(Qt::BlankCursor);
          ui->box_control->hide();
          ui->titlebar->hide();
          scene->setSceneRect(0, 0, ui->view->width(), ui->view->height());
          GVI->setSize(QSizeF(ui->view->width(), ui->view->height()));

      }else{
          if(m_timer->isActive())m_timer->stop();
    }
}



//监视对象
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{


    if(target ==expWidget || target ==viewWidget ||  target ==searchWidget)
   {

      /*处理播放器鼠标移动消息 */

       if (event->type() == QEvent::MouseMove ){

        if(target ==viewWidget ){
         //重启定时器
         m_timer->start(3000);
          if(ui->box_control->isHidden()){
              viewWidget->setCursor(Qt::ArrowCursor);
              ui->box_control->show();
               if(!ui->action_theme_0->isCheckable()){
                   ui->titlebar->show();
               }


              viewresize();
              }
          }
        if(m_bDrag && !isFullScreen() && !this->isMaximized())
            {

                QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);
               //获得鼠标移动的距离
                QPoint distance = MouseEvent->globalPos() - mouseStartPoint;
               //QPoint distance = event->pos() - mouseStartPoint;
                //改变窗口的位置
                this->move(windowTopLeftPoint + distance);
            }

       /*处理播放器鼠标点击消息 */

         }else if(event->type() == QEvent::MouseButtonPress){


           QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);

           if(MouseEvent->button() == Qt::LeftButton)
           {
               m_bDrag = true;
               //获得鼠标的初始位置
               mouseStartPoint = MouseEvent->globalPos();
               //mouseStartPoint = event->pos();
               //获得窗口的初始位置
               windowTopLeftPoint = this->frameGeometry().topLeft();
           }
    /*处理播放器鼠标释放消息 */
       }else if(event->type() == QEvent::MouseButtonRelease){

         QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);

         if(MouseEvent->button() == Qt::LeftButton)
         {
            m_bDrag = false;
         }



          /*处理播放器鼠标双击消息 */

         }else if(event->type() == QEvent::MouseButtonDblClick &&target ==viewWidget ){
            if(ui->box_explore->currentIndex()==1){on_pushButton_full_clicked();}else{
                ui->box_explore->setCurrentIndex(1);
            }

            return  true;

       }else if(event->type() == QEvent::MouseButtonDblClick &&target ==expWidget){

           on_info_play_clicked();

            return  true;

       }


    /*处理预览图片鼠标双击消息 */

    }else if(event->type() == QEvent::MouseButtonDblClick &&target ==ui->info_pic ){
        //用默认应用打开图片
         QStringList v=ui->comboBox_name->itemData(ui->comboBox_name->currentIndex()).toString().split("|");
         QString file=topic(v.value(0),v.value(1));
         open(file);

       //处理播放器消息
    }else if(event->type() ==QEvent::KeyPress && ui->box_explore->currentIndex()==1){

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

                  }else if(keyEvent->key() == Qt::Key_Right){

                          addseek();  return  true;

                  }else if(keyEvent->key() == Qt::Key_Up){
                       volumeUp();  return  true;

                  }else if(keyEvent->key() == Qt::Key_Down){

                       volumeDown();  return  true;

                }

      //处理搜索回车消息
      }else if(event->type() ==QEvent::KeyPress && target ==ui->search_name ){

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


   //获取资源数据结束
   }else if(event->type() ==QEvent::User){
        model->removeRows(0,model->rowCount());
        model->setItem(0,0,new QStandardItem("正在刷新..."));
        ui->search_source->clear();
        ui->search_source->addItem("全部");
         for ( int i=0; i<type.size(); i++ )
         {
             model->setItem(i,0,new QStandardItem(type.value(i).name));
             if(type.value(i).name!="直播列表"){ui->search_source->addItem(type[i].name);}
              foreach (Nameinfo var,type[i].type)
              {
                    model->item(i)->appendRow(new QStandardItem(var.name));
              }

          }
         //qDebug()<<"type size:"<<type.size();

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

            if(!isFileExist(app.cache+"/"+toHash(vInfo.api)+"_"+vInfo.id.value(i)+".jpg"))
            {

               QtConcurrent::run(this,&MainWindow::ThreadFunc,3,vInfo.pic.value(i)+"|"+vInfo.api+"|"+vInfo.id.value(i));

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


    //线程搜索下载图片结束
   }else if(event->type()==QEvent::User+3){

         //取关联数据
          QStringList v=ui->comboBox_name->itemData(ui->comboBox_name->currentIndex()).toString().split("|");

         //设置预览图片

         QString file=topic(v.value(0),v.value(1));

         if(!isFileExist(file)){file=app.nopic;}

         QPixmap pixmap(file);

         ui->info_pic->setAlignment(Qt::AlignCenter);  //图片居中

         ui->info_pic->setPixmap(pixmap);


    //浏览下载图片结束
    }else if(event->type()>(QEvent::User+3)){

        int key=event->type()-QEvent::User-4;

        QString file=topic(vInfo.api,vInfo.id.value(key));

        if(isFileExist(file)){createListWidget(ui->listWidget,key,false);}

        echoload(false);

       // qDebug()<<key;

      //return true;


        //音量按钮鼠标移动事件
     }else if(target == ui->pushButton_sound){

        if (event->type() == QEvent::HoverEnter){

            ui->value_Slider->show();
        }

     //离开控制栏 隐藏音量控制条
    }else if(event->type() == QEvent::Leave and target == ui->box_control){

        ui->value_Slider->hide();

  //end
    }


    /*处理按键消息 */
    return QWidget::eventFilter(target, event);
}


//播放器媒体状态被改变
void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:showMessage("无效视频",true);break;
    case QMediaPlayer::NoMedia:showMessage("无效视频",true);break;
    case QMediaPlayer::LoadingMedia :
         showMessage("正在加载",true); viewresize();
         ui->sliderProgress->setEnabled(false);
         ui->comboBox_part->setCurrentIndex(playlist->currentIndex());
         qDebug()<<"openUrl:"<<player->currentMedia().canonicalUrl().toString();
         break;
    case QMediaPlayer::LoadedMedia:showMessage("准备就绪",true);break;
    case QMediaPlayer::StalledMedia:player->pause();showMessage("正在缓冲",false);break;
    case QMediaPlayer::BufferingMedia:showMessage("正在缓冲",false);break;
    case QMediaPlayer::BufferedMedia:player->play();hideMessage();break;
    case QMediaPlayer::EndOfMedia:showMessage("播放结束",true);break;
    case QMediaPlayer::InvalidMedia:showMessage("加载失败",false);break;
    }
}
//播放器媒体状态被改变
void MainWindow::stateChanged(QMediaPlayer::State state)
{
    QString play="QPushButton{border-image:url(://resource/img/play_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/play_on.svg) -0px 0px no-repeat;}";
    QString pause="QPushButton{border-image:url(://resource/img/pause_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/pause_on.svg) -0px 0px no-repeat;}";

    switch (state) {
     case QMediaPlayer::PlayingState:showMessage("播放",true); ui->pushButton_paly->setStyleSheet(pause);
        break;
     case QMediaPlayer::PausedState:showMessage("暂停",true);ui->pushButton_paly->setStyleSheet(play);break;
     case QMediaPlayer::StoppedState:showMessage("停止",true);ui->pushButton_paly->setStyleSheet(play);break;
    }
}


//播放器视频长度状态发生改变
void MainWindow::durationChange(qint64 playtime)
{
    app.live=false;
    ui->sliderProgress->setMaximum(playtime);
    QTime t(0,0,0);
    t = t.addMSecs(playtime);
    STimeDuration = t.toString("hh:mm:ss");
    ui->sliderProgress->setEnabled(true);


}

//播放器进度被改变
void MainWindow::positionChange(qint64 p)
{
    //播放记录
    MinWriteNotes(ui->comboBox_part->currentIndex());
    if(app.live)return;
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
    if(app.live)return;
    //时间转换
    QTime t(0,0,0);
    t = t.addMSecs(v);
    QString STimeElapse = t.toString("HH:mm:ss");
    //设置进度标签
    ui->labelTimeVideo->setText(STimeElapse+"/"+STimeDuration);

    //设置进度标签
   // ui->labelTimeDuration->setText(STimeDuration+"/"+);

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
    if(app.live)return;
    player->setPosition(player->position() - 5000);
}

void MainWindow::addseek()
{
    if(app.live)return;
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

}
//控制条下集按钮被单击
void MainWindow::on_pushButton_next_clicked()
{
    if(playlist->currentIndex()+1==playlist->mediaCount()){

        on_info_next_clicked();

    }else{

       playlist->next();

    }

}

//控制条静音按钮被单击
void MainWindow::on_pushButton_sound_clicked()
{
    QString sound="QPushButton{border-image:url(://resource/img/volume-up_out.svg) 0px 0px no-repeat;}QPushButton:hover{://resource/img/volume-up_on.svg) -0px 0px no-repeat;}";
    QString mute="QPushButton{border-image:url(://resource/img/volume-off_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/volume-off_on.svg) -0px 0px no-repeat;}";

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

    QString full="QPushButton{border-image:url(://resource/img/full_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/full_on.svg) -0px 0px no-repeat;}";
    QString general="QPushButton{border-image:url(://resource/img/general_out.svg) 0px 0px no-repeat;}QPushButton:hover{border-image:url(://resource/img/general_on.svg) -0px 0px no-repeat;}";

    if(cfull){

         //保存全屏前状态
         app.playlist=!ui->box_source->isHidden();
         app.windowState=this->windowState();
        ui->box_source->hide();
        ui->box_info->hide();
        ui->box_page->hide();
        ui->box_control->hide();
        ui->box_explore->findChildren<QTabBar*>().at(0)->hide();
         m_timer->start(2000);
        //setCursor(Qt::BlankCursor);  //隐藏鼠标
         ui->pushButton_full->setStyleSheet(general);
         ui->box_explore->setStyleSheet("border:none;");
         ui->titlebar->hide();
         showFullScreen();

    }else{

        ui->box_explore->setStyleSheet(app.playlist?"":"border:none;");
        ui->pushButton_full->setStyleSheet(full);
        ui->box_control->show();
        m_timer->stop();


        viewWidget->setCursor(Qt::ArrowCursor);  //显示正常鼠标
        showNormal();

        if(!ui->action_theme_0->isChecked()){ui->titlebar->show();}

        if(app.playlist){
            ui->box_source->show();
            ui->box_info->show();
             ui->box_page->show();
             ui->box_explore->findChildren<QTabBar*>().at(0)->show();
             setWindowState(app.windowState);
        }
    }

}

//控制条全屏按钮被单击
void MainWindow::on_pushButton_full_clicked()
{
    if(ui->box_explore->currentIndex()==1){
        switchFullScreen(!isFullScreen());
    }
}

void MainWindow::ThreadFunc(int tp,QString word){
   QString sname,id,api;QStringList v;
     if (word=="")return;


     //下载搜索资源图片
     if(tp==0){

        //取资源数据
         // type.clear();
          QMutexLocker locker(&mtx);
          getclass(app.sourcePath);getlive(app.livePath);
          QEvent event (QEvent::Type(QEvent::User+tp));
          QApplication::postEvent(this ,new QEvent(event));



     }else if(tp==1){
       //获取搜索影片数据
         v=word.split("|");
         search(v.value(0),v.value(1).toInt());
         QEvent event (QEvent::Type(QEvent::User+tp));
         QApplication::postEvent(this ,new QEvent(event));


     }else if(tp==2){
        //获取浏览影片数据
         v=ui->comboBox_name->itemData(word.toInt()).toString().split("|");
         api=v.value(0);id=v.value(1);
         getvideo(tp,api,id);

         QEvent event (QEvent::Type(QEvent::User+tp));
         QApplication::postEvent(this ,new QEvent(event));

      }else if(tp==3){

         //下载搜索图片

          v=word.split("|");

         UrlRequestImg(v.value(0),toHash(v.value(1))+"_"+v.value(2));

         QEvent event (QEvent::Type(QEvent::User+tp));

         QApplication::postEvent(this ,new QEvent(event));



     }else if(tp==4){
        //下载浏览图片
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

        //检查是否一致
        if(vInfo.id.value(index).toInt()==id.toInt()){
            ui->info_des->setHtml(todes(vInfo,index));
            loadMedia(index);

        }else{
            QtConcurrent::run(this,&MainWindow::ThreadFunc,2,QString::number(index));
        }

        //设置预览图片

        QString file=topic(api,id);

        if(!isFileExist(file)){file=app.nopic;}

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

    if(ui->box_explore->currentIndex()!=1) ui->box_explore->setCurrentIndex(1);
    if(ui->box_source->isHidden()){

       showMaximized();

       ui->box_explore->setStyleSheet("");
        ui->box_control->show();
        ui->box_source->show();
        ui->box_info->show();
        ui->box_page->show();
        ui->box_explore->findChildren<QTabBar*>().at(0)->show();


        //取消置顶
        //hide();setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);show();

    }else{

       showNormal();
        ui->box_source->hide();
        ui->box_info->hide();
        ui->box_page->hide();
        ui->box_explore->findChildren<QTabBar*>().at(0)->hide();
        ui->box_explore->setStyleSheet("border:0;");
       // resize(QSize(ui->view->width(),ui->view->height()));

         //窗口置顶
        //hide();setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);show();

   }

 //viewresize();
// ui->view->show();
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

    if(index.data().toString()=="直播列表" || index.parent().data().toString()=="直播列表"){

       int row=index.row();

       ui->box_explore->setCurrentIndex(1);

       if(index.parent().data().toString()=="直播列表"){ row=index.parent().row();}

       //if(playlist->mediaCount()!=type.value(row).type.size())

       if(!app.live)
       {
            playlist->clear();
            foreach(Nameinfo var,type.value(row).type)
            {
                playlist->addMedia(QUrl(var.id));
            }
            app.live=true;
        }

         if(index.parent().data().toString()=="直播列表"){ playlist->setCurrentIndex(index.row());}

          player->play();

         ui->page_info->setText("频道："+QString::number(index.row()+1)+"/"+QString::number(playlist->mediaCount()));

         app.live=true;



    }else{

         getpageinfo(1);
    }

}
//动态创建浏览器图片表项
void MainWindow::createListWidget(QListWidget *listWidget,int key,bool create=false){

    if(!create && listWidget->count()<=key)return;

    QMutexLocker locker(&mtx);

    if(key>=vInfo.id.size() || key>=vInfo.name.size())return;

    QString file=topic(vInfo.api,vInfo.id.value(key));

    if(!isFileExist(file)){file=app.nopic;}

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

     app.page=page;

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

      ui->listWidget->clear(); isDirExist(app.cache,true);

       getvideo(4,api,id,QString::number(page));

       ui->comboBox_name->clear();ui->comboBox_part->clear();

      for (int i=0;i<vInfo.id.size();i++) {

          ui->comboBox_name->addItem(vInfo.name.value(i),vInfo.api+"|"+vInfo.id.value(i));

             QString file=topic(vInfo.api,vInfo.id.value(i));

           createListWidget(ui->listWidget,i,true);

         if(!isFileExist(file)){

            QtConcurrent::run(this,&MainWindow::ThreadFunc,4,QString::number(i));

         }

     }

      if(ui->comboBox_name->count()==0){
         echoload(false);
         QMessageBox::warning(nullptr, "提示", "当前分类无资源!",QMessageBox::Yes);
      }

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

void  MainWindow::loadPlay(bool play,int index=0,qint64 time=0){

   if(play)
  {

        ui->sliderProgress->setEnabled(true);

        player->stop();
        playlist->clear();

          for (int i=0;i<ui->comboBox_part->count();i++)
          {

          playlist->addMedia(QUrl(ui->comboBox_part->itemData(i).toString()));

          }

        playlist->setCurrentIndex(index);

        player->setPosition(time);player->play();


   }else{

        player->pause();
   }
}


//进入播放选项卡并播放视频
void MainWindow::on_info_play_clicked()
{
    if(ui->comboBox_part->count()>0){
        ui->box_explore->setCurrentIndex(1);
        app.live=false;
        loadPlay(true,ui->comboBox_part->currentIndex());
    }

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

//搜索按钮被单击
void MainWindow::on_search_ok_clicked()
{
    if(ui->search_name->text().trimmed()==""){
       student_model->removeRows(0,student_model->rowCount());
    }else{
       echoload(true);
       QtConcurrent::run(this,&MainWindow::ThreadFunc,1,ui->search_name->text()+"|"+QString::number(ui->search_source->currentIndex()));
    }

}

//搜索列表被单击
void MainWindow::on_search_list_clicked(const QModelIndex &index)
{
    echoload(true);
    app.live=false;
    ui->info_des->clear();
    ui->comboBox_part->clear();
    if(ui->comboBox_name->currentIndex()==index.row()){
       on_comboBox_name_currentIndexChanged(index.row());
       echoload(false);
    }else{
        ui->comboBox_name->setCurrentIndex(index.row());
    }
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    //浏览
    if(index==0){
         //echoload(true);
         ui->box_page->show();
         if(ui->listWidget->count()!=ui->comboBox_name->count())getpageinfo(1);
    //播放
    }else if(index==1){
        //video->setFocus();
        ui->box_page->show();
    //搜索
    }else if(index==2){
        ui->box_page->hide();
        if(student_model->rowCount()!=ui->comboBox_name->count())on_search_ok_clicked();
    }
}



void MainWindow::on_search_source_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    on_search_ok_clicked();

}



//打开 本地
void MainWindow::on_action_open_triggered()
{
        //选择多个文件
        QString curPath=QDir::currentPath();//获取系统当前目录
        QString dlgTitle="选择多个文件"; //对话框标题
        QString filter="视频文件(*.mp4 *.avi *.wmv *.mpg *.mpeg *.m3u8 *.rmvb *.3gp *.mov *.swf *.flv);;音频文件(*.mp3 *. *.wav *.flac *.ogg *.ape *.cda *.mid *.voc);;所有文件(*.*)"; //文件过滤器
        QStringList fileList=QFileDialog::getOpenFileNames(this,dlgTitle,curPath,filter);
        if(fileList.count()>0)
        {
           playlist->clear();
           for (int i=0; i<fileList.count();i++)
            {
               playlist->addMedia(QUrl::fromLocalFile(fileList.value(i)));
             }
           player->play();
        }
}

void MainWindow::on_action_openurl_triggered()
{
      QString dlgTitle="打开URL";
      QString txtLabel="请输入URL地址";
      QString defaultInput="";
        QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入

        //QLineEdit::EchoMode echoMode=QLineEdit::Password;//密码输入
        bool ok=false;

        QString text = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);

        if (ok && !text.isEmpty()&&!text.isEmpty()){
          player->setMedia(QUrl(text));
          player->play();

      }
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


void MainWindow::setVideoMode(Qt::AspectRatioMode mode){
  switch (mode) {
    case Qt::IgnoreAspectRatio:  //铺满
         ui->action_videosize_IgnoreAspectRatio->setChecked(true);
         ui->action_videosize_KeepAspectRatio->setChecked(false);
         ui->action_videosize_KeepAspectRatioByExpanding->setChecked(false);
         break;
    case Qt::KeepAspectRatio:  // 缩放
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

   GVI->setAspectRatioMode(mode);

}

//播放器弹出菜单
void MainWindow::PlayMenu(const QPoint &pos){
     Q_UNUSED(pos);
     if(player->state()==QMediaPlayer::PlayingState){
       ui->action_player->setText("暂停");
    }else{
     ui->action_player->setText("播放");
    }
     ui->menu_play->exec(QCursor::pos());
 }

//浏览器弹出菜单
void MainWindow::ExploreMenu(const QPoint &pos){
    Q_UNUSED(pos);
    ui->menu_explore->exec(QCursor::pos());
}
//设置
void MainWindow::on_pushButton_setting_clicked()
{
    on_pushButton_seting_clicked();

      //ui->menu_play->exec(QCursor::pos());
}

void MainWindow::on_action_explore_play_triggered()
{
    on_info_play_clicked();
}

//调用关联应用直接打开，一般会是浏览器
void MainWindow::on_action_explore_xopen_triggered()
{
    if(ui->comboBox_part->count()>0){
    open(ui->comboBox_part->itemData(ui->comboBox_part->currentIndex()).toString());
    }
}

//下载到本地再调用关联应用打开
void MainWindow::on_action_explore_xplay_triggered()
{
    if(ui->comboBox_part->count()>0){
        echoload(true);
        OpenM3u8(ui->comboBox_part->itemData(ui->comboBox_part->currentIndex()).toString());
        echoload(false);
    }

}
//新窗口打开
void MainWindow::on_action_explore_xnew_triggered()
{
    if(ui->comboBox_part->count()>0){
        QStringList list;
        for (int i=0;i<ui->comboBox_part->count();i++) {
            list.append(ui->comboBox_part->itemData(i).toString());
        }
         QProcess::startDetached(QDir::currentPath()+"/vst-video",list);

    }
}

void MainWindow::on_pushButton_front_clicked()
{
    if(playlist->currentIndex()==0){
        on_info_front_clicked();
    }else{
       playlist->previous();
    }
}
//等待动画
void MainWindow::echoload(bool echo){
    if(echo){
         loading.move((x()+(width()-32)/2), (y()+(height()-32)/2));
         QTimer::singleShot(20000, &loading, SLOT(hide())); //超时
         loading.show();
    }else{
        //loading= new QDialog();

       // delete  loading;
         loading.hide();
    }
}

//动态创建等待动画窗口
void MainWindow::createLoading(){
    QVBoxLayout *widgetLayout = new QVBoxLayout;
    QLabel *load= new QLabel();
    QMovie *movie = new QMovie("://resource/img/loading.gif");
    load->setMovie(movie);movie->start();
    widgetLayout->setMargin(0);widgetLayout->setSpacing(0);
    widgetLayout->addWidget(load);
    loading.setLayout(widgetLayout);
    loading.resize(32,32);
    loading.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog |Qt::WindowStaysOnTopHint);  //无边框，置顶
    loading.setAttribute(Qt::WA_TranslucentBackground);   //背景透明
}



/*  标题栏按钮 */
void MainWindow::on_action_exit_triggered()
{
    close();
}

void MainWindow::on_pushButton_close_clicked()
{
    close();
}

void MainWindow::on_pushButton_mini_clicked()
{
    showMinimized();
}

void MainWindow::on_pushButton_seting_clicked()
{

    renotes();  //刷新播放记录
    ui->menu_seting->exec(QCursor::pos());
}

void MainWindow::on_pushButton_max_clicked()
{

    if(isMaximized()){
        showNormal() ;
        ui->pushButton_max->setToolTip("最大化");
    }else{
        showMaximized();
        ui->pushButton_max->setToolTip("还原");
    }

}

void MainWindow::on_action_resource_triggered()
{
    model->removeRows(0,model->rowCount());
    model->setItem(0,0,new QStandardItem("正在刷新..."));
    QtConcurrent::run(this,&MainWindow::ThreadFunc,0,QString::number(1));
}


//写播放记录


void MainWindow::MinWriteNotes(int index=0){
    Notesinfo note;QString str;
    note.title=QString("【%1】%2").arg(ui->comboBox_name->currentText()).arg(ui->comboBox_part->currentText());
    if(ui->comboBox_name->currentText()!=""){
    note.api=ui->comboBox_name->itemData(ui->comboBox_name->currentIndex()).toString();
    note.id="";
    note.part=QString::number(index);
    note.time=QString::number(player->position());
    str=QString("%1|%2|%3|%4").arg(note.title).arg(note.api).arg(note.part).arg(note.time);
    config.set("notes",toHash(note.api),str);
    }
}

//刷新播放记录
void MainWindow::renotes()
{
  QStringList list,keys;
  QString key;
  ui->menu_notes->clear();
  keys=config.getKeys("notes");

  if(keys.size()>0){

    for(int i=0;i<keys.size();i++){
       key=config.getValue(keys.value(i)).toString();
       list=key.split("|");
       QAction *test = new QAction(list.value(0), this);test->setData(list);
       ui->menu_notes->addAction(test);
    }
    //添加清除按钮
    ui->menu_notes->addSeparator();
    QAction *test = new QAction("清空记录", this);test->setData("clear");
    ui->menu_notes->addAction(test);
}

}

//播放记录被选择
 void MainWindow::menu_action_notes_triggered(QAction *action){

    QString name,api,id,part,time;

    if(action->data().toString()=="clear"){

        config.remove("notes");
        renotes();

   }else{

    name=action->data().toList().value(0).toString();
    api=action->data().toList().value(1).toString();
    id=action->data().toList().value(2).toString();
    part=action->data().toList().value(3).toString();
    time=action->data().toList().value(4).toString();

    echoload(true);
    app.live=false;

    getvideo(2,api,id);

    ui->comboBox_name->clear();

    ui->comboBox_part->clear(); ui->info_des->clear();

    ui->comboBox_name->addItem(vInfo.name.value(0),api+"|"+id);

     for(int i=0;i<vInfo.video.size();i++){
         QStringList list =vInfo.video.value(i).split("#");
         QStringList v;

         //下载图片

         if(!isFileExist(app.cache+"/"+toHash(vInfo.api)+"_"+vInfo.id.value(i)+".jpg"))
         {

            QtConcurrent::run(this,&MainWindow::ThreadFunc,3,vInfo.pic.value(i)+"|"+vInfo.api+"|"+vInfo.id.value(i));

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

      ui->box_explore->setCurrentIndex(1);

      ui->comboBox_part->setCurrentIndex(part.toInt());

      loadPlay(true,part.toInt(),time.toInt());

   }

}
 //标题栏菜单
 void MainWindow::TitlebarMenu(const QPoint &pos){
     Q_UNUSED(pos);
     if(this->isMaximized()){
         ui->action_max->setText("还原");
     }else{
         ui->action_max->setText("最大化");
     }

     ui->action_top->setChecked(ui->action_tophint->isChecked());
     ui->menu_titlebar->exec(QCursor::pos());

 }



 void MainWindow::Sleep(int msec)
 {
     QTime dieTime = QTime::currentTime().addMSecs(msec);
     while( QTime::currentTime() < dieTime )
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
 }


/*   主题切换     */

 void MainWindow::switchtheme(int index,bool sleep=true){

     QString light="QPushButton{border:none;background-color:transparent;}QPushButton:hover{background-color:#505050;}";

     QString deep="QPushButton{border:none;background-color:transparent;}QPushButton:hover{background-color:#E0E0E0;}";

   switch (index) {

      default:

       case 0:

            if(sleep){

                 QTimer::singleShot(500, this, [=]{setWindowFlags(app.Flags);show();});
            }else{
                 setWindowFlags(app.Flags);
            }

            ui->titlebar->hide();this->setStyleSheet("");
            ui->action_theme_0->setChecked(true);
            ui->action_theme_1->setChecked(false);
            ui->action_theme_2->setChecked(false);

           break;

       case 1:
       ui->titlebar->show();

       if(sleep){

             QTimer::singleShot(500, this, [=]{setWindowFlags(windowFlags()|Qt::FramelessWindowHint);show();});
       }else{
            setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
       }



       ui->action_theme_0->setChecked(false);
       ui->action_theme_1->setChecked(true);
       ui->action_theme_2->setChecked(false);


       //图标
       ui->pushButton_max->setIcon(QIcon("://resource/img/maximize_light.svg"));
       ui->pushButton_close->setIcon(QIcon("://resource/img/close_light.svg"));
       ui->pushButton_mini->setIcon(QIcon("://resource/img/minimize_light.svg"));
       ui->pushButton_seting->setIcon(QIcon("://resource/img/menu_light.svg"));


       //样式 this->setStyleSheet("color:#00F;QWidget{background-color:#606060;}");


       this->setStyleSheet("QWidget{background-color:#606060;border-color:#606060}"
                           "QTreeView{color:#909090;}"
                           "QComboBox{color:#909090;}QComboBox QAbstractItemView {background-color:#f0f0f0;}"
                           "QTextEdit{color:#909090;}"
                           "QLineEdit#page_edit{background-color:#606060;}"
                           "QLineEdit#search_name{background-color:#909090;}"
                           );


       ui->pushButton_max->setStyleSheet(light);
       ui->pushButton_close->setStyleSheet(light);
       ui->pushButton_mini->setStyleSheet(light);
       ui->pushButton_seting->setStyleSheet("QPushButton{border:none;background-color:transparent;}QPushButton:pressed{background-color:#505050;}");


       //expWidget->setStyleSheet("border-color:#606060");

       break;

      //默认浅色主题

       case 2:
       if(sleep){
             QTimer::singleShot(500, this, [=]{setWindowFlags(windowFlags()|Qt::FramelessWindowHint);show();});
       }else{
            setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
       }
       ui->titlebar->show();
       ui->action_theme_0->setChecked(false);
       ui->action_theme_1->setChecked(false);
       ui->action_theme_2->setChecked(true);




       //图标
       ui->pushButton_max->setIcon(QIcon("://resource/img/maximize_deep.svg"));
       ui->pushButton_close->setIcon(QIcon("://resource/img/close_deep.svg"));
       ui->pushButton_mini->setIcon(QIcon("://resource/img/minimize_deep.svg"));
       ui->pushButton_seting->setIcon(QIcon("://resource/img/menu_deep.svg"));

       //样式

       this->setStyleSheet("QWidget{background-color:#F0F0F0;border-color:#000}"
                           "QTreeView{color:#606060;}"
                           "QComboBox{color:#606060;}"
                           "QComboBox QAbstractItemView{background-color:#ff0;}"
                           "QTextEdit{color:#606060;}"
                           "QLineEdit#page_edit{background-color:#f0f0f0;}"
                           "QLineEdit#search_name{background-color:#c0c0c0;}"


                           );

       ui->listWidget->setStyleSheet("border-color:#f0f0f0");
       ui->pushButton_max->setStyleSheet(deep);
       ui->pushButton_close->setStyleSheet(deep);
       ui->pushButton_mini->setStyleSheet(deep);
       ui->pushButton_seting->setStyleSheet("QPushButton{border:none;background-color:transparent;}QPushButton:pressed{background-color:#e0e0e0;}");

       break;
   }


 }

 void MainWindow::on_action_theme_0_triggered()
 {
     switchtheme(0);
     config.set("set","theme",0);
 }

void MainWindow::on_action_theme_1_triggered()
{
    switchtheme(1);
    config.set("set","theme",1);
}

void MainWindow::on_action_theme_2_triggered()
{
      switchtheme(2);
      config.set("set","theme",2);
}



/*    同步视频大小    */


void MainWindow::resizeEvent(QResizeEvent* size){
  Q_UNUSED(size);
  viewresize();
}


void MainWindow::viewresize(){

     widthV=viewWidget->width();
     heightV=viewWidget->height();
     scene->setSceneRect(0, 0, widthV, heightV);
     GVI->setSize(QSizeF(widthV, heightV));
     scene->update();GVI->update();
     ui->view->updateSceneRect(QRectF(0,0,widthV, heightV));
     //ui->view->show();
}

/*  视频消息输出    */

void MainWindow::showMessage(QString s,bool autoHide=false)
{
    //GTI->setY(ui->view->y()+ui->view->height()-40);
    GTI->setPlainText(s);
    GTI->show();
    if (autoHide)
    {
       QTimer::singleShot(5000, this, [=]{GTI->hide();});
    }
}

void MainWindow::hideMessage()
{
      GTI->hide();
}



/*  旋转镜像  */
void MainWindow::on_action_rotate_left_triggered()
{
    qreal x = GVI->boundingRect().width()/2.0;
    qreal y = GVI->boundingRect().height()/2.0;
    app.angle -= 90;
    GVI->setTransform(QTransform().translate(x,y).rotate(app.angle).translate(-x,-y));
}

void MainWindow::on_action_rotate_right_triggered()
{
    qreal x = GVI->boundingRect().width() / 2.0;
    qreal y = GVI->boundingRect().height() / 2.0;
    app.angle+= 90;
    GVI->setTransform(QTransform().translate(x,y).rotate(app.angle).translate(-x,-y));
}

void MainWindow::on_action_rotate_x_triggered()
{
    app.mh = -app.mh;
    //旋转
    qreal x = GVI->boundingRect().width() / 2.0;
    qreal y = GVI->boundingRect().height() / 2.0;
    GVI->setTransform(QTransform().translate(x, y).scale(app.mh, 1).translate(-x, -y));
}

void MainWindow::on_action_rotate_y_triggered()
{
    app.mv = -app.mv;
    qreal x = GVI->boundingRect().width() / 2.0;
    qreal y = GVI->boundingRect().height() / 2.0;
    GVI->setTransform(QTransform().translate(x, y).scale(1, app.mv).translate(-x, -y));
}



//媒体信息
void MainWindow::on_action_info_triggered()
{
    QString s;QStringList SLMD;
    SLMD = player->availableMetaData();
    for(int i=0; i<SLMD.size(); i++){
        if(SLMD.at(i)=="PixelAspectRatio" || SLMD.at(i)=="Resolution"){
            s += SLMD.at(i) + ": " + QString::number(player->metaData(SLMD.at(i)).toSize().width()) + " X " + QString::number(player->metaData(SLMD.at(i)).toSize().height()) + "\n";
        }else{
            s += SLMD.at(i) + ": " + player->metaData(SLMD.at(i)).toString() + "\n";
        }
    }
    QMessageBox MBInfo(QMessageBox::NoIcon, "信息", s);
    QImage imageCover = player->metaData(QMediaMetaData::ThumbnailImage).value<QImage>();
    if(imageCover.isNull()){
         MBInfo.setIconPixmap(QPixmap("://resource/img/ico.svg"));
         //MBInfo.setIconPixmap(*ui->info_pic->pixmap());
    }else{
         MBInfo.setIconPixmap(QPixmap::fromImage(imageCover));
    }

    MBInfo.exec();
}


/*   置顶/取消   */
 void  MainWindow::setWindowsTopHint(bool top){

         if(top){

         if(player->isVideoAvailable() && player->state()==QMediaPlayer::PlayingState){player->pause();Sleep(1000);}
             setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
             show();
             player->play();
             //if(app.playState==true){app.playState=false;player->play();Sleep(100);}
             config.set("set","tophint",1);
         }else{
              if(player->isVideoAvailable() && player->state()==QMediaPlayer::PlayingState){player->pause();Sleep(1000);}
              setWindowFlags(windowFlags()^Qt::WindowStaysOnTopHint);
              show();
              player->play();
              config.set("set","tophint",0);
         }
  }



void MainWindow::on_action_tophint_triggered()
{
    if(ui->action_tophint->isChecked()){

        QTimer::singleShot(500, this, [=]{setWindowsTopHint(true);});

    }else{

        QTimer::singleShot(500, this, [=]{setWindowsTopHint(false);});

    }
}

//切换选项卡
void MainWindow::on_box_explore_currentChanged(int index)
{

    switch (index) {
      case 0:
        if(player->state()==QMediaPlayer::PlayingState)
        {
         ui->info_pic->hide();
         ui->info_video->layout()->addWidget(ui->view);
         ui->view->show();
        }
        if(ui->listWidget->count()!=vInfo.name.count()){getpageinfo (app.page);}
        break;
    case 1:
        ui->info_pic->show();
        ui->box_player->addWidget(ui->view);
        ui->view->show();
        break;
    case 2:
        if(player->state()==QMediaPlayer::PlayingState)
        {
          ui->info_pic->hide();
          ui->info_video->layout()->addWidget(ui->view);
          ui->view->show();
         }

        if(ui->comboBox_name->count()!=student_model->rowCount()){on_search_ok_clicked(); }
        break;
    }
       viewresize();
}


void MainWindow::on_action_update_triggered()
{
   this->showMinimized();
   open("https://github.com/xymov/vst-video/releases") ;
}

void MainWindow::on_action_set_triggered()
{
     emit setshow();
}

void MainWindow::bufferStatusChanged(int index){
 if(index<100){
    showMessage(QString("正在缓冲: %1%").arg(index));

 }
}



//关于窗口
void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "全聚合影视 v2.54\n一款基于 Qt5 的云播放器。\n作者：nohacks\nE-mail: nohacks@vip.qq.com\n主页：https://github.com/xymov\n致谢：\n播放器：https://github.com/sonichy/HTYMediaPlayer\n");
    aboutMB.setIconPixmap(QPixmap("://resource/img/ico.svg"));
    aboutMB.exec();
}


