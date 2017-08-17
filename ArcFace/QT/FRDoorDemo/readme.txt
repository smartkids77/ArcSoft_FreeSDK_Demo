注意事项
1.下载并安装QT 5.8
2.将下载的FT、FD和FR包里的头文件、动态库拷贝到工程目录下
3.通过脚本生成注册人脸数据库
(1)先替换initDB.py中APPID和SDKKEY的值.
(2)然后准被一批注册人脸图. 格式可以是bmp、jpg、png，文件名为对应的人名.
(3)再执行以下命令，生成文件名为face_*.db的注册人脸数据库
python initDB.py  your_face_image_directory
4.启动QtCreator并打开工程文件FR_Door_Demo.pro
5.新建头文件key.h  根据你在网站上实际申请的key,定义以下宏
#define FT_APPID       "XXXXXXXXXXXXXXXXXXXX"
#define FT_SDKKEY      "YYYYYYYYYYYYYYYYYYYY"
#define FR_APPID       "ZZZZZZZZZZZZZZZZZZZZ"
#define FR_SDKKEY      "WWWWWWWWWWWW"
6.编译并执行程序
7.通过load DB按钮，选择前面生成的.db文件
8.打开摄像头，或将视频文件拖至主窗口，开始进入检测模式


