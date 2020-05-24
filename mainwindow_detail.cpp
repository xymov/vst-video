#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "webresource.h"

#include <QCompleter>
#include <QtConcurrent>

// 影片名被改变
void MainWindow::on_comboBox_name_currentIndexChanged(int index)
{
    if (index == -1)
        return;

    // 取关联数据
    QString word = ui->comboBox_name->itemData(index).toString();
    // 0=api 1=id
    QStringList v = word.split("|");
    QString api = v.value(0);
    QString id = v.value(1);

    // 检查是否一致
    if (vDetail.id.value(index).toInt() == id.toInt())
    {
        // 图片
        QString file = topic(api, id);
        if (!isFileExists(file))
        {
            file = app.nopic;
        }

        QPixmap pixmap(file);
        ui->info_pic->setPixmap(pixmap);
        ui->info_pic->setAlignment(Qt::AlignCenter); // 图片居中

        // 分集
        loadMedia();

        // 介绍
        ui->info_des->setHtml(todes(vDetail, index));        
    }
    else
    {
        QtConcurrent::run(this, &MainWindow::requestDetailVideoInfo, word);
    }
}

void MainWindow::requestDetailVideoInfo(QString word)
{
    QMutexLocker locker(&_detail_locker);

    // 获取浏览影片数据
    QStringList v = word.split("|");
    if(v.size() != 2)
        return;

    QString api = v.value(0);
    QString id = v.value(1);

    get_detail_videoinfo_by_id(api, id);

    QMetaObject::invokeMethod(this, "endRequestDetailVideoInfo", Q_ARG(QString, word));
}

// 线程搜索影片详情结束
void MainWindow::endRequestDetailVideoInfo(QString word)
{
    Q_UNUSED(word)

    ui->comboBox_part->clear();
    ui->info_des->clear();
    ui->detail_partlist->clear();

    if(vDetail.video.isEmpty())
        return;

    // 图片
    QString file = topic(vDetail.api, vDetail.id.last());
    if (isFileExists(file))
    {
        QPixmap pixmap(file);
        ui->info_pic->setPixmap(pixmap);
        ui->info_pic->setAlignment(Qt::AlignCenter); // 图片居中
    }
    else
    {
        QtConcurrent::run(this, &MainWindow::requestDetailImage, vDetail.pic.value(0) + "|" + vDetail.api + "|" + vDetail.id.value(0));
    }

    // 分集
    loadMedia();

    // 介绍
    ui->info_des->setHtml(todes(vDetail, vDetail.id.length() - 1));

    show_loading(false);
}

void MainWindow::requestDetailImage(QString word)
{
    if (word.isEmpty())
        return;

    // 下载搜索图片
    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    QMutexLocker locker(&_detail_locker);

    UrlRequestImg(v.value(0), toHash(v.value(1)) + "_" + v.value(2));

    QMetaObject::invokeMethod(this, "endRequestDetailImage", Q_ARG(QString, word));
}

// 线程搜索下载图片结束
void MainWindow::endRequestDetailImage(QString word)
{
    // 取关联数据
    QStringList v = word.split("|");

    // 设置预览图片
    QString file = topic(v.value(1), v.value(2));
    if (!isFileExists(file))
    {
        file = app.nopic;
    }

    QPixmap pixmap(file);
    ui->info_pic->setPixmap(pixmap);
    ui->info_pic->setAlignment(Qt::AlignCenter); // 图片居中    
}

// 根据列表序号加载媒体
void MainWindow::loadMedia(int index)
{
    ui->comboBox_part->clear();
    ui->detail_partlist->clear();

    QStringList list = vDetail.video.last().split("#");
    foreach (QString s, list)
    {
        // 第30集$https://index.m3u8$ckm3u8

        QStringList v = s.trimmed().split("$");
        if (v.size() == 1)
        {
            ui->comboBox_part->addItem("高清", v.value(0));
        }
        else if (v.size() == 2)
        {
            ui->comboBox_part->addItem("高清", v.value(0));
        }
        else if (v.size() == 3)
        {
            ui->comboBox_part->addItem(v.value(0), v.value(1));

            // 右侧分集列表
            QListWidgetItem *item = new QListWidgetItem(v.value(0));
            item->setSizeHint(QSize(120, 40));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui->detail_partlist->addItem(item);
        }
    }

    if(ui->comboBox_part->count() > 0 && index < ui->comboBox_part->count())
    {
        ui->comboBox_part->setCurrentIndex(index);
        ui->detail_partlist->setCurrentRow(index);
    }
}

// 剧集被改变
void MainWindow::on_comboBox_part_currentIndexChanged(int index)
{
    if (index > 0)
    {
        playlist->setCurrentIndex(index);

        // 由于 playlist 的 currentIndexChanged 不起作用，这里直接设置
        ui->player_control_part->setCurrentIndex(index);

        ui->detail_partlist->setCurrentRow(index);
    }
}

// 进入播放选项卡并播放视频
void MainWindow::on_info_play_clicked()
{
    show_loading(false);

    if (ui->comboBox_part->count() == 0)
        return;

    app.live = false;
    loadPlay(ui->comboBox_part->currentIndex());

    ui->label->show();
    ui->label_VideoName->show();
    ui->player_control_part->show();
    ui->tabWidget->setCurrentWidget(ui->tab_player);
}

// 加载播放
void MainWindow::loadPlay(int index, qint64 time)
{
    ui->label_VideoName->setText(QString::asprintf("<a href='https://github.com/xymov/vst-video/'>%s", ui->comboBox_name->currentText().toLocal8Bit().data()));

    video->setUpdatesEnabled(true);

    ui->sliderProgress->setEnabled(true);

    player->stop();
    playlist->clear();

    for (int i = 0; i < ui->comboBox_part->count(); i++)
    {
        // 添加到播放列表
        playlist->addMedia(QUrl(ui->comboBox_part->itemData(i).toString()));

        // 添加到播放器控制板列表
        ui->player_control_part->insertItem(i, ui->comboBox_part->itemText(i));
    }

    // 设置分集输入自动选择
    QCompleter *completer = new QCompleter(ui->player_control_part->model(), this);
    completer->setFilterMode(Qt::MatchContains);
    ui->player_control_part->setCompleter(completer);

    // 选择播放内容和位置
    playlist->setCurrentIndex(index);
    ui->player_control_part->setCurrentIndex(index);

    player->setPosition(time);

    // 开始播放
    player->play();
}

// 上一个视频
void MainWindow::on_info_front_clicked()
{
    int index = ui->comboBox_name->currentIndex();

    if (index == 0)
    {
        player->stop();
        ui->comboBox_name->setCurrentIndex(index - 1);
        loadPlay();
    }
}

// 下一个视频
void MainWindow::on_info_next_clicked()
{
    int index = ui->comboBox_name->currentIndex();

    if (index + 1 < ui->comboBox_name->count())
    {
        player->stop();
        ui->comboBox_name->setCurrentIndex(index + 1);
        loadPlay();
    }
}

void MainWindow::on_detail_partlist_itemClicked(QListWidgetItem *item)
{
    ui->comboBox_part->setCurrentText(item->text());
}

void MainWindow::on_detail_partlist_itemDoubleClicked(QListWidgetItem *item)
{
    ui->comboBox_part->setCurrentText(item->text());
    on_info_play_clicked();
}
