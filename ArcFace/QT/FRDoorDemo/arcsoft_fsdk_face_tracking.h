/*******************************************************************************
* Copyright(c) ArcSoft, All right reserved.
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary
* and confidential information.
*
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER
* AUTHORIZATION.
*
* If you are not an intended recipient of this file, you must not copy,
* distribute, modify, or take any action in reliance on it.
*
* If you have received this file in error, please immediately notify ArcSoft and
* permanently delete the original and any copy of any file and any printout
* thereof.
*********************************************************************************/

#ifndef _ARCSOFT_FSDK_FACETRACKING_H_
#define _ARCSOFT_FSDK_FACETRACKING_H_

#include "amcomdef.h"
#include "asvloffscreen.h"


#ifdef __cplusplus
extern "C" {
#endif

    typedef MInt32 AFT_FSDK_OrientPriority;
    typedef MInt32 AFT_FSDK_OrientCode;

    /*******************************************************************************************
     FaceTracking Orientation Priority
     *******************************************************************************************/
    enum _AFT_FSDK_OrientPriority {
        AFT_FSDK_OPF_0_ONLY = 0x1,       // 0, 0, ...
        AFT_FSDK_OPF_90_ONLY = 0x2,      // 90, 90, ...
        AFT_FSDK_OPF_270_ONLY = 0x3,     // 270, 270, ...
        AFT_FSDK_OPF_180_ONLY = 0x4,     // 180, 180, ...
        AFT_FSDK_OPF_0_HIGHER_EXT = 0x5, // 0, 90, 270, 180, 0, 90, 270, 180, ...
    };

    /*******************************************************************************************
     FaceTracking Face Orientation
     *******************************************************************************************/
    enum _AFT_FSDK_OrientCode {
        AFT_FSDK_FOC_0 = 0x1,   // 0 degree
        AFT_FSDK_FOC_90 = 0x2,  // 90 degree
        AFT_FSDK_FOC_270 = 0x3, // 270 degree
        AFT_FSDK_FOC_180 = 0x4  // 180 degree
    };

    /*******************************************************************************************
     FaceTracking Face Information
     *******************************************************************************************/

    typedef struct {
        MInt32 nFace;                        // number of faces detected
        AFT_FSDK_OrientCode lfaceOrient;     // the face angle
        MRECT *rcFace;                       // The bounding box of face
    } AFT_FSDK_FACERES, *LPAFT_FSDK_FACERES;

    /*******************************************************************************************
     FaceTracking Version Information
     *******************************************************************************************/
    typedef struct {
        MInt32 lCodebase; // Codebase version number
        MInt32 lMajor;    // Major version number
        MInt32 lMinor;    // Minor version number
        MInt32 lBuild;    // Build version number, increasable only
        MPChar Version;   // Version in string form
        MPChar BuildDate; // Latest build Date
        MPChar CopyRight; // Copyright
    } AFT_FSDK_Version;

    /************************************************************************
     * The function used to Initialize the face tracking engine.
     ************************************************************************/
    MRESULT AFT_FSDK_InitialFaceEngine(
        MPChar AppId,					               // [in]  APPID
        MPChar SDKKey,						           // [in]  SDKKEY
        MByte *pMem,						           // [in]	User allocated memory for the engine
        MInt32 lMemSize,					           // [in]	User allocated memory size
        MHandle *phEngine,                             // [out] Pointing to the tracking engine
        AFT_FSDK_OrientPriority iOrientPriority,       // [in]  Defining the priority of face orientation.
        MInt32 nScale,                                 // [in]  An integer defining the minimal face to detect
        MInt32 nMaxFaceNum                             // [in]  An integer defining the number of max faces
        );

    /************************************************************************
     * The function used to detect and track face automatically.
     * Comment:
     *	The incoming image is scanned for faces.The result pFaceRes will be
     *  passed to this interface in the next calling round.
     ************************************************************************/
    MRESULT AFT_FSDK_FaceFeatureDetect(
        MHandle hEngine,                // [in]  The face tracking engine
        LPASVLOFFSCREEN pImgData,       // [in]  The original image data
        LPAFT_FSDK_FACERES *pFaceRes    // [out] The tracking result
        );

    /************************************************************************
     * The function used to Uninitialize the tracking module.
     ************************************************************************/
    MRESULT AFT_FSDK_UninitialFaceEngine(
        MHandle hEngine           // [in] The face tracking engine
        );

    /************************************************************************
     * The function used to get version information of face tracking library.
     ************************************************************************/
    const AFT_FSDK_Version *AFT_FSDK_GetVersion(MHandle hEngine);

#ifdef __cplusplus
}
#endif

#endif //_ARC_FACETRACKING_H_