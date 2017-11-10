1.请将SDK里的动态库放到readme.txt同级目录下
    libarcsoft_fsdk_face_detection.dll 
    libarcsoft_fsdk_face_recognition.dll
  或
    libarcsoft_fsdk_face_detection.so 
    libarcsoft_fsdk_face_recognition.so
2.32位SDK搭配使用32位的python,64位SDK搭配使用64位的python,否则会失败。
3.依赖Pillow
4.请设置好APPID FD_SDKKEY
    APPID     = c_char_p(b'XXXXXXXXXX')
    FD_SDKKEY = c_char_p(b'YYYYYYYYYY')
    FR_SDKKEY = c_char_p(b'WWWWWWWWWW')
5.请设置好YUV文件路径、图像大小和颜色格式
    输入图支持YUV JPG PNG BMP
    if bUseYUVFile:
        filePathA = u'001_640x480_I420.YUV'
        yuv_widthA = 640
        yuv_heightA = 480
        yuv_formatA = ASVL_COLOR_FORMAT.ASVL_PAF_I420

        filePathB = u'003_640x480_I420.YUV'
        yuv_widthB = 640
        yuv_heightB = 480
        yuv_formatB = ASVL_COLOR_FORMAT.ASVL_PAF_I420

        inputImgA = loadYUVImage(filePathA, yuv_widthA, yuv_heightA, yuv_formatA)
        inputImgB = loadYUVImage(filePathB, yuv_widthB, yuv_heightB, yuv_formatB)
    else:
        filePathA = u'1_9.jpg'
        filePathB = u'001.jpg'

        inputImgA = loadImage(filePathA)
        inputImgB = loadImage(filePathB)


