#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "loading.h"




//QMediaPlayer
#include <QMediaPlayer> //add
#include <QMediaPlaylist>
#include <QVideoWidget>


//QtNetwork
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QTextCodec>

//log
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDateTime>

//xml
#include <QXmlStreamReader>
#include <QDomDocument>

//QThread
#include <QThread>
#include <QStandardItemModel>


#include <QQueue>
#include <QTimer>
#include <QMutex>



#include <QEvent>
#include <QTime>
#include <QTextEdit>
#include <QTreeView>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QListWidget>



#include <QDesktopWidget>
#include <QMessageBox>
#include <QShortcut>
#include <QtConcurrent>
#include <QListWidgetItem>
#include <QScrollBar>


//运行信息
typedef struct Appinfo
{
   bool  playlist;

   QString sourcePath;

   QString livePath;

   QString cache;

   QString nopic;

   bool live;


   Qt::WindowStates windowState;

}Appinfo;
Q_DECLARE_METATYPE(Appinfo);



//名称信息
typedef struct Nameinfo
{
    QString id;
    QString name;
}Nameinfo;
Q_DECLARE_METATYPE(Nameinfo);




//资源信息
typedef struct SourceInfo
{
    QString name;                //资源名称
    QString api;                 //接口地址
    QQueue<Nameinfo>type;       //分类信息
}SourceInfo;
Q_DECLARE_METATYPE(SourceInfo);



//视频信息
typedef struct VideoInfo
{
    QString sname;       //接口名称
    QString api;         //接口地址
    QString page;        //当前页数
    QString pagecount;   //总页数
    QString pagesize;    //页视频数量
    QString recordcount;  //总数量
    QList<QString>last;     //日期
    QList<QString>id;    //id
    QList<QString>tid;    //分类ID
    QList<QString>name;  //视频名称
    QList<QString>tname;  //分类名称
    QList<QString>pic;  //视频图片
    QList<QString>lang;  //语言
    QList<QString>area;  //地区
    QList<QString>year;  //年份
    QList<QString>note;  //视频标签
    QList<QString>actor;  //演员
    QList<QString>state;  //状态
    QList<QString>director;  //导演
    QList<QString>video;    //视频信息 第01集$index.m3u8$ckm3u8#第02集$index.m3u8$ckm3u8#第03集$index.m3u8$ckm3u8
    QList<QString>des;      //视频简介
    QQueue<Nameinfo>type;   //分类信息
   void clear(){
       api.clear();page.clear();pagecount.clear();pagesize.clear();recordcount.clear(); type.clear();
       last.clear();id.clear();tid.clear();name.clear();tname.clear();pic.clear();video.clear();des.clear();
       lang.clear();area.clear();year.clear();note.clear(); actor.clear();state.clear();director.clear();
    }

    /*
       operator QVariant() const
      {
             return QVariant::fromValue(*this);

    */

}VideoInfo;
Q_DECLARE_METATYPE(VideoInfo);


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

     void init();

     void on_listWidget_currentRowChanged(int currentRow);

     void on_page_front_clicked();

     void on_page_next_clicked();

     void on_page_jmp_clicked();

     void getpageinfo (int page);

     void loadMedia(int);

     void on_info_play_clicked();

     void on_info_front_clicked();

     void on_info_next_clicked();

     void on_search_ok_clicked();

     void on_search_list_pressed(const QModelIndex &index);

     void on_tabWidget_currentChanged(int index);

     void on_source_re_clicked();

     void on_search_source_currentIndexChanged(int index);



signals:
     void quit();

