/*----------------------------------------------------------------------------------------------
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary and
* confidential information.
*
* The information and code contained in this file is only for authorized ArcSoft employees
* to design, create, modify, or review.
*
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
*
* If you are not an intended recipient of this file, you must not copy, distribute, modify,
* or take any action in reliance on it.
*
* If you have received this file in error, please immediately notify ArcSoft and
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/


#ifndef __MERROR_H__
#define __MERROR_H__



#define MERR_NONE						(0)
#define MOK								(0)



#define MERR_BASIC_BASE					0X0001
#define MERR_UNKNOWN					MERR_BASIC_BASE
#define MERR_INVALID_PARAM				(MERR_BASIC_BASE+1)
#define MERR_UNSUPPORTED				(MERR_BASIC_BASE+2)
#define MERR_NO_MEMORY					(MERR_BASIC_BASE+3)
#define MERR_BAD_STATE					(MERR_BASIC_BASE+4)
#define MERR_USER_CANCEL				(MERR_BASIC_BASE+5)
#define MERR_EXPIRED					(MERR_BASIC_BASE+6)
#define MERR_USER_PAUSE					(MERR_BASIC_BASE+7)
#define MERR_BUFFER_OVERFLOW		    (MERR_BASIC_BASE+8)
#define MERR_BUFFER_UNDERFLOW		    (MERR_BASIC_BASE+9)
#define MERR_NO_DISKSPACE				(MERR_BASIC_BASE+10)
#define	MERR_COMPONENT_NOT_EXIST		(MERR_BASIC_BASE+11)
#define	MERR_GLOBAL_DATA_NOT_EXIST		(MERR_BASIC_BASE+12)



#define MERR_FSDK_BASE						0X7000
#define MERR_FSDK_INVALID_APP_ID			(MERR_FSDK_BASE + 1)
#define MERR_FSDK_INVALID_SDK_ID			(MERR_FSDK_BASE + 2)
#define MERR_FSDK_INVALID_ID_PAIR			(MERR_FSDK_BASE + 3)
#define MERR_FSDK_MISMATCH_ID_AND_SDK		(MERR_FSDK_BASE + 4)
#define MERR_FSDK_SYSTEM_VERSION_UNSUPPORTED	(MERR_FSDK_BASE + 5)
#define MERR_FSDK_LICENCE_EXPIRED           (MERR_FSDK_BASE + 6)



#define MERR_FSDK_APS_ERROR_BASE				0x11000
#define MERR_FSDK_APS_ENGINE_HANDLE				(MERR_FSDK_APS_ERROR_BASE+1)
#define MERR_FSDK_APS_MEMMGR_HANDLE				(MERR_FSDK_APS_ERROR_BASE+2)
#define MERR_FSDK_APS_DEVICEID_INVALID			(MERR_FSDK_APS_ERROR_BASE+3)
#define MERR_FSDK_APS_DEVICEID_UNSUPPORTED		(MERR_FSDK_APS_ERROR_BASE+4)
#define MERR_FSDK_APS_MODEL_HANDLE				(MERR_FSDK_APS_ERROR_BASE+5)
#define MERR_FSDK_APS_MODEL_SIZE				(MERR_FSDK_APS_ERROR_BASE+6)
#define MERR_FSDK_APS_IMAGE_HANDLE              (MERR_FSDK_APS_ERROR_BASE+7)
#define MERR_FSDK_APS_IMAGE_FORMAT_UNSUPPORTED  (MERR_FSDK_APS_ERROR_BASE+8)
#define MERR_FSDK_APS_IMAGE_PARAM               (MERR_FSDK_APS_ERROR_BASE+9)
#define MERR_FSDK_APS_IMAGE_SIZE				(MERR_FSDK_APS_ERROR_BASE+10)
#define MERR_FSDK_APS_DEVICE_AVX2_UNSUPPORTED	(MERR_FSDK_APS_ERROR_BASE+11)


#define MERR_FSDK_FR_ERROR_BASE					0x12000
#define MERR_FSDK_FR_INVALID_MEMORY_INFO		(MERR_FSDK_FR_ERROR_BASE+1)
#define MERR_FSDK_FR_INVALID_IMAGE_INFO			(MERR_FSDK_FR_ERROR_BASE+2)
#define MERR_FSDK_FR_INVALID_FACE_INFO			(MERR_FSDK_FR_ERROR_BASE+3)
#define MERR_FSDK_FR_NO_GPU_AVAILABLE			(MERR_FSDK_FR_ERROR_BASE+4)
#define MERR_FSDK_FR_MISMATCHED_FEATURE_LEVEL	(MERR_FSDK_FR_ERROR_BASE+5)

#endif

