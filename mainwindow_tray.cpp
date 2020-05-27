#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::initTray()
{
    _tray = new QSystemTrayIcon(this);
    _tray->setIcon(QIcon("://resource/img/icon.png"));
    _tray->setToolTip(this->windowTitle());
    _tray->setContextMenu(ui->menu_tray);

    connect(_tray, &QSystemTrayIcon::activated, this, &MainWindow::activeTray);

    _tray->show();
}

void MainWindow::activeTray(QSystemTrayIcon::ActivationReason reason)
{
    qint64 dt = QDateTime::currentMSecsSinceEpoch();

    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        this->setVisible(!this->isVisible());

        if (!this->isVisible())
            _tray->showMessage(this->windowTitle(), "主窗口已隐藏，可通过系统托盘区图标恢复", _tray->icon(), 3000);
        break;
    case QSystemTrayIcon::Trigger:
        // 由于双击不起作用，这里用计数器模拟双击。300 毫秒内的两次点击表示双击
        if (dt - _trayClickCounter < 300)
            _tray->activated(QSystemTrayIcon::DoubleClick);

        _trayClickCounter = dt;
        break;
    case QSystemTrayIcon::Context:
        if (_tray->contextMenu() != nullptr)
            _tray->contextMenu()->exec(QCursor::pos());
        break;
    case QSystemTrayIcon::Unknown:
    case QSystemTrayIcon::MiddleClick:
        break;
    }
}

void MainWindow::on_action_tray_exit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_action_tray_show_triggered()
{
    this->show();
}
