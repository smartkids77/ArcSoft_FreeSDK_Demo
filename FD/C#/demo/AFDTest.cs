using System;
using System.Runtime.InteropServices;
using System.IO;
using arcsoft.utils;

namespace arcsoft
{
    namespace demo
    {
        public class AFDTest
        {
            //public const string APPID     = "XXXXXXXXXX";
            //public const string FD_SDKKEY = "YYYYYYYYYY";
            
            public const int FD_WORKBUF_SIZE = 20 * 1024 * 1024;
            public const int MAX_FACE_NUM = 50;

            public const bool bUseRAWFile = false;
            public const bool bUseBGRToEngine = true;

            public static void Main(string[] args)
            {
                Console.WriteLine("#####################################################");

                //init Engine
                IntPtr pFDWorkMem = Marshal.AllocCoTaskMem(FD_WORKBUF_SIZE);

                IntPtr hFDEngine = new IntPtr(0);
                IntPtr ret = AFD_FSDKLibrary.AFD_FSDK_InitialFaceEngine(APPID, FD_SDKKEY, pFDWorkMem, FD_WORKBUF_SIZE, ref(hFDEngine), _AFD_FSDK_OrientPriority.AFD_FSDK_OPF_0_HIGHER_EXT, 32, MAX_FACE_NUM);
                if (ret.ToInt64() != 0)
                {
                    Marshal.FreeCoTaskMem(pFDWorkMem);
                    Console.WriteLine(String.Format("AFD_FSDK_InitialFaceEngine 0x{0:x}", ret));
                    Environment.Exit(0);
                }

                //print FDEngine version
                AFD_FSDK_Version versionFD = (AFD_FSDK_Version)Marshal.PtrToStructure(AFD_FSDKLibrary.AFD_FSDK_GetVersion(hFDEngine), typeof(AFD_FSDK_Version));
                Console.WriteLine(String.Format("{0} {1} {2} {3}", versionFD.lCodebase, versionFD.lMajor, versionFD.lMinor, versionFD.lBuild));
                Console.WriteLine(versionFD.Version);
                Console.WriteLine(versionFD.BuildDate);
                Console.WriteLine(versionFD.CopyRight);

                //load Image Data
                ASVLOFFSCREEN inputImg;
                if (bUseRAWFile)
                {
                    String filePath = "001_640x480_I420.YUV";
                    int yuv_width = 640;
                    int yuv_height = 480;
                    int yuv_format = ASVL_COLOR_FORMAT.ASVL_PAF_I420;

                    inputImg = loadRAWImage(filePath, yuv_width, yuv_height, yuv_format);
                }
                else
                {
                    String filePath = "003.jpg";

                    inputImg = loadImage(filePath);
                }

                //do Face Detect
                FaceInfo[] faceInfos = doFaceDetection(hFDEngine, inputImg);
                for (int i = 0; i < faceInfos.Length; i++)
                {
                    FaceInfo rect = faceInfos[i];
                    Console.WriteLine(String.Format("{0} ({1} {2} {3} {4}) orient {5}", i, rect.left, rect.top, rect.right, rect.bottom, rect.orient));
                }

                inputImg.freeUnmanaged();

                //release Engine
                AFD_FSDKLibrary.AFD_FSDK_UninitialFaceEngine(hFDEngine);

                Marshal.FreeCoTaskMem(pFDWorkMem);

                Console.WriteLine("#####################################################");

            }

