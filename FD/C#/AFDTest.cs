using System;
using System.Runtime.InteropServices;
using System.IO;
namespace arcsoft
{
    class demo
    {
        private const string APPID = "XXXXXXXXXXXXXXXXXXXXXXXX";
        private const string SDKKEY = "YYYYYYYYYYYYYYYYYYYYYY";

        static void Main(string[] args)
        {

            int WORKBUF_SIZE = 40*1024*1024;
            IntPtr hEngine = new IntPtr(0);
            IntPtr pWorkMem = Marshal.AllocCoTaskMem(WORKBUF_SIZE);

            int ret = FDEngine.AFD_FSDK_InitialFaceEngine(APPID, SDKKEY, pWorkMem, WORKBUF_SIZE, ref(hEngine), 5, 16, 50);
            if (ret != 0) {
                Console.WriteLine("AFD_FSDK_InitialFaceEngine {0:X}", ret);
                return;
            }

            IntPtr pVersion_IntPtr = FDEngine.AFD_FSDK_GetVersion(hEngine);
            FDEngine.AFD_FSDK_Version pVersion = (FDEngine.AFD_FSDK_Version)Marshal.PtrToStructure(pVersion_IntPtr, typeof(FDEngine.AFD_FSDK_Version));
            Console.WriteLine("{0} {1} {2} {3}", pVersion.lCodebase, pVersion.lMajor,
                                                 pVersion.lMinor, pVersion.lBuild);
            Console.WriteLine("{0}", pVersion.Version);
            Console.WriteLine("{0}", pVersion.BuildDate);
            Console.WriteLine("{0}", pVersion.CopyRight);


            ASVLOFFSCREEN inputImg = new ASVLOFFSCREEN();
            inputImg.u32PixelArrayFormat = 0x601;
            inputImg.i32Width = 640;
            inputImg.i32Height = 480;
            inputImg.pi32Pitch = new int[4];
            inputImg.pi32Pitch[0] = inputImg.i32Width;
            inputImg.pi32Pitch[1] = inputImg.i32Width / 2;
            inputImg.pi32Pitch[2] = inputImg.i32Width / 2;
            inputImg.pi32Pitch[3] = 0;
 
            try
            {
                BinaryReader br = new BinaryReader(new FileStream("003_640x480_I420.YUV", FileMode.Open));
                byte[] rawYUVData = br.ReadBytes(inputImg.pi32Pitch[0] * inputImg.i32Height*3/2);
                IntPtr unmanagedPointer = Marshal.AllocHGlobal(rawYUVData.Length);
                Marshal.Copy(rawYUVData, 0, unmanagedPointer, rawYUVData.Length);
                inputImg.ppu8Plane = new IntPtr[4];
                inputImg.ppu8Plane[0] = unmanagedPointer;
                inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
                inputImg.ppu8Plane[2] = inputImg.ppu8Plane[1] + (inputImg.pi32Pitch[1] * inputImg.i32Height / 2);
                inputImg.ppu8Plane[3] = IntPtr.Zero;
                br.Close();

            }
            catch (IOException e)
            {
                Console.WriteLine(e.Message);
                return;
            }

            IntPtr faceResult_IntPtr = IntPtr.Zero;
            ret = FDEngine.AFD_FSDK_StillImageFaceDetection(hEngine, ref(inputImg), ref(faceResult_IntPtr));
            if (ret != 0)
            {
                Console.WriteLine("AFD_FSDK_StillImageFaceDetection {0:X}", ret);
                return;
            }

            FDEngine.AFD_FSDK_FACERES faceResult = (FDEngine.AFD_FSDK_FACERES)Marshal.PtrToStructure(faceResult_IntPtr, typeof(FDEngine.AFD_FSDK_FACERES));
            int sizeOfMRECT = Marshal.SizeOf(typeof(MRECT));
            
            for (int i = 0; i < faceResult.nFace; i++)
            {
                MRECT rect = (MRECT)Marshal.PtrToStructure(faceResult.rcFace + i * sizeOfMRECT, typeof(MRECT));

                Console.WriteLine("face {0}:({1},{2},{3},{4})", i,
                                    rect.left, rect.top,
                                    rect.right, rect.bottom);

            }
            FDEngine.AFD_FSDK_UninitialFaceEngine(hEngine);

            Marshal.FreeHGlobal(inputImg.ppu8Plane[0]);
            Marshal.FreeCoTaskMem(pWorkMem);

        }
    }
}
