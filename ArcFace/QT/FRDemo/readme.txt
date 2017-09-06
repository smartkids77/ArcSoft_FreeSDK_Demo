注意事项
0.输入源支持摄像头和视频文件
1.下载并安装QT 5.8
2.将下载的FT和FR包里的头文件、动态库拷贝到工程目录下
3.启动QtCreator并打开工程文件FRDemo.pro
4.新建头文件key.h  根据你在网站上实际申请的key,定义以下宏
#define FT_APPID       "XXXXXXXXXXXXXXXXXXXX"
#define FT_SDKKEY      "YYYYYYYYYYYYYYYYYYYY"
#define FR_APPID       "ZZZZZZZZZZZZZZZZZZZZ"
#define FR_SDKKEY      "WWWWWWWWWWWWWWWWWWWW"
5.编译并运行


