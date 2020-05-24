#ifndef SET_H
#define SET_H

#include <QWidget>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>

namespace Ui
{
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

    void quit();

private:
    Ui::set *ui;
};

#endif // SET_H
