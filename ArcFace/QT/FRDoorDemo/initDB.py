from ctypes import *
from PIL import Image
import os.path
import platform
import os
import sys
import sqlite3 
import datetime
import io

APPID = c_char_p(u'XXXXXXXXXXXXXXX'.encode('utf-8'))
FD_SDKKEY = c_char_p(u'YYYYYYYYYYYYYYY'.encode('utf-8'))
FR_SDKKEY = c_char_p(u'WWWWWWWWWWWWWWWW'.encode('utf-8'))

c_ubyte_p = POINTER(c_ubyte) 
ASVL_PAF_RGB24_B8G8R8 = 0x201
ASVL_PAF_RGB32_B8G8R8A8 = 0x302

class AFD_FSDK_Version(Structure):
    _fields_ = [('lCodebase',c_int32),('lMajor',c_int32),('lMinor',c_int32),('lBuild',c_int32),
                ('Version',c_char_p),('BuildDate',c_char_p),('CopyRight',c_char_p)]

class AFR_FSDK_Version(Structure):
    _fields_ = [('lCodebase',c_int32),('lMajor',c_int32),('lMinor',c_int32),('lBuild',c_int32),('lFeatureLevel',c_int32),
                ('Version',c_char_p),('BuildDate',c_char_p),('CopyRight',c_char_p)]

class ASVLOFFSCREEN(Structure):
    _fields_ = [('u32PixelArrayFormat',c_uint32),('i32Width',c_int32),('i32Height',c_int32),
                ('ppu8Plane',c_ubyte_p*4),('pi32Pitch',c_int32*4)]

class MRECT(Structure):
    _fields_ = [('left',c_int32),('top',c_int32),('right',c_int32),('bottom',c_int32)]

class AFD_FSDK_FACERES(Structure):
    _fields_ = [('nFace',c_int32),('rcFace',POINTER(MRECT)),('lfaceOrient',POINTER(c_int32))]

class AFR_FSDK_FACEINPUT(Structure):
    _fields_ = [('rcFace',MRECT),('lOrient',c_int32)]

class AFR_FSDK_FACEMODEL(Structure):
    _fields_ = [('pbFeature',c_void_p),('lFeatureSize',c_int32)]

