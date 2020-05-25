#include "types.h"
#include "utilities.h"
#include "webresource.h"

#include <QCryptographicHash>
#include <QDomDocument>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QTextCodec>
#include <QTimer>

// 取网页数据
QString UrlRequestGet(const QString url)
{
    // 异常处理
    try
    {
        QNetworkAccessManager qnam;
        const QUrl aurl(url);
        QNetworkRequest qnr(aurl);
        qnr.setRawHeader("Content-Type", "application/json");
        QNetworkReply *reply = qnam.get(qnr);

        QEventLoop eventloop;
        QTimer::singleShot(20000, &eventloop, SLOT(quit())); // 超时 20 秒
        QObject::connect(reply, SIGNAL(finished()), &eventloop, SLOT(quit()));
        eventloop.exec(QEventLoop::ExcludeUserInputEvents);

        QTextCodec *codec = QTextCodec::codecForName("utf8");
        QString replyData = codec->toUnicode(reply->readAll());

        reply->deleteLater();
        reply = nullptr;

        return replyData;
    }
    catch (int n)
    {
        qDebug() << "num=" << n << ",UrlRequestGet() error!" << endl;
        return "";
    }
}

// 下载图片文件
void UrlRequestImg(const QString url, const QString key)
{
    // 异常处理
    try
    {
        QString filename = app.cache + key + ".jpg";

        if (!isFileExists(filename))
        {
            QNetworkAccessManager qnam;
            QNetworkReply *reply = qnam.get(QNetworkRequest(QUrl(url)));

            QEventLoop eventloop;
            QTimer::singleShot(20000, &eventloop, SLOT(quit()));
            QObject::connect(reply, SIGNAL(finished()), &eventloop, SLOT(quit()));
            eventloop.exec(QEventLoop::ExcludeUserInputEvents);

            QPixmap currentPicture;
            currentPicture.loadFromData(reply->readAll());
            currentPicture.save(filename); // 保存图片

            reply->deleteLater();
            reply = nullptr;
        }
    }
    catch (int n)
    {
        qDebug() << "num=" << n << ",UrlRequestImg() error!" << endl;
    }
}

// 提交网页数据
QString UrlRequestPost(const QString url, const QString data)
{
    QNetworkAccessManager qnam;
    const QUrl aurl(url);
    QNetworkRequest qnr(aurl);
    qnr.setRawHeader("Content-Type", "application/json;charset=GBK");
    QNetworkReply *reply = qnam.post(qnr, data.toLocal8Bit());

    QEventLoop eventloop;
    QTimer::singleShot(20000, &eventloop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &eventloop, SLOT(quit()));
    eventloop.exec(QEventLoop::ExcludeUserInputEvents);

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QString replyData = codec->toUnicode(reply->readAll());

    reply->deleteLater();
    reply = nullptr;

    return replyData;
}

// xml 文本转 dom 对象
QDomElement xmltoDom(QString xmlText)
{
    QDomDocument doc;
    doc.setContent(xmlText.toUtf8());

    QDomElement docElem = doc.documentElement();

    return docElem;
}

// dom 遍历 xml 获取影片信息
void listDom(QDomElement docElem, VideoInfo &cInfo)
{
    QDomNode node = docElem.firstChild();
    //if(node.toElement().isNull()){}
    while (!node.isNull())
    {
        QDomElement element = node.toElement();
        if (!element.isNull())
        {
            // 页面信息
            if (element.tagName() == "list")
            {
                cInfo.page = element.attribute("page");
                cInfo.pagecount = element.attribute("pagecount");
                cInfo.pagesize = element.attribute("pagesize");
                cInfo.recordcount = element.attribute("recordcount");
            }
            else if (element.tagName() == "last")
            {
                cInfo.last << element.text(); // 日期
            }
            else if (element.tagName() == "id")
            {
                cInfo.id << element.text(); // 影片 id
            }
            else if (element.tagName() == "tid")
            {
                cInfo.tid << element.text(); // 分类 ID
            }
            else if (element.tagName() == "name")
            {
                cInfo.name << element.text(); // 影片名称
            }
            else if (element.tagName() == "type")
            {
                cInfo.tname << element.text(); // 名称分类
            }
            else if (element.tagName() == "dd")
            {
                cInfo.video << element.text(); // 影片数据
            }
            else if (element.tagName() == "pic")
            {
                cInfo.pic << element.text(); // 影片图片
            }
            else if (element.tagName() == "lang")
            {
                cInfo.lang << element.text(); // 语言
            }
            else if (element.tagName() == "area")
            {
                cInfo.area << element.text(); // 地区
            }
            else if (element.tagName() == "year")
            {
                cInfo.year << element.text(); //年份
            }
            else if (element.tagName() == "state")
            {
                cInfo.state << element.text(); // 状态
            }
            else if (element.tagName() == "note")
            {
                cInfo.note << element.text(); // 标签
            }
            else if (element.tagName() == "des")
            {
                cInfo.des << element.text(); // 简介
            }
            else if (element.tagName() == "ty")
            {
                //分类信息
                Nameinfo name;
                name.id = element.attribute("id");
                name.name = element.text();
                cInfo.type << name;
            }
            listDom(element, cInfo);
        }

        node = node.nextSibling();
    }
}