            public static FaceInfo[] doFaceDetection(IntPtr hFDEngine, ASVLOFFSCREEN inputImg)
            {
                FaceInfo[] faceInfo = new FaceInfo[0];

                IntPtr pFaceRes = IntPtr.Zero;
                IntPtr ret = AFD_FSDKLibrary.AFD_FSDK_StillImageFaceDetection(hFDEngine, ref(inputImg), ref(pFaceRes));
                if (ret.ToInt64() != 0)
                {
                    Console.WriteLine(String.Format("AFD_FSDK_StillImageFaceDetection 0x{0:x}", ret));
                    return faceInfo;
                }

                AFD_FSDK_FACERES faceRes = (AFD_FSDK_FACERES)Marshal.PtrToStructure(pFaceRes, typeof(AFD_FSDK_FACERES));
                if (faceRes.nFace > 0)
                {
                    faceInfo = new FaceInfo[faceRes.nFace];
                    for (int i = 0; i < faceRes.nFace; i++)
                    {
                        MRECT rect = (MRECT)Marshal.PtrToStructure(faceRes.rcFace + i * Marshal.SizeOf(typeof(MRECT)), typeof(MRECT));
                        int orient = Marshal.ReadInt32(faceRes.lfaceOrient + 4 * i);
                        faceInfo[i] = new FaceInfo();

                        faceInfo[i].left = rect.left;
                        faceInfo[i].top = rect.top;
                        faceInfo[i].right = rect.right;
                        faceInfo[i].bottom = rect.bottom;
                        faceInfo[i].orient = orient;
                    }
                }
                return faceInfo;
            }

