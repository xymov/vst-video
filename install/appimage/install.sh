#!/bin/bash

#参考文章：https://blog.csdn.net/Adieu_csdn/article/details/102638934?#linuxdeployqt_5

sudo  cp -r  ./bin /usr/local/bin

sudo chmod  -R  a+x  /usr/local/bin/

sudo linuxdeployqt vst-video -appimage




