#include "mainwindow.h"

#include <QApplication>
#include <QtCore>

// 当前运行信息
Appinfo app;

// 资源分类信息
// QMap<QString,SourceInfo>type;
QQueue<SourceInfo> sources;

// 当前搜索资源信息
QQueue<VideoInfo> vSearch;

// 当前搜索列表中的单个资源（需要事先获取）或浏览分类列表中的单页资源信息
VideoInfo vBrowse;

// 详情中的当前资源信息
VideoInfo vDetail;

int main(int argc, char *argv[])
{
// 适配高清屏
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);
    //a.setQuitOnLastWindowClosed(true);
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

    // 解决汉字乱码问题
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    qRegisterMetaType<SourceInfo>("SourceInfo");
    qRegisterMetaType<Appinfo>("Appinfo");

    qSetMessagePattern("[ %{file}: %{line} ] %{message}");

    MainWindow w;    
    w.show();

    return a.exec();
}
