#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

//QMediaPlayer
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

//QThread
#include <QThread>

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

     void positionChange(int);

     void durationChange(int);

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
    void setSTime(int);
    QString STimeDuration="00:00:00";
    QString API;

    void ThreadFunc(bool type,QString name);

    //影片信息
    QVector<QString>vid;QVector<QString>vname;
    QString vdes;QVector<QString>vurl;


    //取网页数据
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
            reply = nullptr;

            return replyData;
        }

      //提交网页数据
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
            reply =nullptr;

            return replyData;
        }

         //写日志
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


      //xml文本转dom对象
        static QDomElement xmltoDom(QString xmlText)
        {
            QDomDocument doc; doc.setContent(xmlText.toUtf8());

            QDomElement  docElem = doc.documentElement();

            return docElem;
        }

     //dom遍历xml获取影片信息
        static void listDom(QDomElement docElem,QVector<QString>&vid,QVector<QString>&vname,QVector<QString>&vurl,QString &vdes)
        {
            //vid.clear();vname.clear();vurl.clear();vdes.clear();

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

                    if(element.tagName()=="id"){

                        vid.append(element.text());


                    }else if(element.tagName()=="name"){

                          vname.append(element.text());


                   }else if(element.tagName()=="dd"){
                          vurl.append(element.text());


                    }else if(element.tagName()=="des"){

                          vdes=element.text();
                    }

                    listDom(element,vid,vname,vurl,vdes);

                }

                node = node.nextSibling();
            }

            return;
        }


        //搜索显示影片名称
        static void  getvideo(bool type,const QString &api,const QString &word,QVector<QString>&vid,QVector<QString>&vname,QVector<QString>&vurl,QString &vdes){
         QString  url,done;
         if(type){url="?wd=";}else{url="?ac=videolist&ids=";}
             done=UrlRequestGet(api+url+word);
             listDom(xmltoDom(done),vid,vname,vurl,vdes);
        }


        //正则搜索显示影片名称
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

        //正则搜索影片详情

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