if __name__ == '__main__':
    if len(sys.argv) == 2:
        if sys.version_info[0] < 3:
            srcImageDir = sys.argv[1].decode(sys.stdout.encoding)
        else:
            srcImageDir = sys.argv[1]
        if not os.path.isdir(srcImageDir):
            print(srcImageDir+u':directory does not exist')
            exit(0)
    else:
        print('wrong number of arguments')
        exit(0)

    libname_FD = 'arcsoft_fsdk_face_detection'
    libname_FR = 'arcsoft_fsdk_face_recognition'
    if platform.system() == 'Windows':
        FDlib = CDLL(os.getcwd() + os.path.sep + 'lib'+libname_FD+'.dll')
        FRlib = CDLL(os.getcwd() + os.path.sep + 'lib'+libname_FR+'.dll')
        libc = cdll.msvcrt
    else:
        FDlib = CDLL(os.getcwd() + os.path.sep + 'lib'+libname_FD+'.so')
        FRlib = CDLL(os.getcwd() + os.path.sep + 'lib'+libname_FR+'.so')
        libc = FDlib

    FDlib.AFD_FSDK_GetVersion.restype = POINTER(AFD_FSDK_Version)
    FDlib.AFD_FSDK_GetVersion.argtypes =(c_void_p,)
    FDlib.AFD_FSDK_InitialFaceEngine.restype = c_int32
    FDlib.AFD_FSDK_InitialFaceEngine.argtypes = (c_char_p,c_char_p,c_void_p,c_int32,POINTER(c_void_p),c_int32,c_int32,c_int32)
    FDlib.AFD_FSDK_StillImageFaceDetection.restype = c_int32
    FDlib.AFD_FSDK_StillImageFaceDetection.argtypes = (c_void_p,POINTER(ASVLOFFSCREEN),POINTER(POINTER(AFD_FSDK_FACERES)))
    FDlib.AFD_FSDK_UninitialFaceEngine.restype = c_int32
    FDlib.AFD_FSDK_UninitialFaceEngine.argtypes = (c_void_p,)

    FRlib.AFR_FSDK_GetVersion.restype = POINTER(AFR_FSDK_Version)
    FRlib.AFR_FSDK_GetVersion.argtypes =(c_void_p,)
    FRlib.AFR_FSDK_InitialEngine.restype = c_int32
    FRlib.AFR_FSDK_InitialEngine.argtypes = (c_char_p,c_char_p,c_void_p,c_int32,POINTER(c_void_p))
    FRlib.AFR_FSDK_ExtractFRFeature.restype = c_int32
    FRlib.AFR_FSDK_ExtractFRFeature.argtypes = (c_void_p,POINTER(ASVLOFFSCREEN),POINTER(AFR_FSDK_FACEINPUT),POINTER(AFR_FSDK_FACEMODEL))
    FRlib.AFR_FSDK_UninitialEngine.restype = c_int32
    FRlib.AFR_FSDK_UninitialEngine.argtypes = (c_void_p,)

    libc.malloc.restype = c_void_p
    libc.malloc.argtypes =(c_size_t,)
    libc.free.argtypes = (c_void_p,)
    libc.memcpy.restype = c_void_p
    libc.memcpy.argtypes =(c_void_p,c_void_p,c_size_t)

    WORKBUF_SIZE = 40*1024*1024
    pFDWorkMem = libc.malloc(c_size_t(WORKBUF_SIZE))
    hFDEngine = c_void_p()
    ret = FDlib.AFD_FSDK_InitialFaceEngine(APPID, FD_SDKKEY, pFDWorkMem, c_int32(WORKBUF_SIZE), byref(hFDEngine),c_int32(5),c_int32(16),c_int32(50))
    if ret != 0:
        print ('AFD_FSDK_InitialFaceEngine ret == 0x{:x}'.format(ret))
        exit(0)

    pVersionInfo = FDlib.AFD_FSDK_GetVersion(hFDEngine)
    print ('{} {} {} {}'.format(pVersionInfo.contents.lCodebase, pVersionInfo.contents.lMajor, pVersionInfo.contents.lMinor, pVersionInfo.contents.lBuild))
    print ('{}'.format(c_char_p(pVersionInfo.contents.Version).value.decode('utf-8')))
    print ('{}'.format(c_char_p(pVersionInfo.contents.BuildDate).value.decode('utf-8')))
    print ('{}'.format(c_char_p(pVersionInfo.contents.CopyRight).value.decode('utf-8')))

    pFRWorkMem = libc.malloc(c_size_t(WORKBUF_SIZE))
    hFREngine = c_void_p()
    ret = FRlib.AFR_FSDK_InitialEngine(APPID, FR_SDKKEY, pFRWorkMem, c_int32(WORKBUF_SIZE), byref(hFREngine))
    if ret != 0:
        print ('AFR_FSDK_InitialEngine ret == 0x{:x}'.format(ret))
        exit(0)

    pVersionInfo = FRlib.AFR_FSDK_GetVersion(hFREngine)
    print ('{} {} {} {}'.format(pVersionInfo.contents.lCodebase, pVersionInfo.contents.lMajor, pVersionInfo.contents.lMinor, pVersionInfo.contents.lBuild))
    print ('{}'.format(c_char_p(pVersionInfo.contents.Version).value.decode('utf-8')))
    print ('{}'.format(c_char_p(pVersionInfo.contents.BuildDate).value.decode('utf-8')))
    print ('{}'.format(c_char_p(pVersionInfo.contents.CopyRight).value.decode('utf-8')))

    db_name = 'face_{}.db'.format(int((datetime.datetime.utcnow() - datetime.datetime(1970, 1, 1)).total_seconds()))
    db_conn = sqlite3.connect(db_name)
    db_cursor = db_conn.cursor()
    db_cursor.execute('CREATE TABLE faceinfo(id INTEGER, name TEXT, feature BLOB,thumbnail BLOB)')
    faceid = 0
    for root, dirs, files in os.walk(srcImageDir):
        for name in files:
            name_upper = name.upper()
            facename, file_extension = os.path.splitext(name)
            if(name_upper.endswith('.PNG') or name_upper.endswith('.JPG') or name_upper.endswith('.BMP')):
                image_raw = Image.open(os.path.join(root, name)).convert('RGB')
                image_raw = image_raw.crop((0,0,image_raw.width&0xFFFFFFFC,image_raw.height&0xFFFFFFFC))
                thumbnail_width = 192
                thumbnail_height =  (image_raw.height*thumbnail_width//image_raw.width)&0xFFFFFFFC
                thumbnail = image_raw.resize((thumbnail_width, thumbnail_height))
                b, g, r = image_raw.split()
                image_bgr_for_fd = Image.merge('RGB', (r, g, b))
                image_bgra_for_fr = image_bgr_for_fd.convert('RGBA')

                bgr_raw = image_bgr_for_fd.tobytes()
                bgra_raw = image_bgra_for_fr.tobytes()
                inputImage = ASVLOFFSCREEN()
                inputImage.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8
                inputImage.i32Width = image_bgr_for_fd.width
                inputImage.i32Height = image_bgr_for_fd.height
                inputImage.pi32Pitch[0] = inputImage.i32Width*3
                inputImage.pi32Pitch[1] = 0
                inputImage.pi32Pitch[2] = 0
                inputImage.pi32Pitch[3] = 0
                inputImage.ppu8Plane[0] = cast(bgr_raw,c_ubyte_p)
                inputImage.ppu8Plane[1] = cast(0,c_ubyte_p)
                inputImage.ppu8Plane[2] = cast(0,c_ubyte_p)
                inputImage.ppu8Plane[3] = cast(0,c_ubyte_p)

                pFaceRes = POINTER(AFD_FSDK_FACERES)()
                ret = FDlib.AFD_FSDK_StillImageFaceDetection(hFDEngine, byref(inputImage),byref(pFaceRes))
                if ret != 0:
                    print ('AFD_FSDK_StillImageFaceDetection ret == 0x{:x}'.format(ret))
                    exit(0)

                inputImage.u32PixelArrayFormat = ASVL_PAF_RGB32_B8G8R8A8
                inputImage.pi32Pitch[0] = inputImage.i32Width*4
                inputImage.ppu8Plane[0] = cast(bgra_raw,c_ubyte_p)

                for i in range(0, pFaceRes.contents.nFace):
                    print(facename+u' {:d} of total {:d} ({:d},{:d},{:d},{:d})'.format( \
                        i+1,pFaceRes.contents.nFace, \
                        pFaceRes.contents.rcFace[i].left, \
                        pFaceRes.contents.rcFace[i].top,\
                        pFaceRes.contents.rcFace[i].right, \
                        pFaceRes.contents.rcFace[i].bottom))
                    faceinput = AFR_FSDK_FACEINPUT()
                    faceinput.rcFace.left = pFaceRes.contents.rcFace[i].left
                    faceinput.rcFace.top = pFaceRes.contents.rcFace[i].top
                    faceinput.rcFace.right = pFaceRes.contents.rcFace[i].right
                    faceinput.rcFace.bottom = pFaceRes.contents.rcFace[i].bottom
                    faceinput.lOrient = pFaceRes.contents.lfaceOrient[i]
                    facemodel = AFR_FSDK_FACEMODEL()
                    facemodel.lFeatureSize = 0
                    facemodel.pbFeature = cast(0,c_void_p)
                    ret = FRlib.AFR_FSDK_ExtractFRFeature(hFREngine, byref(inputImage),byref(faceinput),byref(facemodel))
                    if ret != 0:
                        print ('AFD_FSDK_StillImageFaceDetection ret == 0x{:x}'.format(ret))
                    else:
                        #print ('facemodel.lFeatureSize == {}'.format(facemodel.lFeatureSize))
                        imgByteIO = io.BytesIO()
                        thumbnail.save(imgByteIO, format='PNG')
                        img_bytearray = imgByteIO.getvalue()
                        feature = create_string_buffer(facemodel.lFeatureSize)
                        libc.memcpy(feature,facemodel.pbFeature,facemodel.lFeatureSize)
                        db_cursor.execute(u'INSERT INTO faceinfo VALUES (?,?,?,?)', (faceid,facename,sqlite3.Binary(feature.raw),sqlite3.Binary(img_bytearray)))
                        faceid = faceid + 1
                    break

    db_conn.commit()
    db_conn.close()
    FDlib.AFD_FSDK_UninitialFaceEngine(hFDEngine)
    FRlib.AFR_FSDK_UninitialEngine(hFREngine)
    libc.free(pFDWorkMem)
    libc.free(pFRWorkMem)
    exit(0)
