from ctypes import *
import os.path
from PIL import Image
import platform

#APPID = c_char_p(u"XXXXXXXXXX".encode('utf-8'))
#SDKKEY = c_char_p(u"YYYYYYYYY".encode('utf-8'))
INPUT_IMAGE_PATH = "s_1920x1080.bmp"
STYLE_MODEL_PATH = "CHINESEART-001.tmpl"
OUTPUT_IMAGE_PATH = "s_1920x1080_result.bmp"

c_ubyte_p = POINTER(c_ubyte) 
class APS_FSDK_Version(Structure):
    _fields_ = [("lCodebase",c_long),("lMajor",c_long),("lMinor",c_long),("lBuild",c_long),
                ("Version",c_char_p),("BuildDate",c_char_p),("CopyRight",c_char_p)]
    
class ASVLOFFSCREEN(Structure):
    _fields_ = [("u32PixelArrayFormat",c_uint32),("i32Width",c_int32),("i32Height",c_int32),
                ("ppu8Plane",c_char_p*4),("pi32Pitch",c_int32*4)]

if platform.system() == 'Windows':
    mainlib = CDLL(os.path.dirname(os.path.abspath(__file__)) + os.path.sep + "libarcsoft_fsdk_aps.dll")
    libc = cdll.msvcrt
else:
    mainlib = CDLL(os.path.dirname(os.path.abspath(__file__)) + os.path.sep + "libarcsoft_fsdk_aps.so")
    libc = mainlib

libc.malloc.restype = c_void_p
libc.malloc.argtypes =(c_size_t,)
libc.free.argtypes = (c_void_p,)

mainlib.APS_FSDK_Get_Version.restype = POINTER(APS_FSDK_Version)
mainlib.APS_FSDK_Get_Version.argtypes =(c_void_p,)
mainlib.MMemMgrCreate.restype = c_void_p
mainlib.MMemMgrCreate.argtypes = (c_void_p,c_long,)
mainlib.APS_FSDK_InitEngine.restype = c_long
mainlib.APS_FSDK_InitEngine.argtypes = (c_char_p,c_char_p,c_void_p,c_int32,POINTER(c_void_p))
mainlib.APS_FSDK_UninitEngine.argtypes = (c_void_p,)
mainlib.APS_FSDK_StyleTransfer.restype = c_long
mainlib.APS_FSDK_StyleTransfer.argtypes = (c_void_p,c_char_p,c_long,POINTER(ASVLOFFSCREEN),POINTER(ASVLOFFSCREEN))

WORKBUF_SIZE = 512*1024*1024
ASVL_PAF_RGB24_B8G8R8 = 0x201
ASVL_PAF_RGB24_R8G8B8 = 0x204
color_format = ASVL_PAF_RGB24_R8G8B8

pWorkMem = libc.malloc(c_size_t(WORKBUF_SIZE))
hMgr = mainlib.MMemMgrCreate(pWorkMem, c_long(WORKBUF_SIZE))
hEngine = c_void_p()
ret = mainlib.APS_FSDK_InitEngine(APPID, SDKKEY, hMgr, c_int32(-1), byref(hEngine))
if ret != 0:
    print ("APS_FSDK_InitEngine ret == 0x{:x}".format(ret))
    exit(0)

pVersionInfo = mainlib.APS_FSDK_Get_Version(hEngine)
print ('{} {} {} {}'.format(pVersionInfo.contents.lCodebase, pVersionInfo.contents.lMajor, pVersionInfo.contents.lMinor, pVersionInfo.contents.lBuild))
print ('{}'.format(c_char_p(pVersionInfo.contents.Version).value.decode('utf-8')))
print ('{}'.format(c_char_p(pVersionInfo.contents.BuildDate).value.decode('utf-8')))
print ('{}'.format(c_char_p(pVersionInfo.contents.CopyRight).value.decode('utf-8')))

in_im = Image.open(INPUT_IMAGE_PATH).transpose(Image.FLIP_TOP_BOTTOM)
if color_format == ASVL_PAF_RGB24_B8G8R8:
    b, g, r = in_im.split()
    in_im = Image.merge("RGB", (r, g, b))
in_imdata = in_im.tobytes()

inputImage = ASVLOFFSCREEN()
inputImage.u32PixelArrayFormat = color_format
inputImage.i32Width = in_im.size[0]
inputImage.i32Height = in_im.size[1]
inputImage.pi32Pitch[0] = inputImage.i32Width * 3
inputImage.pi32Pitch[1] = 0
inputImage.pi32Pitch[2] = 0
inputImage.pi32Pitch[3] = 0
inputImage.ppu8Plane[0] = c_char_p(in_imdata)
inputImage.ppu8Plane[1] = cast(0,c_char_p)
inputImage.ppu8Plane[2] = cast(0,c_char_p)
inputImage.ppu8Plane[3] = cast(0,c_char_p)

out_imdata = create_string_buffer(in_im.size[0]*in_im.size[1]*3)
outputImage = ASVLOFFSCREEN()
outputImage.u32PixelArrayFormat = ASVL_PAF_RGB24_R8G8B8
outputImage.i32Width = in_im.size[0]
outputImage.i32Height = in_im.size[1]
outputImage.pi32Pitch[0] = outputImage.i32Width * 3
outputImage.pi32Pitch[1] = 0
outputImage.pi32Pitch[2] = 0
outputImage.pi32Pitch[3] = 0
outputImage.ppu8Plane[0] = cast(out_imdata,c_char_p)
outputImage.ppu8Plane[1] = cast(0,c_char_p)
outputImage.ppu8Plane[2] = cast(0,c_char_p)
outputImage.ppu8Plane[3] = cast(0,c_char_p)

style_model_data = open(STYLE_MODEL_PATH,"rb").read()
ret = mainlib.APS_FSDK_StyleTransfer(hEngine, style_model_data, len(style_model_data), byref(inputImage), byref(outputImage))
if ret != 0:
    print ("APS_FSDK_StyleTransfer ret == 0x{:x}".format(ret))
    exit(0)

out_im = Image.frombuffer("RGB", (outputImage.i32Width ,outputImage.i32Height ), out_imdata.raw, "raw")
if color_format == ASVL_PAF_RGB24_B8G8R8:
    b, g, r = out_im.split()
    out_im = Image.merge("RGB", (r, g, b))
out_im.save(OUTPUT_IMAGE_PATH)
mainlib.APS_FSDK_UninitEngine(hEngine)
libc.free(pWorkMem);
exit()
