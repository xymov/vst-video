#ifndef WEBRESOURCE_H
#define WEBRESOURCE_H

#include "types.h"

#include <QDomDocument>

// 取网页数据
QString UrlRequestGet(const QString url);

// 下载图片文件
void UrlRequestImg(const QString url, const QString key);

// 提交网页数据
QString UrlRequestPost(const QString url, const QString data);

// xml 文本转 dom 对象
QDomElement xmltoDom(QString xmlText);

// dom 遍历 xml 获取影片信息
void listDom(QDomElement docElem, VideoInfo &cInfo);

// DOM 遍历方式搜索显示影片信息
void getvideo(int tp, const QString api, const QString word = "", const QString page = "");

// DOM 遍历方式搜索显示影片信息,用于详情页面
void get_detail_videoinfo_by_id(const QString api, const QString word);

// 取所有资源类型
void getclass(const QString pfile);

// 取直播
void getlive(const QString pfile);

// 搜索资源站
void search(QString searchword, int ctype = 0);

// 组合简介信息
QString todes(VideoInfo cInfo, int index);

// 调用默认程序打开 M3U8 格式
void OpenM3u8(QString url);

// 本地化 m3u8,返回内容
QString localtom3u8(const QString url);

// 图片路径
QString topic(QString api, QString id);

#endif // WEBRESOURCE_H
