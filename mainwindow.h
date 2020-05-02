#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

#include <QTimer>


//QMediaPlayer
#include <QMediaPlayer> //add
#include <QMediaPlaylist>
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
#include <QStandardItemModel>

#include <QTreeView>

#include "loading.h"

typedef struct playinfo
{
    QString last;
    QString id;
    QString tid;
    QString name;
    QString type;
    QString pic;
    QString note;
    QString actor;     //演员
    QString director;  //导演
    QString url;
    QString des;

    operator QVariant() const
        {
            return QVariant::fromValue(*this);
        }

}play;
Q_DECLARE_METATYPE(play);

typedef struct videoinfo
{
    QString page;
    QString pagecount;
    QString pagesize;
    QString recordcount;
    QList<QVariant>video;
}video;
Q_DECLARE_METATYPE(video);




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
     void stateChanged(QMediaPlayer::State);

     void on_sliderProgress_sliderMoved(int position);

     void on_Button_search_clicked();

     void on_comboBox_name_currentIndexChanged(int index);

     void on_comboBox_part_currentIndexChanged(int index);

     void addseek();
     void decseek();
     void volumeUp();
     void volumeDown();
     void TimerTimeOut();


     void on_pushButton_playlist_clicked();


     void on_value_Slider_valueChanged(int value);

     void volumeChange(int value);

     void on_tree_source_pressed(const QModelIndex &index);

signals:
     void quit();

private:

    Ui::MainWindow *ui;
    bool eventFilter(QObject *target, QEvent *event);
    void ThreadFunc(int type,QString name);
    QString STimeDuration="00:00:00";
    QMediaPlaylist *playlist;
    QMediaPlayer *player;
    QVideoWidget *video;
    void setSTime(qint64);
    void echoload(bool);
    QString API;
    loading load;
    QTimer *m_timer;

    //影片信息
     QStringList vid,vname,vurl;
     QString vdes; QMap<QString,QString>type;
     QStandardItemModel * model;
     QMap<QString,QString>urls;

      //取网页数据
      QString UrlRequestGet( const QString url )
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
        QString UrlRequestPost( const QString url,const QString data )
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
        void log( const QString &logFile, const QByteArray &data )
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
        QDomElement xmltoDom(QString xmlText)
        {
            QDomDocument doc; doc.setContent(xmlText.toUtf8());

            QDomElement  docElem = doc.documentElement();

            return docElem;
        }

     //dom遍历xml获取影片信息
        void listDom(QDomElement docElem)
        {
            QDomNode node = docElem.firstChild();

            //if(node.toElement().isNull()){}

            while(!node.isNull())
            {
                QDomElement element = node.toElement();

                if( !element.isNull() )
                {
                    if(element.tagName()=="id"){

                           this->vid.append(element.text());

                      }else if(element.tagName()=="name"){

                           this->vname.append(element.text());

                     }else if(element.tagName()=="dd"){
                           this->vurl.append(element.text());

                      }else if(element.tagName()=="des"){

                          this->vdes=element.text();

                     }else if(element.tagName()=="ty"){

                          this->type.insert(element.attribute("id"), element.text());

                    }
                      listDom(element);
                }

                node = node.nextSibling();
            }

            return;
        }



        //dom遍历xml获取影片信息
           void videoDom(QDomElement docElem)
           {
               QDomNode node = docElem.firstChild();

               //if(node.toElement().isNull()){}

               while(!node.isNull())
               {
                   QDomElement element = node.toElement();

                   if( !element.isNull() )
                   {
                       if(element.tagName()=="list"){

                            this->type.insert(element.attribute("page"), element.text());



                              this->vid.append(element.text());

                         }else if(element.tagName()=="name"){

                              this->vname.append(element.text());

                        }else if(element.tagName()=="dd"){
                              this->vurl.append(element.text());

                         }else if(element.tagName()=="des"){

                             this->vdes=element.text();

                        }else if(element.tagName()=="ty"){

                             this->type.insert(element.attribute("id"), element.text());

                       }
                         videoDom(element);
                   }

                   node = node.nextSibling();
               }

               return;
           }









       //DOM遍历方式搜索显示影片信息
       void  getvideo(int type,const QString api,const QString word="", const QString page=""){
         QString  url,done;
         switch (type) {

          case 1 :

             url=api+"?wd="+word;this->vid.clear();this->vname.clear();
             break;

         case 2:

             url=api+"?ac=videolist&ids="+word;;this->vurl.clear();this->vdes.clear();
             break;

          case 3:
             url=api;this->type.clear();
             break;

         case 4:

              url=api+"?ac=videolist&t="+word+"&pg="+page;
              break;

         }
         done=UrlRequestGet(url);
         listDom(xmltoDom(done));
        }

        //本地编码转换为Unicode
         QString toUnicode(QString text){
            QTextCodec *codec = QTextCodec::codecForLocale();
            char* ch; QByteArray ba = text.toLatin1(); ch=ba.data();
            return codec->toUnicode(ch);
        }


    //取所有资源类型
       void getclass(QTreeView * tree,const QString pfile){

           QFile file(pfile);

            if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
                model = new QStandardItemModel(tree);//创建模型
                tree->setModel(model);
                model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("资源列表"));
               // tree->setColumnWidth(0,200);
                urls.clear();
                for(int i=0;!file.atEnd();i++){
                     QByteArray line = file.readLine().trimmed();
                     QString str(line);
                     QStringList list =str.split(",");
                     //添加到变量
                     urls.insert(list[0],list[1]);
                     //添加到树形框
                     model->setItem(i,0,new QStandardItem(list[0]));
                     //取分类信息
                     getvideo(3,list[1],"");

                     QMap<QString,QString>::iterator it;
                     for ( it =type.begin(); it != type.end(); ++it)
                     {
                          if(it.key().size()==1){
                              model->item(i)->appendRow(new QStandardItem("0"+it.key()+" "+it.value()));
                          }else{
                              model->item(i)->appendRow(new QStandardItem(it.key()+" "+it.value()));
                          }
                      }
                }
                        file.close();
             }
         }


};
#endif // MAINWINDOW_H
