1.请将SDK里的动态库放到readme.txt同级目录下
    libarcsoft_fsdk_face_detection.dll 
  或
    libarcsoft_fsdk_face_detection.so 
2.32位SDK搭配使用32位的python,64位SDK搭配使用64位的python,否则会失败。
3.依赖Pillow
4.请设置好APPID FD_SDKKEY
    APPID     = c_char_p(b'XXXXXXXXXX')
    FD_SDKKEY = c_char_p(b'YYYYYYYYYY')
5.请设置好YUV文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if bUseYUVFile:
        filePath = u'001_640x480_I420.YUV'
        yuv_width = 640
        yuv_height = 480
        yuv_format = ASVL_COLOR_FORMAT.ASVL_PAF_I420
        inputImg = loadYUVImage(filePath,yuv_width,yuv_height,yuv_format)
    else:
        filePath = u'003.jpg'
        inputImg = loadImage(filePath)
