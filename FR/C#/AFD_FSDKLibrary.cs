using System;
using System.Runtime.InteropServices;

namespace arcsoft {
    public class AFD_FSDKLibrary {
        [DllImport("libarcsoft_fsdk_face_detection", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFD_FSDK_InitialFaceEngine(
              [In] string AppId,
              [In] string SDKKey,
              IntPtr pMem,
              int lMemSize,
              ref IntPtr phEngine,
              int iOrientPriority,
              int nScale,
              int nMaxFaceNum
              );

        [DllImport("libarcsoft_fsdk_face_detection", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFD_FSDK_StillImageFaceDetection(
            IntPtr hEngine,
            ref ASVLOFFSCREEN pImgData,
            ref IntPtr pFaceRes
         );

        [DllImport("libarcsoft_fsdk_face_detection", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFD_FSDK_UninitialFaceEngine(
            IntPtr hEngine
         );


        [DllImport("libarcsoft_fsdk_face_detection", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFD_FSDK_GetVersion(IntPtr hEngine);

    }
}
