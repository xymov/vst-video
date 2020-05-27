#!/bin/bash

#拷贝依赖运行库
exe="./usr/src/vst-video/vst-video" #发布的程序名称  
des="./usr/lib/vst-video" #你的路径 
deplist=$(ldd $exe | awk  '{if (match($3,"/")){ printf("%s "),$3 } }')  
cp $deplist $des  


