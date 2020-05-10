s="[Desktop Entry]\nName=全聚合影视\nComment=Media player\nExec=`pwd`/vst-video %u\nIcon=`pwd`/icon.png\nPath=`pwd`\nTerminal=false\nType=Application\nCategories=AudioVideo;"
echo -e $s > vst-video.desktop
cp `pwd`/vst-video.desktop ~/.local/share/applications/vst-video.desktop
cp `pwd`/vst-video.desktop ~/Desktop/vst-video.desktop
