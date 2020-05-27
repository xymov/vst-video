#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::initTitleBar()
{
    /*  自定义标题栏    */
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    FramelessHelper *pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);           // 激活当前窗体
    pHelper->setTitleHeight(50);         // 设置窗体的标题栏高度，可拖动高度
    pHelper->setWidgetMovable(true);     // 设置窗体可移动
    pHelper->setWidgetResizable(true);   // 设置窗体可缩放
    pHelper->setOnlyTitleBarMove(false); // 设置是否只标题栏可拖动
    pHelper->setRubberBandOnMove(true);  // 设置橡皮筋效果-可移动
    // pHelper->setRubberBandOnResize(true);  // 设置橡皮筋效果-可缩放

    // 标题栏菜单关联
    ui->titlebar->setContextMenuPolicy(Qt::CustomContextMenu); // 鼠标右键点击控件时会发送一个customContextMenuRequested信号
    connect(ui->titlebar, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(TitlebarMenu(const QPoint &)));

    // 关闭按钮
    ui->pushButton_close->setFixedSize(60, 40);
    //ui->pushButton_close->setIcon(QIcon("://resource/img/close_out.svg"));
    ui->pushButton_close->setIconSize(QSize(14, 14));

    // 最大化按钮
    ui->pushButton_max->setFixedSize(60, 40);
    //ui->pushButton_max->setIcon(QIcon("://resource/img/normal-size_out.svg"));
    ui->pushButton_max->setIconSize(QSize(16, 16));

    // 最小化按钮
    ui->pushButton_mini->setFixedSize(60, 40);
    //ui->pushButton_mini->setIcon(QIcon("://resource/img/minimize.svg"));
    ui->pushButton_mini->setIconSize(QSize(16, 16));

    // 设置按钮
    ui->pushButton_seting->setFixedSize(60, 40);
    //ui->pushButton_seting->setIcon(QIcon("://resource/img/menu.svg"));
    ui->pushButton_seting->setIconSize(QSize(16, 16));

    this->setMouseTracking(true);
}

// 标题栏菜单
void MainWindow::TitlebarMenu(const QPoint &pos)
{
    Q_UNUSED(pos);

    if (this->isMaximized())
    {
        ui->action_max->setText("还原");
    }
    else
    {
        ui->action_max->setText("最大化");
    }

    ui->menu_titlebar->exec(QCursor::pos());
}

void MainWindow::on_pushButton_seting_clicked()
{
    renotes();

    ui->menu_seting->exec(QCursor::pos());
}

void MainWindow::on_action_exit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_pushButton_close_clicked()
{
    close();
}

void MainWindow::on_pushButton_mini_clicked()
{
    showMinimized();
}

void MainWindow::on_pushButton_max_clicked()
{
    isMaximized() ? showNormal() : showMaximized();
}
