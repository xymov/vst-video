QT       += core gui multimedia multimediawidgets xml concurrent



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config.cpp \
    framelesshelper.cpp \
    main.cpp \
    mainwindow.cpp \
    set.cpp \
    mainwindow_tray.cpp \
    mainwindow_player.cpp \
    mainwindow_detail.cpp \
    mainwindow_resource.cpp \
    mainwindow_list.cpp \
    mainwindow_titlebar.cpp \
    mainwindow_theme.cpp \
    mainwindow_record.cpp \
    mainwindow_loading.cpp \
    webresource.cpp \
    utilities.cpp

HEADERS += \
    config.h \
    framelesshelper.h \
    mainwindow.h \
    set.h \
    webresource.h \
    types.h \
    utilities.h

FORMS += \
    mainwindow.ui \
    set.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    README.md \
    deb/DEBIAN/control \
    deb/install.sh \
    deb/usr/share/applications/vst-video.desktop \
    deb/usr/share/icons/vst-video.png \
    deb/usr/src/vst-video/ico.png \
    deb/usr/src/vst-video/install.sh \
    deb/usr/src/vst-video/vst-video \
    deb/usr/src/vst-video/vst-video.sh \
    install/DEBIAN/control \
    install/QIFW/README \
    install/QIFW/config/config.xml \
    install/QIFW/packages/net.xymov.ifw/data/LICENSE \
    install/QIFW/packages/net.xymov.ifw/data/vst-video \
    install/QIFW/packages/net.xymov.ifw/meta/installscript.qs \
    install/QIFW/packages/net.xymov.ifw/meta/package.xml \
    install/README \
    install/README.md \
    install/appimage/bin/appimagetool \
    install/appimage/bin/linuxdeployqt \
    install/appimage/bin/patchelf \
    install/appimage/icon.png.png \
    install/appimage/install.sh \
    install/appimage/vst-video.desktop \
    install/bin/README.md \
    install/bin/appimagetool \
    install/bin/install.sh \
    install/bin/linuxdeployqt \
    install/bin/patchelf \
    install/config/config.xml \
    install/deb/DEBIAN/control \
    install/deb/getdll.sh \
    install/deb/install.sh \
    install/deb/output/DEBIAN/control \
    install/deb/output/usr/share/applications/vst-video.desktop \
    install/deb/output/usr/share/icons/vst-video.png \
    install/deb/output/usr/src/vst-video/ico.png \
    install/deb/output/usr/src/vst-video/install.sh \
    install/deb/output/usr/src/vst-video/vst-video \
    install/deb/output/usr/src/vst-video/vst-video.sh \
    install/deb/usr/share/applications/vst-video.desktop \
    install/deb/usr/share/icons/vst-video.png \
    install/deb/usr/src/vst-video/ico.png \
    install/deb/usr/src/vst-video/install.sh \
    install/deb/usr/src/vst-video/vst-video \
    install/deb/usr/src/vst-video/vst-video.sh \
    install/install.sh \
    install/install.sh \
    install/packages/net.xymov.ifw/data/LICENSE \
    install/packages/net.xymov.ifw/data/vst-video \
    install/packages/net.xymov.ifw/meta/installscript.qs \
    install/packages/net.xymov.ifw/meta/package.xml \
    install/usr/lib/vst-video/libFLAC.so.8 \
    install/usr/lib/vst-video/libGL.so.1 \
    install/usr/lib/vst-video/libGLX.so.0 \
    install/usr/lib/vst-video/libGLdispatch.so.0 \
    install/usr/lib/vst-video/libICE.so.6 \
    install/usr/lib/vst-video/libQt5Concurrent.so.5 \
    install/usr/lib/vst-video/libQt5Core.so.5 \
    install/usr/lib/vst-video/libQt5Gui.so.5 \
    install/usr/lib/vst-video/libQt5Multimedia.so.5 \
    install/usr/lib/vst-video/libQt5MultimediaWidgets.so.5 \
    install/usr/lib/vst-video/libQt5Network.so.5 \
    install/usr/lib/vst-video/libQt5OpenGL.so.5 \
    install/usr/lib/vst-video/libQt5Widgets.so.5 \
    install/usr/lib/vst-video/libQt5Xml.so.5 \
    install/usr/lib/vst-video/libSM.so.6 \
    install/usr/lib/vst-video/libX11-xcb.so.1 \
    install/usr/lib/vst-video/libX11.so.6 \
    install/usr/lib/vst-video/libXau.so.6 \
    install/usr/lib/vst-video/libXdmcp.so.6 \
    install/usr/lib/vst-video/libXext.so.6 \
    install/usr/lib/vst-video/libXi.so.6 \
    install/usr/lib/vst-video/libXtst.so.6 \
    install/usr/lib/vst-video/libasyncns.so.0 \
    install/usr/lib/vst-video/libbsd.so.0 \
    install/usr/lib/vst-video/libc.so.6 \
    install/usr/lib/vst-video/libcap.so.2 \
    install/usr/lib/vst-video/libdbus-1.so.3 \
    install/usr/lib/vst-video/libdl.so.2 \
    install/usr/lib/vst-video/libdouble-conversion.so.1 \
    install/usr/lib/vst-video/libfreetype.so.6 \
    install/usr/lib/vst-video/libgcc_s.so.1 \
    install/usr/lib/vst-video/libgcrypt.so.20 \
    install/usr/lib/vst-video/libglib-2.0.so.0 \
    install/usr/lib/vst-video/libgpg-error.so.0 \
    install/usr/lib/vst-video/libgraphite2.so.3 \
    install/usr/lib/vst-video/libharfbuzz.so.0 \
    install/usr/lib/vst-video/libicudata.so.63 \
    install/usr/lib/vst-video/libicui18n.so.63 \
    install/usr/lib/vst-video/libicuuc.so.63 \
    install/usr/lib/vst-video/liblz4.so.1 \
    install/usr/lib/vst-video/liblzma.so.5 \
    install/usr/lib/vst-video/libm.so.6 \
    install/usr/lib/vst-video/libnsl.so.1 \
    install/usr/lib/vst-video/libogg.so.0 \
    install/usr/lib/vst-video/libpcre.so.3 \
    install/usr/lib/vst-video/libpcre2-16.so.0 \
    install/usr/lib/vst-video/libpng16.so.16 \
    install/usr/lib/vst-video/libpthread.so.0 \
    install/usr/lib/vst-video/libpulse-mainloop-glib.so.0 \
    install/usr/lib/vst-video/libpulse.so.0 \
    install/usr/lib/vst-video/libpulsecommon-12.2.so \
    install/usr/lib/vst-video/libresolv.so.2 \
    install/usr/lib/vst-video/librt.so.1 \
    install/usr/lib/vst-video/libsndfile.so.1 \
    install/usr/lib/vst-video/libstdc++.so.6 \
    install/usr/lib/vst-video/libsystemd.so.0 \
    install/usr/lib/vst-video/libuuid.so.1 \
    install/usr/lib/vst-video/libvorbis.so.0 \
    install/usr/lib/vst-video/libvorbisenc.so.2 \
    install/usr/lib/vst-video/libwrap.so.0 \
    install/usr/lib/vst-video/libxcb.so.1 \
    install/usr/lib/vst-video/libz.so.1 \
    install/usr/share/applications/vst-video.desktop \
    install/usr/share/icons/vst-video.png \
    install/usr/src/vst-video/ico.png \
    install/usr/src/vst-video/install.sh \
    install/usr/src/vst-video/live.txt \
    install/usr/src/vst-video/source.txt \
    install/usr/src/vst-video/vst-video \
    install/usr/src/vst-video/vst-video.sh \
    vst-video \
    icon.png

SUBDIRS += \
    install/QIFW/vst-video.pro \
    install/QIFW/vst-video.pro \
    install/vst-video.pro
