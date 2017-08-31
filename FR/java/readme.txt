1.请将SDK里的dll放到readme.txt同级目录下
    libarcsoft_fsdk_face_detection.dll 
    libarcsoft_fsdk_face_recognition.dll
2.32位SDK搭配使用32位的jre,64位SDK搭配使用64位的jre,否则会失败。
3.依赖jna-4.4.0
4.请设置好APPID FD_SDKKEY FR_SDKKEY
	public static final String    APPID  = "XXXXXXXXXX";
	public static final String FD_SDKKEY = "YYYYYYYYYY";
	public static final String FR_SDKKEY = "WWWWWWWWWW";
5.请设置好YUV文件路径、图像大小和颜色格式
    String yuv_filePathA = "001_640x480_I420.YUV";
    int yuv_widthA = 640;
    int yuv_heightA = 480;
    int yuv_formatA = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
    String yuv_filePathB = "003_640x480_I420.YUV";
    int yuv_widthB = 640;
    int yuv_heightB = 480;
    int yuv_formatB = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
