from ctypes import *
import os.path
import platform
import os

APKID = c_char_p(u"XXXXXXXXXXXXXXXXXXXX".encode('utf-8'))
SDKKEY = c_char_p(u"YYYYYYYYYYYYYYYYYYY".encode('utf-8'))

c_ubyte_p = POINTER(c_ubyte) 
class AFD_FSDK_Version(Structure):
	_fields_ = [("lCodebase",c_int32),("lMajor",c_int32),("lMinor",c_int32),("lBuild",c_int32),
                ("Version",c_char_p),("BuildDate",c_char_p),("CopyRight",c_char_p)]
	
class ASVLOFFSCREEN(Structure):
	_fields_ = [("u32PixelArrayFormat",c_uint32),("i32Width",c_int32),("i32Height",c_int32),
				("ppu8Plane",c_ubyte_p*4),("pi32Pitch",c_int32*4)]

class MRECT(Structure):
	_fields_ = [("left",c_int32),("top",c_int32),("right",c_int32),("bottom",c_int32)]

class AFD_FSDK_FACERES(Structure):
	_fields_ = [("nFace",c_int32),("rcFace",POINTER(MRECT)),("lfaceOrient",POINTER(c_int32))]

libname = 'arcsoft_fsdk_face_detection'
if platform.system() == 'Windows':
    mainlib = CDLL(os.getcwd() + os.path.sep + "lib"+libname+".dll")
    libc = cdll.msvcrt
else:
    mainlib = CDLL(os.getcwd() + os.path.sep + "lib"+libname+".so")
    libc = mainlib

mainlib.AFD_FSDK_GetVersion.restype = POINTER(AFD_FSDK_Version)
mainlib.AFD_FSDK_GetVersion.argtypes =(c_void_p,)
mainlib.AFD_FSDK_InitialFaceEngine.restype = c_int32
mainlib.AFD_FSDK_InitialFaceEngine.argtypes = (c_char_p,c_char_p,c_void_p,c_int32,POINTER(c_void_p),c_int32,c_int32,c_int32)
mainlib.AFD_FSDK_StillImageFaceDetection.restype = c_int32
mainlib.AFD_FSDK_StillImageFaceDetection.argtypes = (c_void_p,POINTER(ASVLOFFSCREEN),POINTER(POINTER(AFD_FSDK_FACERES)))
mainlib.AFD_FSDK_UninitialFaceEngine.argtypes = (c_void_p,)
libc.malloc.restype = c_void_p
libc.malloc.argtypes =(c_size_t,)
libc.free.argtypes = (c_void_p,)

WORKBUF_SIZE = 40*1024*1024
pWorkMem = libc.malloc(c_size_t(WORKBUF_SIZE))
hEngine = c_void_p()
ret = mainlib.AFD_FSDK_InitialFaceEngine(APKID, SDKKEY, pWorkMem, c_int32(WORKBUF_SIZE), byref(hEngine),c_int32(5),c_int32(16),c_int32(50))
if ret != 0:
	print ("AFD_FSDK_InitialFaceEngine ret == 0x{:x}".format(ret))
	exit(0)

pVersionInfo = mainlib.AFD_FSDK_GetVersion(hEngine)
print ('{} {} {} {}'.format(pVersionInfo.contents.lCodebase, pVersionInfo.contents.lMajor, pVersionInfo.contents.lMinor, pVersionInfo.contents.lBuild))
print ('{}'.format(c_char_p(pVersionInfo.contents.Version).value.decode('utf-8')))
print ('{}'.format(c_char_p(pVersionInfo.contents.BuildDate).value.decode('utf-8')))
print ('{}'.format(c_char_p(pVersionInfo.contents.CopyRight).value.decode('utf-8')))

ASVL_PAF_I420 = 0x601
INPUT_IMAGE_FORMAT = ASVL_PAF_I420
INPUT_IMAGE_PATH  = "003_640x480_I420.YUV"
INPUT_IMAGE_WIDTH  =  640
INPUT_IMAGE_HEIGHT =  480

f = open(INPUT_IMAGE_PATH,"rb");
in_imdata = f.read(os.path.getsize(INPUT_IMAGE_PATH))
f.close();

inputImage = ASVLOFFSCREEN()
inputImage.u32PixelArrayFormat = INPUT_IMAGE_FORMAT
inputImage.i32Width = INPUT_IMAGE_WIDTH
inputImage.i32Height = INPUT_IMAGE_HEIGHT
inputImage.pi32Pitch[0] = inputImage.i32Width
inputImage.pi32Pitch[1] = inputImage.i32Width>>1
inputImage.pi32Pitch[2] = inputImage.i32Width>>1
inputImage.pi32Pitch[3] = 0
inputImage.ppu8Plane[0] = cast(in_imdata,c_ubyte_p)
inputImage.ppu8Plane[1] = cast(addressof(inputImage.ppu8Plane[0].contents)+(inputImage.pi32Pitch[0]*inputImage.i32Height),c_ubyte_p)
inputImage.ppu8Plane[2] = cast(addressof(inputImage.ppu8Plane[1].contents)+(inputImage.pi32Pitch[1]*inputImage.i32Height>>1),c_ubyte_p)
inputImage.ppu8Plane[3] = cast(0,c_ubyte_p)

pFaceRes = POINTER(AFD_FSDK_FACERES)()
ret = mainlib.AFD_FSDK_StillImageFaceDetection(hEngine, byref(inputImage),byref(pFaceRes))
if ret != 0:
	print ("AFD_FSDK_StillImageFaceDetection ret == 0x{:x}".format(ret))
	exit(0)

for i in range(0, pFaceRes.contents.nFace):
    print("face {:d}:({:d},{:d},{:d},{:d}) {:d}".format(i, \
    	pFaceRes.contents.rcFace[i].left, \
    	pFaceRes.contents.rcFace[i].top,\
    	pFaceRes.contents.rcFace[i].right, \
    	pFaceRes.contents.rcFace[i].bottom,pFaceRes.contents.lfaceOrient[i]))

mainlib.AFD_FSDK_UninitialFaceEngine(hEngine)
libc.free(pWorkMem);
exit(0)
