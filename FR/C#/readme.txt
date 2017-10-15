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
    //public const string FR_SDKKEY = "WWWWWWWWWW";
4.请设置好图像文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if (bUseRAWFile) {
        String filePathA = "001_640x480_I420.YUV";
        int yuv_widthA = 640;
        int yuv_heightA = 480;
        int yuv_formatA = ASVL_COLOR_FORMAT.ASVL_PAF_I420;

        String filePathB = "003_640x480_I420.YUV";
        int yuv_widthB = 640;
        int yuv_heightB = 480;
        int yuv_formatB = ASVL_COLOR_FORMAT.ASVL_PAF_I420;

        inputImgA = loadRAWImage(filePathA, yuv_widthA, yuv_heightA, yuv_formatA);
        inputImgB = loadRAWImage(filePathB, yuv_widthB, yuv_heightB, yuv_formatB);
    } else {
        String filePathA = "001.jpg";
        String filePathB = "1_9.jpg";

        inputImgA = loadImage(filePathA);
        inputImgB = loadImage(filePathB);
    }
    注意：如果图像文件使用相对路径，请放到工作目录下
