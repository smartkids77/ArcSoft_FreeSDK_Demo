1.配置工程
  Windows (Visual stuido):以readme.txt所在目录为工作目录
    请将SDK里的动态库放到该工作目录下，同时在工程配置中将工作目录配置到
    <StartWorkingDirectory>../../../</StartWorkingDirectory>
  Linux (MonoDevelop):以"readme.txt所在目录/bin/Debug/"为默认工作目录
    请将SDK里的动态库放到到工作目录下，并配置对System.Drawing的引用

2.32位SDK搭配使用CPU x86,64位SDK搭配使用CPU x64,请在工程配置中设置正确，否则会失败。
3.请设置好APPID FD_SDKKEY
    //public const string APPID     = "XXXXXXXXXX";
    //public const string FD_SDKKEY = "YYYYYYYYYY";
4.请设置好图像文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if (bUseRAWFile) {
        String filePath = "001_640x480_I420.YUV";
        int yuv_width = 640;
        int yuv_height = 480;
        int yuv_format = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
        inputImg = loadRAWImage(filePath, yuv_width, yuv_height, yuv_format);
    } else {
        String filePath = "003.jpg";
        inputImg = loadImage(filePath);
    }
    注意：如果图像文件使用相对路径，请放到工作目录下
