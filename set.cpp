#include "set.h"
#include "ui_set.h"

set::set(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::set)
{
    ui->setupUi(this);

    //窗口居中
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);

     setWindowFlags(Qt::WindowStaysOnTopHint |windowFlags());



}

set::~set()
{
    delete ui;
}

void set::quit(){

    close();
}

void set::reshow(){

    ui->source_edit->setPlainText(Readfile("./source.txt"));

    ui->live_edit->setText(Readfile("./live.txt"));

    this->show();

}



void set::on_source_esc_clicked()
{
    close();
}

void set::on_pushButton_live_esc_clicked()
{
    close();
}

void set::on_pushButton_live_ok_clicked()
{
    if(Writefile("./live.txt",ui->live_edit->toPlainText())){
         QMessageBox::information(nullptr, "提示", "保存成功！",QMessageBox::Yes);
    }

}

void set::on_source_ok_clicked()
{
    if(Writefile("./source.txt",ui->source_edit->toPlainText()))
     {
        QMessageBox::information(nullptr, "提示", "保存成功！",QMessageBox::Yes);
      };
}
