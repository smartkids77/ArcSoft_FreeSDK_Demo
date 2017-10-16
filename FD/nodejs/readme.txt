1.请将SDK里的动态库放到readme.txt同级目录下
    libarcsoft_fsdk_face_detection.dll
  或libarcsoft_fsdk_face_detection.so
2.32位SDK搭配使用32位的nodejs,64位SDK搭配使用64位的nodejs,否则会失败。
3.依赖 ffi ref ref-struct ref-array jimp
    npm install ffi --dev
    npm install ref --dev
    npm install ref-struct --dev
    npm install ref-array --dev
    npm install jimp --dev
4.请设置好APPID FD_SDKKEY
    //var APPID  = 'XXXXXXXXXX';
    //var FD_SDKKEY = 'YYYYYYYYYY';
5.请设置好图像文件路径、图像大小和颜色格式
