#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "webresource.h"

#include <QtConcurrent>

void MainWindow::initRecord()
{
    // 播放记录
    renotes();

    connect(ui->menu_notes, SIGNAL(triggered(QAction *)), this, SLOT(menu_action_notes_triggered(QAction *)));
}

// 加载播放记录
void MainWindow::renotes()
{
    ui->menu_notes->clear();

    QStringList keys = config.getKeys("notes");
    if (keys.size() == 0)
        return;

    QStringList list;
    QString key;
    for (int i = 0; i < keys.size(); i++)
    {
        //【片名】分集名称|pic|api|id|part|time
        key = config.getValue(keys.value(i)).toString();
        list = key.split("|");

        QAction *test = new QAction(list.value(0), this);
        test->setData(list);
        ui->menu_notes->addAction(test);
    }

    // 添加清除按钮
    ui->menu_notes->addSeparator();

    QAction *test = new QAction("清空记录", this);
    test->setData("clear");
    ui->menu_notes->addAction(test);
}

// 播放记录被选择
void MainWindow::menu_action_notes_triggered(QAction *action)
{
    if (action->data().toString() == "clear")
    {
        config.remove("notes");
        renotes();
        return;
    }

    ui->comboBox_name->clear();
    ui->comboBox_part->clear();
    ui->info_des->clear();

    // 添加所有播放记录
    QString name, api, id;
    QStringList v;
    foreach(QAction *sub, ui->menu_notes->actions())
    {
        //【片名】分集名称|api|id|part|time
        v = sub->data().toList().value(0).toString().split("】");
        name = v.first().mid(1);
        api = sub->data().toList().value(1).toString();
        id = sub->data().toList().value(2).toString();

        ui->comboBox_name->addItem(name, api + "|" + id);
    }

    // 设置当前播放记录
    v = action->data().toList().value(0).toString().split("】");
    name = v.first().mid(1);
    QString part = action->data().toList().value(3).toString();
    QString time = action->data().toList().value(4).toString();

    show_loading(true);
    app.live = false;

    ui->comboBox_name->setCurrentText(name);

    show_loading(true);

    QtConcurrent::run(this, &MainWindow::requestDetailPlay, part.toInt(), time.toInt());
}

// 写播放记录
void MainWindow::MinWriteNotes(int part)
{
    if (ui->comboBox_name->currentIndex() == -1)
        return;

    Notesinfo note;
    note.title = QString("【%1】%2").arg(ui->comboBox_name->currentText()).arg(ui->comboBox_part->currentText());

    // api|id
    note.api = ui->comboBox_name->itemData(ui->comboBox_name->currentIndex()).toString();
    note.part = QString::number(part);
    note.time = QString::number(player->position());

    QString str = QString("%1|%2|%3|%4").arg(note.title).arg(note.api).arg(note.part).arg(note.time);

    config.set("notes", toHash(note.api), str);
}
