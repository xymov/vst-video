#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "framelesshelper.h"
#include "set.h"
#include "types.h"
#include "utilities.h"

#include <QListWidget>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSystemTrayIcon>
#include <QTableView>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_paly_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_sound_clicked();

    void on_pushButton_full_clicked();

    void sliderProgressReleased();

    void positionChange(qint64);

    void durationChange(qint64);

    void mediaStatusChanged(QMediaPlayer::MediaStatus);
    void stateChanged(QMediaPlayer::State);

    void on_sliderProgress_sliderMoved(int position);

    void on_comboBox_name_currentIndexChanged(int index);

    void on_comboBox_part_currentIndexChanged(int index);

    //bool close();
    void addseek();
    void decseek();
    void volumeUp();
    void volumeDown();
    void TimerTimeOut();

    void on_pushButton_playlist_clicked();

    void on_value_Slider_valueChanged(int value);

    void volumeChange(int value);

    void on_tree_source_pressed(const QModelIndex &index);

    void on_listWidget_currentRowChanged(int currentRow);

    void on_page_front_clicked();

    void on_page_next_clicked();

    void on_page_jmp_clicked();

    void on_info_play_clicked();

    void on_info_front_clicked();

    void on_info_next_clicked();

    void on_search_source_currentIndexChanged(int index);

    void PlayMenu(const QPoint &pos);

    void ExploreMenu(const QPoint &pos);

    void on_action_open_triggered();

    void on_action_openurl_triggered();

    void on_action_brightness_add_triggered();

    void on_action_brightness_sub_triggered();

    void on_action_contrast_add_triggered();

    void on_action_contrast_sub_triggered();

    void on_action_Saturation_add_triggered();

    void on_action_Saturation_sub_triggered();

    void on_action_videosize_IgnoreAspectRatio_triggered();

    void on_action_videosize_KeepAspectRatio_triggered();

    void on_action_videosize_KeepAspectRatioByExpanding_triggered();

    void on_action_explore_play_triggered();

    void on_action_explore_xopen_triggered();

    void on_action_explore_xplay_triggered();

    void on_action_explore_xnew_triggered();

    void on_action_about_triggered();

    void on_pushButton_front_clicked();

    void on_pushButton_setting_clicked();

    void on_tableView_currentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_tableView_doubleClicked(const QModelIndex &index);

    // void on_action_mini_mode_triggered();

    void on_action_exit_triggered();

    void on_pushButton_close_clicked();

    void on_pushButton_mini_clicked();

    void on_pushButton_seting_clicked();

    void on_pushButton_max_clicked();

    void on_action_seting_triggered();

    //void on_action_mini_triggered();

    void on_action_resource_triggered();

    // 加载播放记录
    void renotes();

    void menu_action_notes_triggered(QAction *);

    //void on_action_tophint_triggered();

    void setWindowsTopHint();
    void remWindowsTopHint();

    void TitlebarMenu(const QPoint &pos);

    void on_action_tophint_toggled(bool arg1);

    void on_action_theme_1_triggered();

    void on_action_theme_2_triggered();

    void on_action_theme_0_triggered();

    void activeTray(QSystemTrayIcon::ActivationReason reason);

    void on_action_tray_exit_triggered();

    void on_action_tray_show_triggered();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_search_name_returnPressed();

    void on_volume_slider_valueChanged(int value);

    void on_player_control_part_currentIndexChanged(int index);

    void on_label_VideoName_linkActivated(const QString &link);

    void on_action_explore_detail_triggered();

    void playlist_currentIndexChanged(int index);

    void on_detail_partlist_itemClicked(QListWidgetItem *item);

    void on_detail_partlist_itemDoubleClicked(QListWidgetItem *item);

    void requestResources();
    void endRequestResources();

    void requestSearch(QString word);
    void endRequestSearch();

    void requestBrowseImage(QString word);
    void endRequestBrowseImage(QString word);
    void requestPreviewVideoInfo(QString word);
    void endRequestPreviewVideoInfo(QString word);
    void requestPreviewImage(QString word);
    void endRequestPreviewImage(QString word);

    void requestDetailVideoInfo(QString word);
    void endRequestDetailVideoInfo(QString word);
    void requestDetailImage(QString word);
    void endRequestDetailImage(QString word);

    void requestDetailPlay(int part = 0, int time = 0);
    void endRequestDetailPlay(int part, int time);

    void on_page_edit_returnPressed();

    void hideVolumeSlider();
    void showVolumeSlider();

signals:

    void quit();
    void setshow();

protected:


private:
    Ui::MainWindow *ui;

    // 窗口任意移动
    bool m_bDrag;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;

    QStandardItemModel *search_model;
    QString STimeDuration = "00:00:00";
    QMediaPlaylist *playlist;
    QMediaPlayer *player;
    QVideoWidget *video;

    QString API;
    QTimer *m_timer;
    QMutex mtx;
    set seting;
    QDialog loading;
    Config config;

    // 影片信息
    QStandardItemModel *_tree_model;

    QSystemTrayIcon *_tray;
    qint64 _trayClickCounter = 0;

    QTableView *_tableView;

    QSlider *_volume_slider;
    QLabel *_volume_value;
    QTimer *_volume_slider_timer;

    // 用于详情资源并发同步
    QMutex _detail_locker;
    // 用于浏览资源并发同步
    QMutex _browse_locker;

private:

    void initMainWindow();
    void initTitleBar();
    void initTray();
    void initResource();
    void initList();
    void createTableView();
    void initPlayer();
    void initTheme();
    void initRecord();

    void setSTime(qint64);
    void getCommond();
    void setVideoMode(Qt::AspectRatioMode mode);
    void show_loading(bool show);
    bool eventFilter(QObject *target, QEvent *event);

    void createListWidget(QListWidget *listWidget, int key);
    void setListWidgetImage(QListWidget *listWidget, QString word);

    void initLoading();
    void createVolume();
    void switchFullScreen(bool);
    void loadPlay(int index = 0, qint64 time = 0);

    void MinWriteNotes(int part = 0);

    void initListWidget(QListWidget *listWidget);

    void getpageinfo(int page);

    void loadMedia(int index = 0);

    void explore_view(bool listMode = false);

    void export_list_to_detail();
};

#endif // MAINWINDOW_H
