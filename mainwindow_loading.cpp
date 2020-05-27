#include "mainwindow.h"

#include <QLabel>
#include <QVBoxLayout>

// 动态创建等待动画窗口
void MainWindow::initLoading()
{
    QVBoxLayout *widgetLayout = new QVBoxLayout;
    widgetLayout->setMargin(0);
    widgetLayout->setSpacing(0);

    QMovie *movie = new QMovie("://resource/img/loading.gif");
    movie->start();
    QLabel *load = new QLabel();
    load->setMovie(movie);
    widgetLayout->addWidget(load);

    loading.setLayout(widgetLayout);
    loading.resize(32, 32);
    loading.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint); // 无边框，置顶
    loading.setAttribute(Qt::WA_TranslucentBackground);                                      // 背景透明
}

// 显示等待动画
void MainWindow::show_loading(bool show)
{
    if (show)
    {
        loading.move((x() + (width() - 32) / 2), (y() + (height() - 32) / 2));
        QTimer::singleShot(20000, &loading, SLOT(hide())); // 超时
        loading.show();
    }
    else
    {
        //loading= new QDialog();

        // delete  loading;
        loading.hide();
    }
}
