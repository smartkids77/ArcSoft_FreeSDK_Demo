1.请将SDK里的dll放到readme.txt同级目录下
    libarcsoft_fsdk_face_detection.dll 
  同时在工程配置中将工作目录配置成，否则会找不到dll
    <StartWorkingDirectory>../../../</StartWorkingDirectory>
2.32位SDK搭配使用CPU x86,64位SDK搭配使用CPU x64,请在工程配置中设置正确，否则会失败。
3.请设置好APPID FD_SDKKEY
    //public const string APPID     = "XXXXXXXXXX";
    //public const string FD_SDKKEY = "YYYYYYYYYY";
4.请设置好YUV文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if (bUseYUVFile) {
        String filePath = "001_640x480_I420.YUV";
        int yuv_width = 640;
        int yuv_height = 480;
        int yuv_format = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
        inputImg = loadYUVImage(filePath, yuv_width, yuv_height, yuv_format);
    } else {
        String filePath = "003.jpg";
        inputImg = loadImage(filePath);
    }

