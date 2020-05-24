#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "webresource.h"

#include <QtConcurrent>

void MainWindow::initResource()
{
    // 检查资源文件
    if (!isFileExists(app.sourcePath))
    {
        QFile soucre("://resource/source/source.txt");
        soucre.copy(app.sourcePath);
        QFile(app.sourcePath).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }

    if (!isFileExists(app.livePath))
    {
        QFile soucre("://resource/source/live.txt");
        soucre.copy(app.livePath);
        QFile(app.livePath).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }

    // 检查缓存目录
    if (!isDirExist(app.cache, true))
    {
        QMessageBox::warning(nullptr, "提示", "创建缓存目录cache失败，图片显示可能异常，请手工建立!", QMessageBox::Yes);
    }

    // 资源列表
    _tree_model = new QStandardItemModel(ui->tree_source);                     // 创建模型
    _tree_model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("资源列表"));
    // model->setItem(0,0,new QStandardItem("正在刷新.."));
    ui->tree_source->setModel(_tree_model);
    ui->tree_source->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可编辑

    ui->search_source->installEventFilter(this);
    ui->search_name->installEventFilter(this);
}

// 树形框项目被选择
void MainWindow::on_tree_source_pressed(const QModelIndex &index)
{
    if (index.data().toString() == "直播列表" || index.parent().data().toString() == "直播列表")
    {
        //if(playlist->mediaCount()!=type.value(row).type.size())

        if (!app.live)
        {
            ui->info_pic->clear();
            ui->comboBox_name->clear();
            ui->comboBox_part->clear();
            ui->info_des->clear();
            ui->detail_partlist->clear();

            playlist->clear();

            // 添加所有直播资源到播放列表
            foreach (SourceInfo info, sources)
            {
                if(!info.name.contains("直播"))
                    continue;

                foreach (Nameinfo var, info.type)
                {
                    playlist->addMedia(QUrl(var.id));
                }
            }
        }

        if (index.parent().data().toString() == "直播列表")
        {
            playlist->setCurrentIndex(index.row());
        }

        player->play();

        //ui->page_info->setText("频道：" + QString::number(index.row() + 1) + "/" + QString::number(playlist->mediaCount()));

        app.live = true;

        //ui->labelTimeVideo->setText("Live");

        //ui->sliderProgress->setEnabled(false);

        video->setUpdatesEnabled(false);

        ui->label_VideoName->setText(index.data().toString());
        ui->player_control_part->hide();
        ui->label->show();
        ui->label_VideoName->show();

        ui->tabWidget->setCurrentWidget(ui->tab_player);
    }
    else
    {
        explore_view(false);

        getpageinfo(1);

        ui->tabWidget->setCurrentWidget(ui->tab_browse);
    }
}

// 搜索框按了确定键
void MainWindow::on_search_name_returnPressed()
{
    if (ui->search_name->text().trimmed().isEmpty())
    {
        search_model->removeRows(0, search_model->rowCount());
    }
    else
    {
        show_loading(true);

        QtConcurrent::run(this, &MainWindow::requestSearch, ui->search_name->text() + "|" + QString::number(ui->search_source->currentIndex()));

        ui->tabWidget->setCurrentWidget(ui->tab_browse);
        explore_view(true);
    }
}

void MainWindow::requestSearch(QString word)
{
    if (word.isEmpty())
        return;

    // 获取搜索影片数据
    QStringList v = word.split("|");
    search(v.value(0), v.value(1).toInt());

    QMetaObject::invokeMethod(this, &MainWindow::endRequestSearch);
}

// 线程搜索影片列表结束
void MainWindow::endRequestSearch()
{
    search_model->removeRows(0, search_model->rowCount());

    int row = 0;
    QString word;
    for (int i = 0; i < vSearch.size(); i++)
    {
        for (int i2 = 0; i2 < vSearch.value(i).id.size(); i2++, row++)
        {
            search_model->setItem(row, 0, new QStandardItem(vSearch.value(i).last.value(i2)));
            search_model->item(row, 0)->setTextAlignment(Qt::AlignCenter);
            search_model->setItem(row, 1, new QStandardItem(vSearch.value(i).sname));
            search_model->item(row, 1)->setTextAlignment(Qt::AlignCenter);
            search_model->setItem(row, 2, new QStandardItem(vSearch.value(i).id.value(i2)));
            search_model->item(row, 2)->setTextAlignment(Qt::AlignCenter);
            search_model->setItem(row, 3, new QStandardItem(vSearch.value(i).tname.value(i2)));
            search_model->item(row, 3)->setTextAlignment(Qt::AlignCenter);
            search_model->setItem(row, 4, new QStandardItem(vSearch.value(i).name.value(i2)));
            search_model->item(row, 0)->setTextAlignment(Qt::AlignLeft);
            search_model->item(row, 4)->setForeground(QBrush(QColor(255, 0, 0)));

            // vSearch.value(i).pic.value(i2) 是空的
            word = vSearch.value(i).pic.value(i2) + "|" + vSearch.value(i).api + "|" + vSearch.value(i).id.value(i2);

            search_model->item(row, 0)->setData(word);
        }
    }

    // 默认选择第一个
    if(search_model->rowCount() > 0)
        _tableView->selectRow(0);

    show_loading(false);
}

void MainWindow::on_search_source_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    on_search_name_returnPressed();
}

void MainWindow::on_action_seting_triggered()
{
    emit setshow();
}

void MainWindow::on_action_resource_triggered()
{
    _tree_model->removeRows(0, _tree_model->rowCount());
    _tree_model->setItem(0, 0, new QStandardItem("正在刷新..."));

    QtConcurrent::run(this, &MainWindow::requestResources);
}

// 取资源数据
void MainWindow::requestResources()
{
    // type.clear();
    QMutexLocker locker(&mtx);

    getclass(app.sourcePath);

    getlive(app.livePath);

    QMetaObject::invokeMethod(this, &MainWindow::endRequestResources);
}

// 获取资源数据结束
void MainWindow::endRequestResources()
{
    _tree_model->removeRows(0, _tree_model->rowCount());
    _tree_model->setItem(0, 0, new QStandardItem("正在刷新..."));

    ui->search_source->clear();
    ui->search_source->addItem("全部");

    for (int i = 0; i < sources.size(); i++)
    {
        _tree_model->setItem(i, 0, new QStandardItem(sources.value(i).name));

        if (sources.value(i).name != "直播列表")
        {
            ui->search_source->addItem(sources[i].name);
        }

        foreach (Nameinfo var, sources[i].type)
        {
            _tree_model->item(i)->appendRow(new QStandardItem(var.name));
        }
    }
    //qDebug()<<"type size:"<<type.size();

    // 展开直播列表
    QList<QStandardItem *> lst = _tree_model->findItems("直播", Qt::MatchContains);
    if (lst.count() > 0)
        ui->tree_source->expand(lst.first()->index());
}
