#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "webresource.h"

#include <QtConcurrent>
#include <QScrollBar>

void MainWindow::initList()
{
    // 图片列表框
    ui->listWidget->setIconSize(QSize(210, 210));             // 设置图标大小
    ui->listWidget->setGridSize(QSize(240, 240));             // 设置 item 大小
    ui->listWidget->setResizeMode(QListView::Adjust);         // 自动适应布局
    ui->listWidget->setViewMode(QListView::IconMode);         // 大图标模式
    ui->listWidget->setMovement(QListView::Static);           // 禁止拖动
    ui->listWidget->setSpacing(10);                           // 间距
    ui->listWidget->horizontalScrollBar()->setDisabled(true); // 不显示横向滚动条

    // 浏览器右键菜单管理
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ExploreMenu(const QPoint &)));

    ui->listWidget->installEventFilter(this);

    ui->page_info->setText("");                  // 页数信息默认清空

    // 创建搜索表格视图
    createTableView();
}

void MainWindow::createTableView()
{
    // 搜索表格列表视图
    _tableView = new QTableView(ui->tab_browse);
    _tableView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    _tableView->hide();
    ui->verticalLayout->addWidget(_tableView);

    // 数据模型
    search_model = new QStandardItemModel();
    search_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("更新日期")));
    search_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("资源站")));
    search_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("视频ID")));
    search_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("视频分类")));
    search_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("视频名称")));

    // 绑定
    _tableView->setModel(search_model);

    // 设置表格的各列的宽度值
    _tableView->setColumnWidth(0, 150);
    _tableView->setColumnWidth(1, 90);
    _tableView->setColumnWidth(2, 70);
    _tableView->setColumnWidth(3, 80);
    _tableView->setColumnWidth(4, 400);

    _tableView->horizontalHeader()->setStretchLastSection(true);                     // 自动适应布局
    _tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);            // 表头信息显示居左
    search_model->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignLeft);

    _tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection); // 只能单选
    _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);                 // 设置选中时为整行选中
    _tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);                  // 不可编辑

    // 搜索右键菜单管理
    _tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ExploreMenu(const QPoint &)));

    connect(_tableView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_tableView_currentRowChanged(const QModelIndex &, const QModelIndex &)));

    connect(_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(on_tableView_doubleClicked(const QModelIndex &)));
}

// 取分页信息
void MainWindow::getpageinfo(int page)
{
    // 取当前选择项
    QModelIndex index = ui->tree_source->currentIndex();
    if (index.row() < 0)
        return;

    show_loading(true);

    // 取选择项目序号
    int row = index.parent().row();

    // 取关联分类 ID
    QString id;
    if (row == -1)
    {
        id = "";
        row = index.row();
    }
    else
    {
        id = sources.value(row).type.value(index.row()).id;
    }

    // 取关联 api 地址
    QString api = sources.value(row).api;

    ui->listWidget->clear();

    isDirExist(app.cache, true);

    getvideo(4, api, id, QString::number(page));

    for (int i = 0; i < vBrowse.id.size(); i++)
    {
        createListWidget(ui->listWidget, i);
    }

    if (ui->listWidget->count() == 0)
    {
        show_loading(false);
        QMessageBox::warning(nullptr, "提示", "当前分类无资源!", QMessageBox::Yes);
    }
    else
    {
        // 默认选择第一个
        ui->listWidget->setCurrentRow(0);
    }

    QString str = QString("页数：%1/%3，视频：%2/%4").arg(vBrowse.page).arg(vBrowse.pagesize).arg(vBrowse.pagecount).arg(vBrowse.recordcount);

    ui->page_info->setText(str);

    ui->page_edit->setValidator(new QIntValidator(1, vBrowse.pagecount.toInt(), this)); // 限定输入范围

    show_loading(false);
}

