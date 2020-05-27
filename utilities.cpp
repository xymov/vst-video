#include "types.h"
#include "utilities.h"

#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QTextCodec>
#include <QCryptographicHash>

// 写日志
void log(const QString &logFile, const QByteArray &data)
{
    QFile file(logFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    QByteArray ba = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();
    ba += "     " + data + "\n";
    file.write(ba);
    file.close();
}

// 判断文件是不是存在
bool isFileExists(QString fullFileName)
{
    QFileInfo fileInfo(fullFileName);
    return fileInfo.isFile();
}

// 判断文件夹是否存在,不存在则创建，默认假
bool isDirExist(QString fullPath, bool mk)
{
    // 异常处理
    try
    {
        QDir dir(fullPath);
        if (dir.exists())
            return true;

        if (mk)
            return dir.mkdir(fullPath);

        return false;
    }
    catch (int n)
    {
        qDebug() << "num=" << n << ",isDirExist() error!" << endl;

        return false;
    }
}

// 调用默认程序打开命令行
void open(QString url)
{
#ifdef Q_OS_WIN32

    QString m_szHelpDoc = QString("file:///") + url;
    bool is_open = QDesktopServices::openUrl(QUrl(m_szHelpDoc, QUrl::TolerantMode));
    if (!is_open)
    {
        LogWriter::getLogCenter()->PrintLog(LOG_ERROR, "open help doc failed");
        LogWriter::getLogCenter()->SaveFileLog(LOG_ERROR, "open help doc failed");
        return;
    }

#else

    // 在 linux 下，可以通过 system 来 xdg-open 命令调用默认程序打开文件；
    QString cmd = QString("xdg-open ") + url;
    system(cmd.toStdString().c_str());

#endif
}

// 相对路径转绝对路径
QString put_url(QString path, QString url)
{
    if (url.mid(0, 4) == "http")
    {
        return url;
    }
    else if (url.mid(0, 1) == "/")
    {
        QStringList list = path.split("/");
        QString cpath = list.value(0) + list.value(1) + "//" + list.value(2);
        return cpath + url;
    }
    else
    {
        //cpath=path.mid(0,path.size()-fi.fileName().size());
        QFileInfo fi(path);
        return fi.path() + "/" + url;
    }
}

// 本地编码转换为 Unicode
QString toUnicode(QString text)
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    char *ch;
    QByteArray ba = text.toLatin1();
    ch = ba.data();
    return codec->toUnicode(ch);
}

// 取文本 MD5 值
QString toHash(const QString pwd)
{
    QString md5;
    QByteArray ba, bb;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(pwd);
    md.addData(ba);
    bb = md.result();
    md5.append(bb.toHex());
    return md5;
}

QString Readfile(const QString pfile)
{
    QString ba;
    QFile file(pfile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ba;

    for (int i = 0; !file.atEnd(); i++)
    {
        QByteArray line = file.readLine();
        QString str(line);
        if (!str.isEmpty())
        {
            ba += str.toUtf8();
        }
    }

    file.close();
    return ba;
}

bool Writefile(const QString pfile, QString data)
{
    QFile file(pfile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    bool ret = file.write(data.toUtf8());

    file.close();
    return ret;
}

// 写播放记录
void WriteNotes(const QString &logFile, const Notesinfo &note)
{
    QFile file(logFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    QByteArray ba = (note.title + "|" + note.api + "|" + note.id + "|" + note.part + "|" + note.time).toLocal8Bit() + "\n";
    file.write(ba);
    file.close();
}
