#include "loading.h"
#include "ui_loading.h"

loading::loading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loading)
{
    ui->setupUi(this);
}

loading::~loading()
{
    delete ui;
}