// 动态创建浏览器图片表项
void MainWindow::createListWidget(QListWidget *listWidget, int key)
{
    if (key < listWidget->count())
        return;

    if (key >= vBrowse.id.size() || key >= vBrowse.name.size())
        return;

    QString word = vBrowse.pic.value(key) + "|" + vBrowse.api + "|" + vBrowse.id.value(key);

    QVBoxLayout *widgetLayout = new QVBoxLayout;
    widgetLayout->setMargin(0);
    widgetLayout->setSpacing(0);

    // 图标
    QString file = topic(vBrowse.api, vBrowse.id.value(key));
    if (!isFileExists(file))
    {
        QtConcurrent::run(this, &MainWindow::requestBrowseImage, word);
        file = app.nopic;
    }

    // 头像裁剪
    QPixmap pixmap(file);
    if (pixmap.width() > 227 || pixmap.height() > 227)
    {
        pixmap = pixmap.scaled(234, 234, Qt::KeepAspectRatio);
    }

    QLabel *imageLabel = new QLabel;
    //imageLabel->setScaledContents(true);  // 图片缩放
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter); // 图片居中

    widgetLayout->addWidget(imageLabel);

    // 标签
    QLabel *txtLabel = new QLabel;
    txtLabel->setFixedHeight(60);
    txtLabel->setWordWrap(true);
    txtLabel->setAlignment(Qt::AlignCenter); // 文本居中

    QString name = vBrowse.name[key];
    if (name.size() > 30)
    {
        name = name.mid(0, 20) + "...";
        txtLabel->setToolTip(vBrowse.name.value(key));
    }

    txtLabel->setText(name);

    widgetLayout->addWidget(txtLabel);

    // 添加列表项
    QWidget *widget = new QWidget;
    widget->setLayout(widgetLayout);

    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(240, 240));
    item->setData(Qt::UserRole, word);

    listWidget->addItem(item);

    listWidget->setSizeIncrement(240, 240); // 当用户重新定义窗口尺寸的时候，窗口会以此为基准
    listWidget->setItemWidget(item, widget);
}

void MainWindow::requestBrowseImage(QString word)
{
    if (word.isEmpty())
        return;

    QStringList v = word.split("|");

    //QMutexLocker locker(&_browse_locker);

    // 下载浏览图片
    UrlRequestImg(v.value(0), toHash(v.value(1)) + "_" + v.value(2));

    QMetaObject::invokeMethod(this, "endRequestBrowseImage", Q_ARG(QString, word));
}

// 浏览下载图片结束
void MainWindow::endRequestBrowseImage(QString word)
{
    QStringList v = word.split("|");

    QString file = topic(v.value(1), v.value(2));
    if (isFileExists(file))
    {
        setListWidgetImage(ui->listWidget, word);
    }
}

// 设置浏览器图片表项
void MainWindow::setListWidgetImage(QListWidget *listWidget, QString word)
{
    // 查找对应列表项
    QListWidgetItem *pItem = nullptr;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QString str = ui->listWidget->item(i)->data(Qt::UserRole).toString();
        if(str != word)
            continue;

        pItem = ui->listWidget->item(i);
    }

    if(pItem == nullptr)
        return;

    QWidget *widget = static_cast<QWidget *>(listWidget->itemWidget(pItem));
    if(widget->children().size() != 3)
        return;

    QStringList v = word.split("|");

    // 图标
    QString file = topic(v.value(1), v.value(2));
    if (!isFileExists(file))
    {
        QtConcurrent::run(this, &MainWindow::requestBrowseImage, word);
        file = app.nopic;
    }

    QPixmap pixmap(file);

    // 头像裁剪
    if (pixmap.width() > 227 || pixmap.height() > 227)
    {
        pixmap = pixmap.scaled(234, 234, Qt::KeepAspectRatio);
    }

    QLabel *imageLabel = static_cast<QLabel *>(widget->children().value(1));
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter); // 图片居中
}

// 上页
void MainWindow::on_page_front_clicked()
{
    int page = vBrowse.page.toInt() - 1;
    if (page < 1)
    {
        return;
    }

    getpageinfo(page);
}

// 下页
void MainWindow::on_page_next_clicked()
{
    int page = vBrowse.page.toInt() + 1;
    if (page > vBrowse.pagecount.toInt())
    {
        return;
    }

    getpageinfo(page);
}

// 跳页
void MainWindow::on_page_jmp_clicked()
{
    getpageinfo(ui->page_edit->text().toInt());
}

void MainWindow::on_page_edit_returnPressed()
{
    getpageinfo(ui->page_edit->text().toInt());
}

// 列表被选择
void MainWindow::on_listWidget_currentRowChanged(int key)
{
    if (key == -1)
        return;

    QListWidgetItem *item = ui->listWidget->item(key);
    QString word = item->data(Qt::UserRole).toString();
    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    // 清空原来的预览信息
    ui->preview_info_pic->clear();
    ui->preview_count->clear();
    ui->preview_info_des->clear();

    // 检查选中内容的数据是否与当前视频分类信息一致
    if (vBrowse.id.value(key).toInt() == v.value(2).toInt())
    {
        // 介绍
        ui->preview_info_des->setHtml(todes(vBrowse, key));

        // 集数
        QStringList list = vBrowse.video.value(key).split("#");
        ui->preview_count->setText(QString::asprintf(" 共 %d 集", list.length()));

        // 照片
        QString file = topic(vBrowse.api, vBrowse.id.value(key));
        if (isFileExists(file))
        {
            QPixmap pixmap(file);
            ui->preview_info_pic->setPixmap(pixmap);
            ui->preview_info_pic->setAlignment(Qt::AlignCenter); // 图片居中
        }
        else
            QtConcurrent::run(this, &MainWindow::requestPreviewImage, word);
    }
    // 分类数据已经包含了视频信息
    //else
    //    QtConcurrent::run(this, &MainWindow::requestPreviewVideoInfo, key);
}

