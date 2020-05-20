#include "config.h"
#include <QtCore/QtCore>
#include <QDebug>

Config::Config(QString qstrfilename)
{
    if (qstrfilename.isEmpty())
    {
        m_qstrFileName = QDir::currentPath()+ "/Config.ini";
    }
    else
    {
        m_qstrFileName = qstrfilename;
    }

    m_psetting = new QSettings(m_qstrFileName, QSettings::IniFormat);
    qDebug() << m_qstrFileName;
}
Config::~Config()
{
    delete m_psetting;
    m_psetting = 0;
}
void Config::set(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue)
{
    m_psetting->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
}

QVariant Config::get(QString qstrnodename,QString qstrkeyname)
{
    QVariant qvar = m_psetting->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    return qvar;
}


void Config::setValue(QString keypath,QVariant qvarvalue)
{
    m_psetting->setValue(keypath, qvarvalue);
}

QVariant Config::getValue(QString keypath)
{

    return  m_psetting->value(keypath);
}

QStringList Config::allKeys(){

    return m_psetting->allKeys();
}

QStringList Config::getKeys(QString key){

    QStringList list,keys;
    list= m_psetting->allKeys();

   for(int i=0;i<list.size();i++){

       if(list.value(i).trimmed().indexOf(QRegExp("(?:^|/)"+key+"(?:$|/)"))!=-1){
           keys.append(list.value(i));
       }

   }

  return keys;


}





void Config::remove(QString keypath){

    m_psetting->remove(keypath);

}
bool Config::iskey(QString qstrnodename,QString qstrkeyname)
{
   return m_psetting->contains(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
}

void Config::clear()
{
    m_psetting->clear();
}








