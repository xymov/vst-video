#ifndef TYPES_H
#define TYPES_H

#include <QDebug>
#include <QDir>
#include <QList>
#include <QQueue>

// 运行信息
typedef struct Appinfo
{
    bool playlist;

    QString sourcePath;

    QString livePath;

    QString cache;

    QString nopic;

    QString notes;

    QStringList arguments;

    Qt::WindowFlags Flags;

    bool live;

    bool liveload;

    Qt::WindowStates windowState;

    QString basePath;

    Appinfo()
    {
        basePath = QDir::homePath() + "/.vst-video";

        QDir dir(basePath);
        if(!dir.exists())
            dir.mkpath(basePath);

        sourcePath = basePath + "/source.txt";
        livePath = basePath + "/live.txt";
        notes = basePath + "/notes.txt";
        cache = basePath + "/cache/";
        nopic = "://resource/img/timg.jpeg";

        live = false;
    }

} Appinfo;
Q_DECLARE_METATYPE(Appinfo);

// 名称信息
typedef struct Nameinfo
{
    QString id;
    QString name;
} Nameinfo;
Q_DECLARE_METATYPE(Nameinfo);

// 名称信息
typedef struct Notesinfo
{
    QString title;
    QString api;
    QString id;
    QString part;
    QString time;
} Notesinfo;
Q_DECLARE_METATYPE(Notesinfo);

// 资源信息
typedef struct SourceInfo
{
    QString name;          //资源名称
    QString api;           //接口地址
    QQueue<Nameinfo> type; //分类信息
} SourceInfo;
Q_DECLARE_METATYPE(SourceInfo);

// 视频信息
typedef struct VideoInfo
{
    QString sname;           //接口名称
    QString api;             //接口地址
    QString page;            //当前页数
    QString pagecount;       //总页数
    QString pagesize;        //页视频数量
    QString recordcount;     //总数量
    QList<QString> last;     //日期
    QList<QString> id;       //id
    QList<QString> tid;      //分类ID
    QList<QString> name;     //视频名称
    QList<QString> tname;    //分类名称
    QList<QString> pic;      //视频图片
    QList<QString> lang;     //语言
    QList<QString> area;     //地区
    QList<QString> year;     //年份
    QList<QString> note;     //视频标签
    QList<QString> actor;    //演员
    QList<QString> state;    //状态
    QList<QString> director; //导演
    QList<QString> video;    //视频信息 第01集$index.m3u8$ckm3u8#第02集$index.m3u8$ckm3u8#第03集$index.m3u8$ckm3u8
    QList<QString> des;      //视频简介
    QQueue<Nameinfo> type;   //分类信息

    void clear()
    {
        api.clear();
        page.clear();
        pagecount.clear();
        pagesize.clear();
        recordcount.clear();
        type.clear();
        last.clear();
        id.clear();
        tid.clear();
        name.clear();
        tname.clear();
        pic.clear();
        video.clear();
        des.clear();
        lang.clear();
        area.clear();
        year.clear();
        note.clear();
        actor.clear();
        state.clear();
        director.clear();
    }

    /*
       operator QVariant() const
      {
             return QVariant::fromValue(*this);

    */

} VideoInfo;
Q_DECLARE_METATYPE(VideoInfo);

// 当前运行信息
extern Appinfo app;

// 资源分类信息
// QMap<QString,SourceInfo>type;
extern QQueue<SourceInfo> sources;

// 当前搜索资源信息
extern QQueue<VideoInfo> vSearch;

// 当前搜索列表中的单个资源（需要事先获取）或浏览分类列表中的单页资源信息
extern VideoInfo vBrowse;

// 详情中的当前资源信息
extern VideoInfo vDetail;

#endif // TYPES_H