void MainWindow::requestPreviewImage(QString word)
{
    if (word.isEmpty())
        return;

    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    //QMutexLocker locker(&_browse_locker);

    QString url = v.value(0);
    // 搜索的图片地址是空的,需要从视频信息中获取
    if(_tableView->isVisible())
        url = vBrowse.pic.first();

    if(url.isEmpty())
    {
        qDebug() << vBrowse.name << " " << v.value(2) << "图片地址无效" << endl;
        return;
    }

    UrlRequestImg(url, toHash(v.value(1)) + "_" + v.value(2));

    QMetaObject::invokeMethod(this, "endRequestPreviewImage", Q_ARG(QString, word));
}

// 线程搜索下载图片结束
void MainWindow::endRequestPreviewImage(QString word)
{
    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    // 设置预览图片
    QString file = topic(v.value(1), v.value(2));
    if (!isFileExists(file))
    {
        file = app.nopic;
    }

    QPixmap pixmap(file);
    ui->preview_info_pic->setPixmap(pixmap);
    ui->preview_info_pic->setAlignment(Qt::AlignCenter); // 图片居中
}

// 搜索列表选择项改变
void MainWindow::on_tableView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)

    if (current.row() == -1)
        return;

    QString word = search_model->item(current.row(), 0)->data().toString();
    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    show_loading(true);

    // 清空原来的预览信息
    ui->preview_info_pic->clear();
    ui->preview_count->clear();
    ui->preview_info_des->clear();

    // 检查选中内容的数据是否与当前视频信息一致
    if (vBrowse.id.value(current.row()).toInt() == v.value(2).toInt())
    {
        // 介绍
        ui->preview_info_des->setHtml(todes(vBrowse, current.row()));

        // 集数
        QStringList list = vBrowse.video.first().split("#");
        ui->preview_count->setText(QString::asprintf(" 共 %d 集", list.length()));

        // 照片
        QString file = topic(vBrowse.api, vBrowse.id.value(current.row()));
        if (isFileExists(file))
        {
            QPixmap pixmap(file);
            ui->preview_info_pic->setPixmap(pixmap);
            ui->preview_info_pic->setAlignment(Qt::AlignCenter); // 图片居中
        }
        else
            QtConcurrent::run(this, &MainWindow::requestPreviewImage, word);
    }
    else
        QtConcurrent::run(this, &MainWindow::requestPreviewVideoInfo, word);
}

// 获取浏览影片数据
void MainWindow::requestPreviewVideoInfo(QString word)
{
    if(word.isEmpty())
        return;

    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    //QMutexLocker locker(&_browse_locker);

    QString api = v.value(1);
    QString id = v.value(2);
    getvideo(2, api, id);

    QMetaObject::invokeMethod(this, "endRequestPreviewVideoInfo", Q_ARG(QString, word));
}

// 线程搜索影片详情结束
void MainWindow::endRequestPreviewVideoInfo(QString word)
{
    // 未请求到信息
    if(vBrowse.id.isEmpty())
        return;

    QStringList v = word.split("|");
    if(v.size() != 3)
        return;

    if(v.value(1) != vBrowse.api || v.value(2) != vBrowse.id.first())
        return;

    // 图片
    QString file = topic(v.value(1), v.value(2));
    if (isFileExists(file))
    {
        QPixmap pixmap(file);
        ui->preview_info_pic->setPixmap(pixmap);
        ui->preview_info_pic->setAlignment(Qt::AlignCenter); // 图片居中
    }
    else
        QtConcurrent::run(this, &MainWindow::requestPreviewImage, word);

    // 集数
    QStringList list = vBrowse.video.first().split("#");
    ui->preview_count->setText(QString::asprintf(" 共 %d 集", list.length()));

    // 介绍
    ui->preview_info_des->setHtml(todes(vBrowse, 0));

    show_loading(false);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(vBrowse.id.isEmpty())
        return;

    int row = ui->listWidget->row(item);

    ui->comboBox_name->clear();
    ui->comboBox_part->clear();

    for (int i = 0; i < vBrowse.id.size(); i++)
    {
        ui->comboBox_name->addItem(vBrowse.name.value(i), vBrowse.api + "|" + vBrowse.id.value(i));
    }

    ui->comboBox_name->setCurrentIndex(row);

    show_loading(true);

    QtConcurrent::run(this, &MainWindow::requestDetailPlay, 0, 0);
}

