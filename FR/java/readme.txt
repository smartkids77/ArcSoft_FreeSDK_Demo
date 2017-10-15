1.
Windows:请将SDK里的动态库放到readme.txt同级目录下
Linux:请将SDK里的动态库放到 {readme.txt同级目录}/bin/linux-x86-64/ 目录下
2.32位SDK搭配使用32位的jre,64位SDK搭配使用64位的jre,否则会失败。
3.依赖jna-4.4.0
4.请设置好APPID FD_SDKKEY FR_SDKKEY
	public static final String    APPID  = "XXXXXXXXXX";
	public static final String FD_SDKKEY = "YYYYYYYYYY";
	public static final String FR_SDKKEY = "WWWWWWWWWW";
5.请设置好图像文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if(bUseRAWFile){
        String filePathA = "640x480_I420.YUV";
        int yuv_widthA = 640;
        int yuv_heightA = 480;
        int yuv_formatA = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
        
        String filePathB = "640x360_I420.YUV";
        int yuv_widthB = 640;
        int yuv_heightB = 360;
        int yuv_formatB = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
        
        inputImgA = loadRAWImage(filePathA,yuv_widthA,yuv_heightA,yuv_formatA);
        inputImgB = loadRAWImage(filePathB,yuv_widthB,yuv_heightB,yuv_formatB);
    }else{
        String filePathA = "fgg_003.jpg";
        String filePathB = "003.jpg";
        
        inputImgA = loadImage(filePathA);
        inputImgB = loadImage(filePathB);
    }
6.AFR_FSDK_FACEMODEL数据存取方法
    (1)从现有AFR_FSDK_FACEMODEL中获取数据
       byte[] featureInByteArray = faceFeatureA.toByteArray();
    (2)从byte[]中生成AFR_FSDK_FACEMODEL
       AFR_FSDK_FACEMODEL faceFeatureX = AFR_FSDK_FACEMODEL.fromByteArray(featureInByteArray);

