#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::initTheme()
{
    // 主题
    int theme = config.get("set", "theme").toInt();
    switch (theme)
    {
    default:
        ui->action_theme_0->setChecked(true);
        break;
    case 1:
        ui->action_theme_1->setChecked(true);
        this->setStyleSheet("QWidget{background-color:#606060;}");
        break;
    case 2:
        ui->action_theme_2->setChecked(true);
        this->setStyleSheet("QWidget{background-color:#f0f0f0;}");
        break;
    }
}

/*  主题切换     */
void MainWindow::on_action_theme_1_triggered()
{
    this->setStyleSheet("QWidget{background-color:#606060;}");
    config.set("set", "theme", 1);
    ui->action_theme_0->setChecked(false);
    ui->action_theme_2->setChecked(false);
}

void MainWindow::on_action_theme_2_triggered()
{
    this->setStyleSheet("QWidget{background-color:#f0f0f0;}");
    config.set("set", "theme", 2);
    ui->action_theme_0->setChecked(false);
    ui->action_theme_1->setChecked(false);
}

void MainWindow::on_action_theme_0_triggered()
{
    this->setStyleSheet("");
    config.set("set", "theme", 0);
    ui->action_theme_1->setChecked(false);
    ui->action_theme_2->setChecked(false);
}