            public static ASVLOFFSCREEN loadRAWImage(String yuv_filePath, int yuv_width, int yuv_height, int yuv_format)
            {
                int yuv_rawdata_size = 0;


                ASVLOFFSCREEN inputImg = new ASVLOFFSCREEN();
                inputImg.pi32Pitch = new int[4];
                inputImg.ppu8Plane = new IntPtr[4];
                inputImg.u32PixelArrayFormat = yuv_format;
                inputImg.i32Width = yuv_width;
                inputImg.i32Height = yuv_height;
                if (ASVL_COLOR_FORMAT.ASVL_PAF_I420 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.pi32Pitch[0] = inputImg.i32Width;
                    inputImg.pi32Pitch[1] = inputImg.i32Width / 2;
                    inputImg.pi32Pitch[2] = inputImg.i32Width / 2;
                    yuv_rawdata_size = inputImg.i32Width * inputImg.i32Height * 3 / 2;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.pi32Pitch[0] = inputImg.i32Width;
                    inputImg.pi32Pitch[1] = inputImg.i32Width;
                    yuv_rawdata_size = inputImg.i32Width * inputImg.i32Height * 3 / 2;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.pi32Pitch[0] = inputImg.i32Width;
                    inputImg.pi32Pitch[1] = inputImg.i32Width;
                    yuv_rawdata_size = inputImg.i32Width * inputImg.i32Height * 3 / 2;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat)
                {
                    inputImg.pi32Pitch[0] = inputImg.i32Width * 2;
                    yuv_rawdata_size = inputImg.i32Width * inputImg.i32Height * 2;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_RGB24_B8G8R8 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.pi32Pitch[0] = inputImg.i32Width * 3;
                    yuv_rawdata_size = inputImg.i32Width * inputImg.i32Height * 3;
                }
                else
                {
                    Console.WriteLine("unsupported  yuv format");
                    Environment.Exit(0);
                }

                //load YUV Image Data from File
                byte[] imagedata = new byte[yuv_rawdata_size];
                FileStream f = new FileStream(yuv_filePath, FileMode.Open);
                BinaryReader br = null;
                try
                {
                    br = new BinaryReader(f);
                    br.Read(imagedata, 0, yuv_rawdata_size);

                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                    Console.WriteLine(e.Message);
                    Environment.Exit(0);
                }
                finally
                {
                    try
                    {
                        if (br != null)
                        {
                            br.Close();
                        }
                    }
                    catch (Exception e)
                    {
                    }
                }

                if (ASVL_COLOR_FORMAT.ASVL_PAF_I420 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(imagedata, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = Marshal.AllocHGlobal(inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    Marshal.Copy(imagedata, inputImg.pi32Pitch[0] * inputImg.i32Height, inputImg.ppu8Plane[1], inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    inputImg.ppu8Plane[2] = Marshal.AllocHGlobal(inputImg.pi32Pitch[2] * inputImg.i32Height / 2);
                    Marshal.Copy(imagedata, inputImg.pi32Pitch[0] * inputImg.i32Height + inputImg.pi32Pitch[1] * inputImg.i32Height / 2, inputImg.ppu8Plane[2], inputImg.pi32Pitch[2] * inputImg.i32Height / 2);
                    inputImg.ppu8Plane[3] = IntPtr.Zero;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(imagedata, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = Marshal.AllocHGlobal(inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    Marshal.Copy(imagedata, inputImg.pi32Pitch[0] * inputImg.i32Height, inputImg.ppu8Plane[1], inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    inputImg.ppu8Plane[2] = IntPtr.Zero;
                    inputImg.ppu8Plane[3] = IntPtr.Zero;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(imagedata, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = Marshal.AllocHGlobal(inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    Marshal.Copy(imagedata, inputImg.pi32Pitch[0] * inputImg.i32Height, inputImg.ppu8Plane[1], inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    inputImg.ppu8Plane[2] = IntPtr.Zero;
                    inputImg.ppu8Plane[3] = IntPtr.Zero;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat)
                {
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(imagedata, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = IntPtr.Zero;
                    inputImg.ppu8Plane[2] = IntPtr.Zero;
                    inputImg.ppu8Plane[3] = IntPtr.Zero;
                }
                else if (ASVL_COLOR_FORMAT.ASVL_PAF_RGB24_B8G8R8 == inputImg.u32PixelArrayFormat)
                {
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(imagedata, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = IntPtr.Zero;
                    inputImg.ppu8Plane[2] = IntPtr.Zero;
                    inputImg.ppu8Plane[3] = IntPtr.Zero;
                }
                else
                {
                    Console.WriteLine("unsupported yuv format");
                    Environment.Exit(0);
                }

                return inputImg;
            }

            public static ASVLOFFSCREEN loadImage(String filePath)
            {

                ASVLOFFSCREEN inputImg = new ASVLOFFSCREEN();
                inputImg.pi32Pitch = new int[4];
                inputImg.ppu8Plane = new IntPtr[4];
                if (bUseBGRToEngine)
                {
                    BufferInfo bufferInfo = ImageLoader.getBGRFromFile(filePath);
                    inputImg.u32PixelArrayFormat = ASVL_COLOR_FORMAT.ASVL_PAF_RGB24_B8G8R8;
                    inputImg.i32Width = bufferInfo.width;
                    inputImg.i32Height = bufferInfo.height;
                    inputImg.pi32Pitch[0] = bufferInfo.stride;
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(bufferInfo.buffer, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = IntPtr.Zero;
                    inputImg.ppu8Plane[2] = IntPtr.Zero;
                    inputImg.ppu8Plane[3] = IntPtr.Zero;
                }
                else
                {
                    BufferInfo bufferInfo = ImageLoader.getI420FromFile(filePath);
                    inputImg.u32PixelArrayFormat = ASVL_COLOR_FORMAT.ASVL_PAF_I420;
                    inputImg.i32Width = bufferInfo.width;
                    inputImg.i32Height = bufferInfo.height;
                    inputImg.pi32Pitch[0] = inputImg.i32Width;
                    inputImg.pi32Pitch[1] = inputImg.i32Width / 2;
                    inputImg.pi32Pitch[2] = inputImg.i32Width / 2;
                    inputImg.ppu8Plane[0] = Marshal.AllocHGlobal(inputImg.pi32Pitch[0] * inputImg.i32Height);
                    Marshal.Copy(bufferInfo.buffer, 0, inputImg.ppu8Plane[0], inputImg.pi32Pitch[0] * inputImg.i32Height);
                    inputImg.ppu8Plane[1] = Marshal.AllocHGlobal(inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    Marshal.Copy(bufferInfo.buffer, inputImg.pi32Pitch[0] * inputImg.i32Height, inputImg.ppu8Plane[1], inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                    inputImg.ppu8Plane[2] = Marshal.AllocHGlobal(inputImg.pi32Pitch[2] * inputImg.i32Height / 2);
                    Marshal.Copy(bufferInfo.buffer, inputImg.pi32Pitch[0] * inputImg.i32Height + inputImg.pi32Pitch[1] * inputImg.i32Height / 2, inputImg.ppu8Plane[2], inputImg.pi32Pitch[2] * inputImg.i32Height / 2);
                    inputImg.ppu8Plane[3] = IntPtr.Zero;

                }
                return inputImg;
            }

        }
    }
}