private:

    Ui::MainWindow *ui;


    void  createSource();

    bool eventFilter(QObject *target, QEvent *event);
    void ThreadFunc(int,QString);
    QStandardItemModel *student_model;
    QString STimeDuration="00:00:00";
    QMediaPlaylist *playlist;
    QMediaPlayer *player;
    QVideoWidget *video;
    void setSTime(qint64);
    void echoload(bool);
    QString API;
    loading load;
    QTimer *m_timer;
    void createListWidget(QListWidget *listWidget,int key,bool insert);

    void  switchFullScreen(bool);

    void  loadPlay(bool play);

    //运行信息
    Appinfo set;


    //影片信息

     QStandardItemModel * model;

     //资源分类信息

    // QMap<QString,SourceInfo>type;

     QQueue<SourceInfo>type;


     //搜索资源信息

     QQueue<VideoInfo>vSearch;

     //播放资源信息
     VideoInfo  vInfo;

     void initListWidget(QListWidget *listWidget);


      //取网页数据
      QString UrlRequestGet( const QString url )
        {

          //异常处理

          try
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

          }catch(int n){

              qDebug()<<"num="<<n<<",UrlRequestGet() error!"<<endl;
              return "";

          }

       }

      //下载图片文件
    void UrlRequestImg(const QString url,const QString key)
        {
        //异常处理

        try
            {
             QString filename ="./cache/"+key+".jpg";

          if(!isFileExist(filename)){

            QNetworkAccessManager qnam;
            QNetworkReply *reply=qnam.get(QNetworkRequest(QUrl(url)));
            QEventLoop eventloop;
            connect( reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
            eventloop.exec( QEventLoop::ExcludeUserInputEvents);
            QPixmap currentPicture;
            currentPicture.loadFromData(reply->readAll());
            currentPicture.save(filename);//保存图片

            reply->deleteLater();
            reply = nullptr;

            }

        }catch(int n){

            qDebug()<<"num="<<n<<",UrlRequestImg() error!"<<endl;

        }

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
        void listDom(QDomElement docElem,VideoInfo &cInfo)
        {

            QDomNode node = docElem.firstChild();
            //if(node.toElement().isNull()){}
            while(!node.isNull())
            {
                QDomElement element = node.toElement();
                if( !element.isNull() )
                {
                    //页面信息
                    if(element.tagName()=="list"){
                         cInfo.page=element.attribute("page");
                         cInfo.pagecount=element.attribute("pagecount");
                         cInfo.pagesize=element.attribute("pagesize");
                         cInfo.recordcount=element.attribute("recordcount");
                    }else if(element.tagName()=="last"){cInfo.last<<element.text();     //日期
                    }else if(element.tagName()=="id"){ cInfo.id<<element.text();        //影片id
                    }else if(element.tagName()=="tid"){cInfo.tid<<element.text();       //分类ID
                    }else if(element.tagName()=="name"){cInfo.name<<element.text();     //影片名称
                    }else if(element.tagName()=="type"){cInfo.tname<<element.text();    //名称分类
                    }else if(element.tagName()=="dd"){cInfo.video<<element.text();      //影片数据
                    }else if(element.tagName()=="pic"){cInfo.pic<<element.text();       //影片图片
                    }else if(element.tagName()=="lang"){cInfo.lang<<element.text();      //语言
                    }else if(element.tagName()=="area"){cInfo.area<<element.text();     //地区
                    }else if(element.tagName()=="year"){cInfo.year<<element.text();     //年份
                    }else if(element.tagName()=="state"){cInfo.state<<element.text();   //状态
                    }else if(element.tagName()=="note"){ cInfo.note<<element.text();    //标签
                    }else if(element.tagName()=="des"){cInfo.des<<element.text();       //简介
                    }else if(element.tagName()=="ty"){                                  //分类信息
                        Nameinfo  name;
                        name.id=element.attribute("id");
                        name.name=element.text();
                        cInfo.type << name;
                    }
                      listDom(element,cInfo);
                }

                node = node.nextSibling();
            }

            return;
        }


      //DOM遍历方式搜索显示影片信息
       void  getvideo(int tp,const QString api,const QString word="", const QString page=""){

         QString  url,done;vInfo.clear();
         switch (tp) {
          case 1 :
             url=api+"?wd="+word+"&pg="+page;
             break;
         case 2:
             url=api+"?ac=videolist&ids="+word;
             break;
          case 3:
             url=api;
             break;
         case 4:
              url=api+"?ac=videolist&t="+word+"&pg="+page;
              break;
         }
         done=UrlRequestGet(url);
         listDom(xmltoDom(done),vInfo);vInfo.api=api;
       }

       //本地编码转换为Unicode
        QString toUnicode(QString text){
            QTextCodec *codec = QTextCodec::codecForLocale();
            char* ch; QByteArray ba = text.toLatin1(); ch=ba.data();
            return codec->toUnicode(ch);
    }

    //取所有资源类型
       void getclass(const QString pfile){

           QFile file(pfile);type.clear();
            if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
                for(int i=0;!file.atEnd();i++){
                     QByteArray line = file.readLine().trimmed();
                     QString str(line);
                     if(str!=""){
                     QStringList list =str.split(",");
                     SourceInfo info;info.name=list.value(0);info.api=list.value(1);
                     getvideo(3,info.api);info.type=vInfo.type;type.insert(i,info);
                     }
                }
              file.close();
             }
         }

       //取直播
       void getlive(const QString pfile){
            QFile file(pfile);
              if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
                  SourceInfo info; info.name="直播列表";
                  for(int i=0;!file.atEnd();i++){
                       QByteArray line = file.readLine().trimmed();
                       QString str(line);
                       if(str!=""){
                        QStringList list =str.split(",");
                        Nameinfo var; var.name=list.value(0);var.id=list.value(1);
                        info.type.insert(i,var);;

                        //info.type[i].name=list.value(0);info.type[i].id=list.value(1);
                       }
                  }
                 file.close();

                 if(info.type.size()>0){type.append(info);}
              }
          }

                     //搜索资源站
                     void  search(QString  searchword,int ctype=0){

                         QString done,url,api;vSearch.clear();
                         if(ctype==0){

                             foreach (SourceInfo it, type) {
                                 VideoInfo cInfo;                 //重要,清空数据
                                 url=it.api+"?wd="+searchword;
                                 done=UrlRequestGet(url); listDom(xmltoDom(done),cInfo);
                                 cInfo.sname=it.name;cInfo.api=it.api;
                                 vSearch.append(cInfo);
                             }


                          }else{
                               VideoInfo cInfo;
                               api=type.value(ctype-1).api;
                               url=api+"?wd="+searchword;
                               done=UrlRequestGet(url);listDom(xmltoDom(done),cInfo);
                               cInfo.sname=type.value(ctype-1).name;cInfo.api=api;
                               vSearch.append(cInfo);

                          }
                     }

                    //组合简介信息
                     QString todes(VideoInfo cInfo,int index){
                         QString str =QString("<h3>%1</h3><h4>%2 %3 %4 %5 %6 %7</h4><p>%8</p>")
                                    .arg(cInfo.name.value(index))
                                    .arg(cInfo.year.value(index))
                                    .arg(cInfo.area.value(index))
                                    .arg(cInfo.tname.value(index))
                                    .arg(cInfo.lang.value(index))
                                    .arg(cInfo.director.value(index))
                                    .arg(cInfo.actor.value(index))
                                    .arg(cInfo.des.value(index));
                          return str;
                     }

                    //取文本MD5值
                     QString toHash(const QString pwd){
                         QString md5;
                         QByteArray ba,bb;
                         QCryptographicHash md(QCryptographicHash::Md5);
                         ba.append(pwd);
                         md.addData(ba);
                         bb = md.result();
                         md5.append(bb.toHex());
                         return md5;
                     }


                     /* 判断文件是不是存在 */
                     bool isFileExist(QString fullFileName)
                     {
                         QFileInfo fileInfo(fullFileName);
                         if(fileInfo.isFile())
                         {
                             return true;
                         }
                         return false;
                     }

                     //判断文件夹是否存在,不存在则创建，默认假

                           bool isDirExist(QString fullPath,bool mk=false)
                           {
                             //异常处理
                               try
                          {
                               QDir dir(fullPath);
                               if(dir.exists())
                               {
                                 return true;
                               }
                               else
                               {
                                 if (mk){qDebug()<<dir.mkdir(fullPath);return dir.mkdir(fullPath);}else{return false;}
                               }


                           }catch(int n)

                                  {
                                     qDebug()<<"num="<<n<<",isDirExist() error!"<<endl;

                                      return false;
                                 }

                           }

              };
              #endif // MAINWINDOW_H
