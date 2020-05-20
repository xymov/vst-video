#include "mainwindow.h"

#include <QApplication>

 #include <QtCore>

int main(int argc, char *argv[])
{
//适配高清屏
#if (QT_VERSION >= QT_VERSION_CHECK(5,9,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#endif
    QApplication a(argc, argv);

    //解决汉字乱码问题
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    qRegisterMetaType<SourceInfo>("SourceInfo");
    qRegisterMetaType<Appinfo>("Appinfo");
    qSetMessagePattern("[ %{file}: %{line} ] %{message}");
    MainWindow w;
    a.connect(&a, SIGNAL(lastWindowClosed()),&a,SLOT(quit()));




    //a.setQuitOnLastWindowClosed(true);
    w.show();
    return a.exec();
}
