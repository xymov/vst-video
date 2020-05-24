#ifndef CONFIG_H
#define CONFIG_H

#include <QVariant>
#include <QSettings>

class Config
{
public:
    Config(QString qstrfilename = "");
    virtual ~Config(void);
    QStringList allKeys();
    QStringList getKeys(QString);
    void clear();
    void remove(QString);
    bool iskey(QString, QString);
    void set(QString, QString, QVariant);
    QVariant get(QString, QString);
    void setValue(QString, QVariant);
    QVariant getValue(QString);

private:
    QString m_qstrFileName;
    QSettings *m_psetting;
};

#endif // CONFIG_H
