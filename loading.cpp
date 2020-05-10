#include "loading.h"
#include "ui_loading.h"


loading::loading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loading)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog |Qt::WindowStaysOnTopHint);  //无边框，置顶
    setAttribute(Qt::WA_TranslucentBackground);   //背景透明

        //屏幕居中显示
        int frmX = width();
        int frmY = height();

        QDesktopWidget w;
        int deskWidth = w.width();
        int deskHeight = w.height();

        QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
        move(movePoint);

        //加载gif图片
        QMovie *movie = new QMovie(":/pic/rc/loading.gif");
        ui->lbl_gif->setMovie(movie);
        movie->start();

       connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()), this, SLOT(close()));

}

void loading::quit(){

   exit(0);
}

loading::~loading()
{
    //delete ui->lbl_gif;

    delete ui;
}