// DOM 遍历方式搜索显示影片信息
void getvideo(int tp, const QString api, const QString word, const QString page)
{
    QString url, done;
    vBrowse.clear();
    switch (tp)
    {
    case 1:
        url = api + "?wd=" + word + "&pg=" + page;
        break;
    case 2:
        url = api + "?ac=videolist&ids=" + word;
        break;
    case 3:
        url = api;
        break;
    case 4:
        url = api + "?ac=videolist&t=" + word + "&pg=" + page;
        break;
    }

    done = UrlRequestGet(url);

    listDom(xmltoDom(done), vBrowse);
    vBrowse.api = api;
}

// DOM 遍历方式搜索显示影片信息
void get_detail_videoinfo_by_id(const QString api, const QString word)
{
    vDetail.clear();

    QString url = api + "?ac=videolist&ids=" + word;

    QString done = UrlRequestGet(url);

    listDom(xmltoDom(done), vDetail);

    vDetail.api = api;
}

// 取所有资源类型
void getclass(const QString pfile)
{
    //QQueue<Nameinfo>type;

    QFile file(pfile);
    sources.clear();
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        for (int i = 0; !file.atEnd(); i++)
        {
            QByteArray line = file.readLine().trimmed();
            QString str(line);
            str = str.toUtf8();
            if (str.isEmpty())
                continue;

            QStringList list = str.split(",");
            SourceInfo info;
            info.name = list.value(0);
            info.api = list.value(1);

            getvideo(3, info.api);

            info.type = vBrowse.type;

            sources.insert(i, info);
        }
        file.close();
    }
}

// 取直播
void getlive(const QString pfile)
{
    QFile file(pfile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        SourceInfo info;
        info.name = "直播列表";
        for (int i = 0; !file.atEnd(); i++)
        {
            QByteArray line = file.readLine().trimmed();
            QString str(line);
            str = str.toUtf8();
            if (str != "")
            {
                QStringList list = str.split(",");
                Nameinfo var;
                var.name = list.value(0);
                var.id = list.value(1);
                info.type.insert(i, var);
            }
        }
        file.close();

        if (info.type.size() > 0)
        {
            sources.append(info);
        }
    }
}

// 搜索资源站
void search(QString searchword, int ctype)
{
    QString done, url, api;
    vSearch.clear();
    if (ctype == 0)
    {
        foreach (SourceInfo it, sources)
        {
            VideoInfo cInfo; // 重要,清空数据
            url = it.api + "?wd=" + searchword;
            done = UrlRequestGet(url);
            listDom(xmltoDom(done), cInfo);
            cInfo.sname = it.name;
            cInfo.api = it.api;
            vSearch.append(cInfo);
        }
    }
    else
    {
        VideoInfo cInfo;
        api = sources.value(ctype - 1).api;
        url = api + "?wd=" + searchword;
        done = UrlRequestGet(url);
        listDom(xmltoDom(done), cInfo);
        cInfo.sname = sources.value(ctype - 1).name;
        cInfo.api = api;
        vSearch.append(cInfo);
    }
}

// 组合简介信息
QString todes(VideoInfo cInfo, int index)
{
    QString str = QString("<h3>%1</h3><h4>%2 %3 %4 %5 %6 %7</h4><p>%8</p>")
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

// 调用默认程序打开 M3U8 格式
void OpenM3u8(QString url)
{
    QFileInfo fi(url);
    QString tmp = "/tmp/" + fi.fileName();
    QFile file(tmp);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(localtom3u8(url).toUtf8());
        file.close();
    }
    open(tmp);
}

// 本地化 m3u8,返回内容
QString localtom3u8(const QString url)
{
    QString done = UrlRequestGet(url);
    QStringList lists = done.split("\n");
    QString m3u8 = "";

    foreach (QString list, lists)
    {
        // 判断是文件信息
        if (list != "" && list.mid(0, 1) != "#")
        {
            m3u8 += put_url(url, list.trimmed());
        }
        else
        {
            m3u8 += list.trimmed() + "\n";
        }
    }
    qDebug() << m3u8;
    return m3u8;
}

QString topic(QString api, QString id)
{
    return app.cache + toHash(api) + "_" + id + ".jpg";
}
