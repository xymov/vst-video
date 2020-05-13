#ifndef SET_H
#define SET_H

#include <QWidget>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>

namespace Ui {
class set;
}

class set : public QWidget
{
    Q_OBJECT

public:
    explicit set(QWidget *parent = nullptr);
    ~set();

private slots:
    void on_source_esc_clicked();

    void on_pushButton_live_esc_clicked();

    void on_pushButton_live_ok_clicked();

    void on_source_ok_clicked();

    void reshow();

private:
    Ui::set *ui;


    QString Readfile(const QString pfile)
    {
        QString ba;
        QFile file(pfile);
               if( file.open(QIODevice::ReadOnly|QIODevice::Text) ){

                   for(int i=0;  !file.atEnd();i++){
                        QByteArray line = file.readLine();
                        QString str(line);
                        if(str!=""){
                           ba+=str.toUtf8();
                        }
                   }
                   file.close();
               }
              return ba;
     }



    bool Writefile(const QString pfile,QString data)
    {
        QByteArray ba;


        bool ret=false;
        QFile file(pfile);
               if( file.open(QIODevice::WriteOnly|QIODevice::Text) ){

                   ret=file.write(data.toUtf8());

                   file.close();
               }
              return ret;
     }

};


#endif // SET_H
