package com.arcsoft;

import com.sun.jna.Library;
import com.sun.jna.NativeLong;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;

public interface AFD_FSDKLibrary extends Library {
    
    AFD_FSDKLibrary INSTANCE = (AFD_FSDKLibrary)LoadUtils.loadLibrary("libarcsoft_fsdk_face_detection.dll",AFD_FSDKLibrary.class);
    
    NativeLong AFD_FSDK_InitialFaceEngine(String appid, String sdkid, Pointer pMem, int lMemSize, PointerByReference phEngine, int iOrientPriority, int nScale, int nMaxFaceNum);

    NativeLong AFD_FSDK_StillImageFaceDetection(Pointer hEngine, ASVLOFFSCREEN pImgData, PointerByReference pFaceRes);

    NativeLong AFD_FSDK_UninitialFaceEngine(Pointer hEngine);

    AFD_FSDK_Version AFD_FSDK_GetVersion(Pointer hEngine);
}