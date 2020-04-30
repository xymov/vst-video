#ifndef LOADING_H
#define LOADING_H

#include <QWidget>

#include <QDesktopWidget>
#include <QMovie>

namespace Ui {
class loading;
}

class loading : public QWidget
{
    Q_OBJECT

public:
    explicit loading(QWidget *parent = nullptr);
    ~loading();

 private slots:

    //void quit();

private:
    Ui::loading *ui;
};

#endif // LOADING_H