void MainWindow::requestDetailPlay(int part, int time)
{
    // 等待分集数据加载，最长 10 秒
    for (int i = 0; i < 50; i++)
    {
        QThread::msleep(200);

        if(ui->comboBox_part->count() > 0)
            break;
    }

    QMetaObject::invokeMethod(this, "endRequestDetailPlay", Q_ARG(int, part), Q_ARG(int, time));
}

void MainWindow::endRequestDetailPlay(int part, int time)
{
    show_loading(false);

    if (ui->comboBox_part->count() == 0)
        return;

    ui->comboBox_part->setCurrentIndex(part);

    app.live = false;
    loadPlay(part, time);

    ui->label->show();
    ui->label_VideoName->show();
    ui->player_control_part->show();
    ui->tabWidget->setCurrentWidget(ui->tab_player);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(search_model->rowCount() == 0)
        return;

    ui->comboBox_name->clear();
    ui->comboBox_part->clear();

    // vSearch.value(i).pic.value(i2) 是空的
    QString name, word;
    for(int i = 0; i < search_model->rowCount(); i++)
    {
        name = search_model->item(i, 4)->text();

        QStringList v = search_model->item(i, 0)->data().toString().split("|");
        word = v.at(1) + "|" + v.at(2);

        ui->comboBox_name->addItem(name, word);
    }

    ui->comboBox_name->setCurrentIndex(index.row());

    show_loading(true);

    QtConcurrent::run(this, &MainWindow::requestDetailPlay, 0, 0);
}

// 浏览器弹出菜单
void MainWindow::ExploreMenu(const QPoint &pos)
{
    Q_UNUSED(pos);

    ui->menu_explore->exec(QCursor::pos());
}

void MainWindow::on_action_explore_play_triggered()
{
    if(_tableView->isVisible())
        on_tableView_doubleClicked(_tableView->currentIndex());
    else
        on_listWidget_itemDoubleClicked(ui->listWidget->currentItem());
}

// 调用关联应用直接打开，一般会是浏览器
void MainWindow::on_action_explore_xopen_triggered()
{
    export_list_to_detail();

    if (ui->comboBox_part->count() > 0)
    {
        open(ui->comboBox_part->itemData(ui->comboBox_part->currentIndex()).toString());
    }
}

void MainWindow::export_list_to_detail()
{
    ui->comboBox_name->clear();
    ui->comboBox_part->clear();

    if(_tableView->isVisible())
    {
        // vSearch.value(i).pic.value(i2) 是空的
        QString name, word;
        for(int i = 0; i < search_model->rowCount(); i++)
        {
            name = search_model->item(i, 4)->text();

            QStringList v = search_model->item(i, 0)->data().toString().split("|");
            word = v.at(1) + "|" + v.at(2);

            ui->comboBox_name->addItem(name, word);
        }

        ui->comboBox_name->setCurrentIndex(_tableView->currentIndex().row());
    }
    else
    {
        for (int i = 0; i < vBrowse.id.size(); i++)
        {
            ui->comboBox_name->addItem(vBrowse.name.value(i), vBrowse.api + "|" + vBrowse.id.value(i));
        }

        ui->comboBox_name->setCurrentIndex(ui->listWidget->currentIndex().row());
    }
}

// 下载到本地再调用关联应用打开
void MainWindow::on_action_explore_xplay_triggered()
{
    export_list_to_detail();

    if (ui->comboBox_part->count() > 0)
    {
        show_loading(true);

        OpenM3u8(ui->comboBox_part->itemData(ui->comboBox_part->currentIndex()).toString());

        show_loading(false);
    }
}

// 新窗口打开
void MainWindow::on_action_explore_xnew_triggered()
{
    export_list_to_detail();

    if (ui->comboBox_part->count() > 0)
    {
        QStringList list;

        for (int i = 0; i < ui->comboBox_part->count(); i++)
        {
            list.append(ui->comboBox_part->itemData(i).toString());
        }

        QProcess::startDetached(QDir::currentPath() + "/vst-video", list);
    }
}

void MainWindow::on_action_explore_detail_triggered()
{
    export_list_to_detail();

    ui->tabWidget->setCurrentWidget(ui->tab_detail);
}

void MainWindow::explore_view(bool listMode)
{
    if (listMode)
    {
        ui->verticalLayout->removeWidget(ui->listWidget);
        ui->verticalLayout->removeWidget(ui->box_page);
        ui->verticalLayout->addWidget(_tableView);

        _tableView->show();
    }
    else
    {
        ui->verticalLayout->removeWidget(_tableView);
        ui->verticalLayout->addWidget(ui->listWidget);
        ui->verticalLayout->addWidget(ui->box_page);

        _tableView->hide();
    }
}
