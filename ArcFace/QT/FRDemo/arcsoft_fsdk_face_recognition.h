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

#ifndef _ARCSOFT_FSDK_FACERECOGNITION_H_
#define _ARCSOFT_FSDK_FACERECOGNITION_H_

#include "amcomdef.h"
#include "asvloffscreen.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef MInt32 AFR_FSDK_OrientCode;

    /*******************************************************************************************
    FaceRecognition Face Orientation
    *******************************************************************************************/
    enum _AFR_FSDK_OrientCode {
        AFR_FSDK_FOC_0 = 0x1,		// 0 degree
        AFR_FSDK_FOC_90 = 0x2,		// 90 degree
        AFR_FSDK_FOC_270 = 0x3,		// 270 degree
        AFR_FSDK_FOC_180 = 0x4,		// 180 degree
        AFR_FSDK_FOC_30 = 0x5,		// 30 degree
        AFR_FSDK_FOC_60 = 0x6,		// 60 degree
        AFR_FSDK_FOC_120 = 0x7,		// 120 degree
        AFR_FSDK_FOC_150 = 0x8,		// 150 degree
        AFR_FSDK_FOC_210 = 0x9,		// 210 degree
        AFR_FSDK_FOC_240 = 0xa,		// 240 degree
        AFR_FSDK_FOC_300 = 0xb,		// 300 degree
        AFR_FSDK_FOC_330 = 0xc		// 330 degree
    };

    /*******************************************************************************************
    FaceRecognition Face Information
    *******************************************************************************************/
    typedef struct {
        MRECT				rcFace;	// The bounding box of face
        AFR_FSDK_OrientCode	lOrient;// The orientation of face
    }AFR_FSDK_FACEINPUT, *LPAFR_FSDK_FACEINPUT;

    /*******************************************************************************************
    FaceRecognition Feature Information
    *******************************************************************************************/
    typedef struct {
        MByte		*pbFeature;		// The extracted features
        MInt32		lFeatureSize;	// The size of pbFeature	
    }AFR_FSDK_FACEMODEL, *LPAFR_FSDK_FACEMODEL;

    /*******************************************************************************************
    FaceRecognition Version Information
    *******************************************************************************************/
    typedef struct {
        MInt32 lCodebase;       // Codebase version number
        MInt32 lMajor;          // Major version number
        MInt32 lMinor;          // Minor version number
        MInt32 lBuild;          // Build version number, increasable only
        MInt32 lFeatureLevel;	// Feature level, used for judging update or not
        MPChar Version;         // Version in string form
        MPChar BuildDate;       // Latest build Date
        MPChar CopyRight;       // Copyright
    }AFR_FSDK_Version;

    /************************************************************************
     * Description:
     *	 The function will be used to Initialize the face recognition engine.
     * Return value:
     *	 Return MOK if success, otherwise fail
     ************************************************************************/
    MRESULT AFR_FSDK_InitialEngine(
        MPChar  AppId,      // [in]  APPID
        MPChar  SDKKey,     // [in]  SDKKEY
        MByte   *pMem,		// [in]	 User allocated memory for the engine
        MInt32	lMemSize,   // [in]	 User allocated memory size
        MHandle	*phEngine   // [out] Pointing to the face recognition engine
        );


    /************************************************************************
     * The function used to get one face's feature.
     *
     * Comment:
     *  The pFaceRes should be the results of face tracking or face detection
     *  The result will be loaded to pFaceModels.
     *
     ************************************************************************/
    MRESULT AFR_FSDK_ExtractFRFeature(
        MHandle              	hEngine,     // [in]  The face recognition engine
        LPASVLOFFSCREEN		    pInputImage, // [in]  The input face images used to enroll
        LPAFR_FSDK_FACEINPUT	pFaceRes,    // [in]  The faces'position and orientation
        LPAFR_FSDK_FACEMODEL	pFaceModels  // [out] The face feature information	
        );


    /************************************************************************
    * The function used to do face authentication, i.e. comparing two face if they are same person.
    ************************************************************************/
    MRESULT AFR_FSDK_FacePairMatching(
        MHandle             	hEngine,		// [in]  The face recognition engine
        AFR_FSDK_FACEMODEL		*reffeature,	// [in]  The reference face feature
        AFR_FSDK_FACEMODEL		*probefeature,  // [in]  The probing face feature.
        MFloat     			    *pfSimilScore	// [out] The authentication result.
        );


    /************************************************************************
     * The function used to uninitialize the face recognition engine.
     ************************************************************************/
    MRESULT AFR_FSDK_UninitialEngine(
        MHandle hEngine     // [in] The face recognition engine
        );


    const AFR_FSDK_Version *AFR_FSDK_GetVersion(MHandle hEngine);

#ifdef __cplusplus
}
#endif

#endif