注意事项
0.输入源支持摄像头和视频文件
1.下载并安装QT,window请安装5.8,Linux请安装5.6
2.将下载的FT和FR包里的头文件、动态库拷贝到工程目录下
3.启动QtCreator并打开工程文件FRDemo.pro
4.新建头文件key.h,定义以下宏，并用申请的appid和sdkkey替换相应的内容
    #define APPID       "your appid"
    #define FT_SDKKEY      "your FT sdkkey"
    #define FR_SDKKEY      "your FR sdkkey"
5.编译并执行程序
6.点击Camera  使用摄像头作为数据源
  或点击Video 使用视频文件作为数据源
7.当画面中出现人脸是点击register 注册人脸并输入相应的人名
8.画面一旦检测到相似的人脸将显示对应的人脸


