using System;
using System.Runtime.InteropServices;


namespace arcsoft
{
    public class FDEngine
    {
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct AFD_FSDK_Version
        {
            public int lCodebase;
            public int lMajor;
            public int lMinor;
            public int lBuild;
            public string Version;
            public string BuildDate;
            public string CopyRight;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct AFD_FSDK_FACERES
        {
            public int nFace;
            public IntPtr rcFace;
            public IntPtr lfaceOrient;
        };


        [DllImport("libarcsoft_fsdk_face_detection.dll")]
        public static extern int AFD_FSDK_InitialFaceEngine(
              [In] string AppId,
              [In] string SDKKey,
              IntPtr pMem,
              int lMemSize,
              ref IntPtr phEngine,
              int iOrientPriority,
              int nScale,
              int nMaxFaceNum
              );

        [DllImport("libarcsoft_fsdk_face_detection.dll")]
        public static extern int AFD_FSDK_UninitialFaceEngine(
            IntPtr hEngine
         );


        [DllImport("libarcsoft_fsdk_face_detection.dll", CharSet = CharSet.Ansi)]
        public static extern IntPtr AFD_FSDK_GetVersion(IntPtr hEngine);


        [DllImport("libarcsoft_fsdk_face_detection.dll")]
        public static extern int AFD_FSDK_StillImageFaceDetection(
            IntPtr hEngine,
            ref ASVLOFFSCREEN pImgData,
            ref IntPtr pFaceRes
         );
    }
}
