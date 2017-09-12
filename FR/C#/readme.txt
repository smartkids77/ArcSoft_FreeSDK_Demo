1.请将SDK里的dll放到readme.txt同级目录下
    libarcsoft_fsdk_face_detection.dll 
    libarcsoft_fsdk_face_recognition.dll
  同时在工程配置中将工作目录配置成，否则会找不到dll
    <StartWorkingDirectory>../../../</StartWorkingDirectory>
2.32位SDK搭配使用CPU x86,64位SDK搭配使用CPU x64,请在工程配置中设置正确，否则会失败。
3.请设置好APPID FD_SDKKEY
    //public const string APPID     = "XXXXXXXXXX";
    //public const string FD_SDKKEY = "YYYYYYYYYY";
    //public const string FR_SDKKEY = "WWWWWWWWWW";
4.请设置好YUV文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if (bUseYUVFile) {
        String filePathA = "001_640x480_I420.YUV";
        int yuv_widthA = 640;
        int yuv_heightA = 480;
        int yuv_formatA = ASVL_COLOR_FORMAT.ASVL_PAF_I420;

        String filePathB = "003_640x480_I420.YUV";
        int yuv_widthB = 640;
        int yuv_heightB = 480;
        int yuv_formatB = ASVL_COLOR_FORMAT.ASVL_PAF_I420;

        inputImgA = loadYUVImage(filePathA, yuv_widthA, yuv_heightA, yuv_formatA);
        inputImgB = loadYUVImage(filePathB, yuv_widthB, yuv_heightB, yuv_formatB);
    } else {
        String filePathA = "001.jpg";
        String filePathB = "1_9.jpg";

        inputImgA = loadImage(filePathA);
        inputImgB = loadImage(filePathB);
    }

