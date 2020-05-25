#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 圆角矩形
     QDialog * Parent=new QDialog();
     Parent->setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint |Parent->windowFlags());
     Parent->setAttribute(Qt::WA_TranslucentBackground);   //背景透明
     this->setParent(Parent);
     this->setStyleSheet("background-color:gray;border-radius:10px;");
   */

    // 组件初始化
    initMainWindow();
    initTitleBar();
    initLoading();
    initTray();
    initResource();
    initList();
    initPlayer();
    initTheme();
    initRecord();

    // 设置播放器为默认选项卡
    ui->tabWidget->setCurrentWidget(ui->tab_player);

    // 检查命令行参数
    getCommond();
}

// Delete信号
MainWindow::~MainWindow()
{
    seting.close();

    delete _tableView;

    delete video;
    delete player;
    delete ui;

    delete _volume_slider;
    delete _volume_value;

    delete _tray;

    qDebug() << "App quit success!" << endl;

    exit(0);
}

// 主窗口初始化
void MainWindow::initMainWindow()
{
    // 保存窗口设置
    app.Flags = windowFlags();

    // 窗口居中
    move((QApplication::desktop()->width() - width()) / 2, (QApplication::desktop()->height() - height()) / 2);
    // 默认大小
    resize(QSize(800, 600));

    // 隐藏菜单
    ui->menuBar->hide();
    //ui->status->hide();

    connect(this, SIGNAL(setshow()), &seting, SLOT(reshow()));
    connect(this, SIGNAL(quit()), &seting, SLOT(quit()));

    // 关闭窗口退出应用 相当于Close信号关联Delete信号
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // 注册监视对象
    this->installEventFilter(this);

    // 是否置顶
    // connect(ui->action_tophint,SIGNAL(toggled(bool)),this,SLOT(setWindowsTopHint(bool)));
    if (config.get("set", "tophint").toInt() == 1)
    {
        //setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint|windowFlags());
        ui->action_top->setChecked(true);
    }
    else
    {
        //setWindowFlags(Qt::FramelessWindowHint |windowFlags());
        ui->action_top->setChecked(false);
    }
}

// 检查命令行
void MainWindow::getCommond()
{
    app.arguments = QCoreApplication::arguments();

    if (app.arguments.count() > 1)
    {
        playlist->clear();
        for (int i = 1; i < app.arguments.count(); i++)
        {
            playlist->addMedia(QUrl(app.arguments.value(i)));
        }

        ui->tabWidget->setCurrentWidget(ui->tab_player);
        on_pushButton_playlist_clicked();
        player->play();
    }
    else
    {
        setWindowState(Qt::WindowMaximized);
        //setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog |Qt::WindowStaysOnTopHint);  // 无边框，置顶

        // 刷新资源列表
        on_action_resource_triggered();
    }
}

// 特定消息监视
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    // 特定组件的消息
    if (target == video)
    {
        // 处理播放器鼠标移动消息
        if (event->type() == QEvent::MouseMove)
        {
            // 重启定时器
            m_timer->start(3000);

            if(ui->box_source->isHidden())
            {
                if (ui->box_control->isHidden())
                {
                    video->setCursor(Qt::ArrowCursor);
                    ui->box_control->show();
                    ui->titlebar->show();
                }
            }
        }
        // 处理播放器鼠标双击消息
        else if (event->type() == QEvent::MouseButtonDblClick)
        {
            on_pushButton_full_clicked();
        }
    }
    // 鼠标移动事件
    else if (event->type() == QEvent::HoverEnter)
    {
        // 音量按钮鼠标移动事件
        if (target == ui->pushButton_sound)
        {
            showVolumeSlider();
        }
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        if(target != _volume_slider)
        {
            hideVolumeSlider();
        }
    }
    else if(event->type() == QEvent::FocusOut)
    {
        // 离开控制栏 隐藏音量控制条
        if (target == _volume_slider)
        {
            hideVolumeSlider();
        }
    }
    // 处理搜索回车消息
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // 取消消息
        if (keyEvent->key() == Qt::Key_Escape)
        {
            if (ui->box_control->isHidden())
            {
                ui->tabWidget->setCurrentWidget(ui->tab_player);

                video->setCursor(Qt::ArrowCursor);
                ui->box_control->show();
                ui->titlebar->show();
            }
        }
        // 搜索框
        else if (target == ui->search_name)
        {
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220)
            {
                on_search_name_returnPressed();
                return true;
            }
        }
        // 浏览器
        else if (target == ui->listWidget)
        {
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220)
            {
                on_listWidget_itemDoubleClicked(ui->listWidget->currentItem());
                return true;
            }
        }
        // 搜索表格
        else if (target == _tableView)
        {
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220)
            {
                on_tableView_doubleClicked(_tableView->currentIndex());
                return true;
            }
        }
        // 播放器
        else if(ui->tabWidget->currentWidget() == ui->tab_player)
        {
            // 回车消息
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220)
            {
                on_pushButton_full_clicked();

                return true;
            }
            // 空格消息
            else if (keyEvent->key() == Qt::Key_Space)
            {
                on_pushButton_paly_clicked();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Left)
            {
                decseek();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Right)
            {
                addseek();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Up)
            {
                volumeUp();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Down)
            {
                volumeDown();
                return true;
            }
        }
    }

    // 处理其他消息
    return QWidget::eventFilter(target, event);
}

void MainWindow::on_action_tophint_toggled(bool arg1)
{
    if (arg1)
    {
        QTimer::singleShot(500, this, SLOT(setWindowsTopHint()));
    }
    else
    {
        QTimer::singleShot(500, this, SLOT(remWindowsTopHint()));
    }
}

// 置顶
void MainWindow::setWindowsTopHint()
{
    hide();

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    config.set("set", "tophint", 1);

    show();
}

// 取消置顶
void MainWindow::remWindowsTopHint()
{
    hide();

    setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);
    config.set("set", "tophint", 0);

    show();
}

// 关于窗口
void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "<H3>全聚合影视 v2.5.1（2020.5.24 魔改版）</H3>一款基于 Qt5 的云播放器。<H4>作者：nohacks</H4><ul><li>E-mail：<a href='mailto:nohacks@vip.qq.com'>nohacks@vip.qq.com</a></li><li>主&nbsp;&nbsp;&nbsp;&nbsp;页：<a href='htts://github/xymov'>https://github.com/xymov</a></li></ul><H4>致谢：</H4><ul><li>播放器：<a href='https://github.com/sonichy/HTYMediaPlayer'>https://github.com/sonichy/HTYMediaPlayer</a></li></ul>");
    aboutMB.setIconPixmap(QPixmap("://resource/img/icon.png"));
    aboutMB.setStyleSheet("QLabel#qt_msgbox_label{"
                          "min-width: 360px;"
                          "min-height: 222px; "
                          //"font-size:14px;"
                          "}");
    aboutMB.exec();
}
