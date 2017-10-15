1.
Windows:请将SDK里的动态库放到readme.txt同级目录下
Linux:请将SDK里的动态库放到 {readme.txt同级目录}/bin/linux-x86-64/ 目录下
2.32位SDK搭配使用32位的jre,64位SDK搭配使用64位的jre,否则会失败。
3.依赖jna-4.4.0
4.请设置好APPID FD_SDKKEY
    public static final String APPID     = "XXXXXXXXXX";
    public static final String FD_SDKKEY = "YYYYYYYYYY";
5.请设置好图像文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if(bUseRAWFile){
        String filePath = "001_640x480_I420.YUV";
        int yuv_width = 640;
        int yuv_height = 480;
        int yuv_format = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
        inputImg = loadRAWImage(filePath,yuv_width,yuv_height,yuv_format);
    }else{
        String filePath = "003.jpg";
        inputImg = loadImage(filePath);
    }