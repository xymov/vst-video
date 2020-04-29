#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

#include <QMediaPlayer> //add
#include <QComboBox>
#include <QPlainTextEdit>
#include <QTextEdit>

//QtNetwork
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QTextCodec>

//log

#include <QFile>
#include <QByteArray>
#include <QDateTime>

//xml
#include <QXmlStreamReader>
#include <QDomDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_paly_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_sound_clicked();

    void on_pushButton_full_clicked();

    void sliderProgressReleased();

     void positionChange(qint64);

     void durationChange(qint64);

     void mediaStatusChanged(QMediaPlayer::MediaStatus);

     void on_sliderProgress_sliderMoved(int position);

     void on_Button_search_clicked();

     void on_comboBox_name_currentIndexChanged(int index);

     void on_comboBox_part_currentIndexChanged(int index);


private:

    Ui::MainWindow *ui;
    bool eventFilter(QObject *target, QEvent *event);
    QMediaPlayer *player;
    QVideoWidget *video;
    void setSTime(qint64);
    QString STimeDuration="00:00:00";
    QString API;

    static QString UrlRequestGet( const QString url )

        {

            QNetworkAccessManager qnam;
            const QUrl aurl( url );
            QNetworkRequest qnr( aurl );
            qnr.setRawHeader("Content-Type","application/json");
            QNetworkReply *reply = qnam.get( qnr );

            QEventLoop eventloop;
            connect( reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
            eventloop.exec( QEventLoop::ExcludeUserInputEvents);

            QTextCodec *codec = QTextCodec::codecForName("utf8");
            QString replyData = codec->toUnicode( reply->readAll() );

            reply->deleteLater();
            reply = 0;

            return replyData;
        }

        static QString UrlRequestPost( const QString url,const QString data )
        {
            QNetworkAccessManager qnam;
            const QUrl aurl( url );
            QNetworkRequest qnr( aurl );
            qnr.setRawHeader("Content-Type","application/json;charset=GBK");
            QNetworkReply *reply = qnam.post( qnr, data.toLocal8Bit() );

            QEventLoop eventloop;
            connect( reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
            eventloop.exec( QEventLoop::ExcludeUserInputEvents);

            QTextCodec *codec = QTextCodec::codecForName("utf8");
            QString replyData = codec->toUnicode( reply->readAll() );

            reply->deleteLater();
            reply = 0;

            return replyData;
        }

        static void log( const QString &logFile, const QByteArray &data )
            {
                QFile file( logFile );
                if( file.open(QIODevice::WriteOnly | QIODevice::Append) )
                {
                    QByteArray ba = QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ).toLocal8Bit();
                    ba += "     " + data + "\n";
                    file.write( ba );
                    file.close();
                }
            }



        static QDomElement xmltoDom(QString xmlText)
        {
            QDomDocument doc; doc.setContent(xmlText.toUtf8());

            QDomElement  docElem = doc.documentElement();

            return docElem;
        }


        static void listDom(QDomElement docElem,QString tag)
        {
            QDomNode node = docElem.firstChild();

            if(node.toElement().isNull())
            {
                //pItem->setText (1, docElem.text());
            }
            while(!node.isNull())
            {
                QDomElement element = node.toElement();

                if( !element.isNull() )
                {

                    if(element.tagName()==tag){

                       // box->addItem(element.text());

                    }
                    listDom(element,tag);

                }

                node = node.nextSibling();
            }
            return;
        }

        //搜索显示影片名称
        static void getname(QComboBox * box,const QString &api,const QString &name){

            QString done=UrlRequestGet(api+"?wd="+name);

            //xml方法
            // listDom(xmltoDom(done),box,"name");

             QRegExp rxlen("<id>(\\d+)</id>.*<name><!\\[CDATA\\[(.*)\\]\\]></name>");

              rxlen.setMinimal (true) ;
                 int pos=0;
                 box->clear();

                 while ((pos=rxlen.indexIn(done,pos))!=-1){
                     QString value = rxlen.cap(1);
                     QString unit = rxlen.cap(2);
                     box->addItem(rxlen.cap(2),rxlen.cap(1));
                     pos+= rxlen.matchedLength();
                 }
        }
            static void getpart(QComboBox * box,QTextEdit * des,const QString &api,const QString &id){
                 QString done=UrlRequestGet(api+"?ac=videolist&ids="+id);
                 QString str;  
                 //qDebug()<<"done:"<<done;
                 QRegExp rxlen("<dd.*><!\\[CDATA\\[(.*)\\]\\]></dd>.*<des><!\\[CDATA\\[(.*)\\]\\]></des>");

                  rxlen.setMinimal (true) ;
                     int pos=0;
                     box->clear();
                     des->clear();

                     while ((pos=rxlen.indexIn(done,pos))!=-1){

                              // des->setPlainText(rxlen.cap(2));

                               des->setHtml(rxlen.cap(2));

                        // qDebug()<<"done:"<<rxlen.cap(1);

                         str=rxlen.cap(1);

                         QStringList list = str.split("#");
                         QStringList video;

                         foreach (QString s, list) {
                             //第30集$https://index.m3u8$ckm3u8

                              video=s.split("$");
                             if(video.size()>1){
                                 box->addItem(video[0],video[1]);
                             }
                         }
                         pos+= rxlen.matchedLength();
                     }
            }

          //本地编码转换为Unicode
          static QString toUnicode(QString text){

            QTextCodec *codec = QTextCodec::codecForLocale();
            char* ch; QByteArray ba = text.toLatin1(); ch=ba.data();
            return codec->toUnicode(ch);
        }

};
#endif // MAINWINDOW_H
