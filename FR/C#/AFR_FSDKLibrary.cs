using System;
using System.Runtime.InteropServices;


namespace arcsoft {
    public class AFR_FSDKLibrary {
        [DllImport("libarcsoft_fsdk_face_recognition", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFR_FSDK_InitialEngine(
            [In] String appid,
            [In] String sdkid,
            IntPtr pMem,
            int lMemSize,
            ref IntPtr phEngine
            );

        [DllImport("libarcsoft_fsdk_face_recognition", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFR_FSDK_ExtractFRFeature(
            IntPtr hEngine,
            ref ASVLOFFSCREEN pImgData,
            ref AFR_FSDK_FACEINPUT pFaceRes,
            ref AFR_FSDK_FACEMODEL pFaceModels
            );

        [DllImport("libarcsoft_fsdk_face_recognition", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFR_FSDK_FacePairMatching(
            IntPtr hEngine,
            ref AFR_FSDK_FACEMODEL reffeature,
            ref AFR_FSDK_FACEMODEL probefeature,
            ref float pfSimilScore
            );

        [DllImport("libarcsoft_fsdk_face_recognition", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFR_FSDK_UninitialEngine(
            IntPtr hEngine
            );

        [DllImport("libarcsoft_fsdk_face_recognition", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AFR_FSDK_GetVersion(IntPtr hEngine);
    }
}
