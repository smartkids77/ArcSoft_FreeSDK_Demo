使用说明
0.下载并安装QT,window请安装5.8,Linux请安装5.6
1.下载并安装python
2.请将下载的FT、FD和FR包里的头文件、动态库拷贝到工程根目录下
3.请按照以下步骤,通过python脚本生成注册人脸数据库
    (1)替换initDB.py中相应的APPID和SDKKEY的值
    (2)准备一批注册人脸图,格式可以是bmp、jpg、png，文件名为对应的人名
    (3)执行以下命令，生成文件名为face_*.db的注册人脸数据库
       python initDB.py  your_face_image_directory
4.启动QtCreator并打开工程文件FR_Door_Demo.pro
5.新建头文件key.h,定义以下宏，并用申请的appid和sdkkey替换相应的内容
    #define APPID       "your appid"
    #define FT_SDKKEY      "your FT sdkkey"
    #define FR_SDKKEY      "your FR sdkkey"
6.编译并执行程序
7.点击load DB按钮，选择前面生成的.db文件
8.打开摄像头，或将视频文件拖至主窗口，开始进入检测模式
9.Demo程序内部算法库调用逻辑说明：
    (1)FT每一帧都处理。
    (2)FR由于消耗的时间比较大，将其放入后台线程中。
    (3)当FT两帧之间检测的人脸框重叠面积小于1/2时，视为人脸发生了变化，
      将数据传递给后台FR线程处理。处理后，更新前台FaceID
    (4)当FR后台线程处于空闲状态，即使FT两帧之间人脸重叠面积大于1/2，
      也将数据传递给后台FR线程处理。处理后，更新前台FaceID


