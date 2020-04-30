#ifndef LOADING_H
#define LOADING_H

#include <QWidget>

namespace Ui {
class loading;
}

class loading : public QWidget
{
    Q_OBJECT

public:
    explicit loading(QWidget *parent = nullptr);
    ~loading();

private:
    Ui::loading *ui;
};

#endif // LOADING_H
