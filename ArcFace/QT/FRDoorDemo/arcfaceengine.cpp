#include <QDebug>
#include <QElapsedTimer>
#include "arcfaceengine.h"

#include "arcsoft_fsdk_face_recognition.h"
#include "arcsoft_fsdk_face_tracking.h"
#include "key.h"

ArcFaceEngine::ArcFaceEngine()
{
	bEnable = false;
	mThreshold = 0.5f;

	mOldWidth = -1;
	mOldHeight = -1;
	mOldFormat = -1;

	mFaceNum = 0;
	for (int k = 0; k < MAX_FT_FACE; k++) {
		mFaceIndex[k] = -1;
	}

	mFTWorkMem = new unsigned char[FT_WORKMEM_SIZE];
	mFRWorkMem = new unsigned char[FR_WORKMEM_SIZE];
	hFTEngine = nullptr;
	hFREngine = nullptr;

	bFaceChange = true;

	mSemFRDone.release();
	mThreadFaceNum = 0;
	mThreadFrame = nullptr;
	bThreadRun = true;
	start();

#if 0
{
	const AFT_FSDK_Version *pVersionInfo = AFT_FSDK_GetVersion(nullptr);
	qDebug() << pVersionInfo->lCodebase << " " << pVersionInfo->lMajor << " " << pVersionInfo->lMinor << " " << pVersionInfo->lBuild;
	qDebug() << pVersionInfo->Version;
	qDebug() << pVersionInfo->BuildDate;
	qDebug() << pVersionInfo->CopyRight;
}
{
	const AFR_FSDK_Version *pVersionInfo = AFR_FSDK_GetVersion(nullptr);
	qDebug() << pVersionInfo->lCodebase << " " << pVersionInfo->lMajor << " " << pVersionInfo->lMinor << " " << pVersionInfo->lBuild;
	qDebug() << pVersionInfo->Version;
	qDebug() << pVersionInfo->BuildDate;
	qDebug() << pVersionInfo->CopyRight;
}
#endif
}

ArcFaceEngine::~ArcFaceEngine(){
	bThreadRun = false;
	mSemToDoFR.release();
	wait();

	if (hFTEngine) {
		AFT_FSDK_UninitialFaceEngine(hFTEngine);
		hFTEngine = nullptr;
	}

	if (hFREngine) {
		AFR_FSDK_UninitialEngine(hFREngine);
		hFREngine = nullptr;
	}

	if (mFTWorkMem) {
		delete[] mFTWorkMem;
		mFTWorkMem = nullptr;
	}

	if (mFRWorkMem) {
		delete[] mFRWorkMem;
		mFRWorkMem = nullptr;
	}

	if (mThreadFrame) {
		delete[] mThreadFrame;
		mThreadFrame = nullptr;
	}

	int facenum = mRegisterFaces.count();
	for (int i = 0; i < facenum; i++) {
        Face &face = mRegisterFaces[i];
        delete face.feature;
	}
	mRegisterFaces.clear();
}


void ArcFaceEngine::run(){
	while (bThreadRun) {
		mSemToDoFR.acquire();
		if (!bThreadRun) {
			break;
		}

		bool forceFlush = false;
		int countValidIndex = 0;
		for (int i = 0; i < mThreadFaceNum; i++) {
			if (mThreadFaceIndex[i] >= 0) {
				countValidIndex++;
			}
		}

		if (mThreadFaceNum == countValidIndex) {
			forceFlush = true;
		}

		for (int i = 0; i < mThreadFaceNum; i++) {
			if ((mThreadFaceIndex[i] < 0) || (forceFlush)) {
				ASVLOFFSCREEN inputImg = { 0 };
				inputImg.u32PixelArrayFormat = mThreadFrameFormat;
				inputImg.i32Width = mThreadFrameWidth;
				inputImg.i32Height = mThreadFrameHeight;
				inputImg.ppu8Plane[0] = mThreadFrame;
				if (ASVL_PAF_I420 == inputImg.u32PixelArrayFormat) {
					inputImg.pi32Pitch[0] = inputImg.i32Width;
					inputImg.pi32Pitch[1] = inputImg.i32Width / 2;
					inputImg.pi32Pitch[2] = inputImg.i32Width / 2;
					inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + inputImg.pi32Pitch[0] * inputImg.i32Height;
					inputImg.ppu8Plane[2] = inputImg.ppu8Plane[1] + inputImg.pi32Pitch[1] * inputImg.i32Height / 2;
				} else if (ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat) {
					inputImg.pi32Pitch[0] = inputImg.i32Width;
					inputImg.pi32Pitch[1] = inputImg.i32Width;
					inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
				} else if (ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat) {
					inputImg.pi32Pitch[0] = inputImg.i32Width;
					inputImg.pi32Pitch[1] = inputImg.i32Width;
					inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
				} else if (ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat) {
					inputImg.pi32Pitch[0] = inputImg.i32Width * 2;
				} else if (ASVL_PAF_RGB32_B8G8R8A8 == inputImg.u32PixelArrayFormat) {
					inputImg.pi32Pitch[0] = inputImg.i32Width * 4;
				} else {
					qDebug() << "unsupported Image format:" << inputImg.u32PixelArrayFormat;
				}

				AFR_FSDK_FACEINPUT faceInput;
				faceInput.lOrient = mThreadFaceOrient[i];
				faceInput.rcFace.left = mThreadFaceRect[i].left;
				faceInput.rcFace.top = mThreadFaceRect[i].top;
				faceInput.rcFace.right = mThreadFaceRect[i].right;
				faceInput.rcFace.bottom = mThreadFaceRect[i].bottom;
				AFR_FSDK_FACEMODEL localFaceModels = { 0 };

				//QElapsedTimer timer;
				//timer.start();

				int ret = AFR_FSDK_ExtractFRFeature(hFREngine, &inputImg, &faceInput, &localFaceModels);

				//qDebug() << "AFR_FSDK_ExtractFRFeature took" << timer.elapsed() << "milliseconds";

				if (ret == 0) {
					float fMaxScore = 0.0f;
					int iMaxIndex = -1;
					int facenum = mRegisterFaces.count();
					for (int j = 0; j < facenum; j++) {
                        Face  &face = mRegisterFaces[j];
						float fScore = 0.0f;
						AFR_FSDK_FACEMODEL targetFaceModels = { 0 };
                        targetFaceModels.lFeatureSize = face.size;
                        targetFaceModels.pbFeature = face.feature;
						ret = AFR_FSDK_FacePairMatching(hFREngine, &targetFaceModels, &localFaceModels, &fScore);
						if (ret == 0) {
							if (fScore > fMaxScore) {
								iMaxIndex = j;
								fMaxScore = fScore;
							}
						}
					}

					if ((iMaxIndex >= 0) && (fMaxScore > mThreshold)) {
						mThreadFaceIndex[i] = iMaxIndex;
						mThreadFaceName[i] = mRegisterFaces[iMaxIndex].name;
					}
				} else {
					//qDebug() << "AFR_FSDK_ExtractFRFeature failed";
				}

			}
		}

		mSemFRDone.release();
	}
}

bool ArcFaceEngine::processFrame(unsigned char *frameData, int frameWidth, int frameHeight, int frameFormat){

	if (!bEnable) {
		return false;
	}

	if ((mOldWidth != frameWidth) || (mOldHeight != frameHeight) || (mOldFormat != frameFormat)) {
		mOldWidth = frameWidth;
		mOldHeight = frameHeight;
		mOldFormat = frameFormat;
		if (hFTEngine) {
			AFT_FSDK_UninitialFaceEngine(hFTEngine);
			hFTEngine = nullptr;
		}
	}


	if (!hFTEngine) {
        int ret = AFT_FSDK_InitialFaceEngine((MPChar)APPID, (MPChar)FT_SDKKEY, mFTWorkMem, FT_WORKMEM_SIZE, &hFTEngine, AFT_FSDK_OPF_0_HIGHER_EXT, 16, MAX_FT_FACE);
		if (ret != 0) {
			qDebug() << "fail to AFT_FSDK_InitialFaceEngine():" << ret;
			return false;
		}
	}

	if (!hFREngine) {
        int ret = AFR_FSDK_InitialEngine((MPChar)APPID, (MPChar)FR_SDKKEY, mFRWorkMem, FR_WORKMEM_SIZE, &hFREngine);
		if (ret != 0) {
			qDebug() << "fail to AFR_FSDK_InitialEngine():" << ret;
			return false;
		}
	}

	int frameSize = 0;
	ASVLOFFSCREEN inputImg = { 0 };
	inputImg.u32PixelArrayFormat = frameFormat;
	inputImg.i32Width = frameWidth;
	inputImg.i32Height = frameHeight;
	inputImg.ppu8Plane[0] = frameData;
	if (ASVL_PAF_I420 == inputImg.u32PixelArrayFormat) {
		inputImg.pi32Pitch[0] = inputImg.i32Width;
		inputImg.pi32Pitch[1] = inputImg.i32Width / 2;
		inputImg.pi32Pitch[2] = inputImg.i32Width / 2;
		inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + inputImg.pi32Pitch[0] * inputImg.i32Height;
		inputImg.ppu8Plane[2] = inputImg.ppu8Plane[1] + inputImg.pi32Pitch[1] * inputImg.i32Height / 2;
		frameSize = inputImg.i32Width*inputImg.i32Height * 3 / 2;
	} else if (ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat) {
		inputImg.pi32Pitch[0] = inputImg.i32Width;
		inputImg.pi32Pitch[1] = inputImg.i32Width;
		inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
		frameSize = inputImg.i32Width*inputImg.i32Height * 3 / 2;
	} else if (ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat) {
		inputImg.pi32Pitch[0] = inputImg.i32Width;
		inputImg.pi32Pitch[1] = inputImg.i32Width;
		inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
		frameSize = inputImg.i32Width*inputImg.i32Height * 3 / 2;
	} else if (ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat) {
		inputImg.pi32Pitch[0] = inputImg.i32Width * 2;
		frameSize = inputImg.i32Width*inputImg.i32Height * 2;
    } else if (ASVL_PAF_RGB24_B8G8R8 == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width * 3;
        frameSize = inputImg.i32Width*inputImg.i32Height * 3;
	} else if (ASVL_PAF_RGB32_B8G8R8A8 == inputImg.u32PixelArrayFormat) {
		inputImg.pi32Pitch[0] = inputImg.i32Width * 4;
		frameSize = inputImg.i32Width*inputImg.i32Height * 4;
	} else {
		qDebug() << "unsupported Image format:" << inputImg.u32PixelArrayFormat;
	}
	LPAFT_FSDK_FACERES faceResult = nullptr;
	int ret = AFT_FSDK_FaceFeatureDetect(hFTEngine, &inputImg, &faceResult);
	if (ret != 0) {
		mFaceNum = 0;
		qDebug() << "fail to AFT_FSDK_FaceFeatureDetect():" << ret;
	} else {
		int oldFaceNum = mFaceNum;
		MRECT oldFaceRect[MAX_FT_FACE];
        int oldFaceIndex[MAX_FT_FACE];
		memcpy(oldFaceRect, mFaceRect, sizeof(MRECT)*oldFaceNum);
		for (int k = 0; k < oldFaceNum; k++) {
			oldFaceIndex[k] = mFaceIndex[k];
		}

		mFaceNum = faceResult->nFace;
		for (int i = 0; i < mFaceNum; i++) {
#if 0
			qDebug() << "face " << i << ":(" <<
				faceResult->rcFace[i].left << "," << faceResult->rcFace[i].top << "," <<
				faceResult->rcFace[i].right << "," << faceResult->rcFace[i].bottom << ")";
#endif
			mFaceOrient[i] = faceResult->lfaceOrient;
			mFaceRect[i].left = faceResult->rcFace[i].left;
			mFaceRect[i].top = faceResult->rcFace[i].top;
			mFaceRect[i].right = faceResult->rcFace[i].right;
			mFaceRect[i].bottom = faceResult->rcFace[i].bottom;
			mFaceIndex[i] = -1;
		}

		for (int i = 0; i < mFaceNum; i++) {
			int maxOverlapArea = 0;
			int maxOverlapIndex = 0;
			int halfArea = ((mFaceRect[i].bottom - mFaceRect[i].top)*(mFaceRect[i].right - mFaceRect[i].left)) / 4;

			for (int k = 0; k < oldFaceNum; k++) {
				int area = computeOverlapArea(mFaceRect[i].left, mFaceRect[i].bottom,
					mFaceRect[i].right, mFaceRect[i].top,
					oldFaceRect[k].left, oldFaceRect[k].bottom,
					oldFaceRect[k].right, oldFaceRect[k].top);
				if (area > maxOverlapArea) {
					maxOverlapArea = area;
					maxOverlapIndex = k;
				}
			}

			if (maxOverlapArea <= halfArea) {
				mFaceIndex[i] = -1;
				bFaceChange = true;
			} else {
				mFaceIndex[i] = oldFaceIndex[maxOverlapIndex];
			}
		}

		if (mSemFRDone.available() > 0) {
			if (bFaceChange) {
				bFaceChange = false;
			} else {
				for (int i = 0; i < mThreadFaceNum; i++) {
					mFaceIndex[i] = mThreadFaceIndex[i];
					mFaceName[i] = mThreadFaceName[i];
				}
			}

			while (mSemFRDone.tryAcquire());
			for (int i = 0; i < mFaceNum; i++) {
				mThreadFaceOrient[i] = mFaceOrient[i];
				mThreadFaceRect[i].left = mFaceRect[i].left;
				mThreadFaceRect[i].top = mFaceRect[i].top;
				mThreadFaceRect[i].right = mFaceRect[i].right;
				mThreadFaceRect[i].bottom = mFaceRect[i].bottom;
				mThreadFaceIndex[i] = mFaceIndex[i];
				mThreadFaceName[i] = mFaceName[i];
			}

			mThreadFaceNum = mFaceNum;
			if ((mThreadFrameWidth != frameWidth) || (mThreadFrameHeight != frameHeight) || (mThreadFrameFormat != frameFormat)) {
				mThreadFrameWidth = frameWidth;
				mThreadFrameHeight = frameHeight;
				mThreadFrameFormat = frameFormat;
				delete[] mThreadFrame;
				mThreadFrame = nullptr;
			}

			if (!mThreadFrame) {
				mThreadFrame = new unsigned char[frameSize];
			}
			memcpy(mThreadFrame, frameData, frameSize);
			mSemToDoFR.release();
		}

		for (int i = 0; i < mFaceNum; i++) {
			if (mFaceIndex[i] >= 0) {
				mFaceName[i] = mRegisterFaces[mFaceIndex[i]].name;
			} else {
				mFaceName[i] = "";
			}
		}

        int oldFaceValue[MAX_FT_FACE];
        for (int i = 0; i < oldFaceNum; i++) {
            oldFaceValue[i] = mFaceValue[i];
        }

        for (int i = 0; i < mFaceNum; i++) {
            mFaceValue[i] = 0;
        }

        for (int i = 0; i < mFaceNum; i++) {
           for (int j = 0; j < oldFaceNum; j++) {
               if(oldFaceIndex[j] == mFaceIndex[i]){
                   mFaceValue[i] = oldFaceValue[j]+1;
                   break;
               }
           }
        }
	}
	return true;
}

void ArcFaceEngine::addFace(int id, QString name, QByteArray feature, QByteArray thumbnail){
    Face face;
    face.name = name;
    face.id = id;
    face.size = feature.size();
    face.feature = new MByte[face.size];
    face.thumbnail = thumbnail;
    memcpy(face.feature, feature.data(), face.size);
    mRegisterFaces.push_back(face);
}


int ArcFaceEngine::computeOverlapArea(int leftA, int bottomA, int rightA, int topA, int leftB, int bottomB, int rightB, int topB) {
	if ((leftB >= rightA) || (topA >= bottomB) || (topB >= bottomA) || (leftA >= rightB)) return 0;
	return ((qMin(rightB, rightA) - qMax(leftA, leftB)) * (qMin(bottomA, bottomB) - qMax(topA, topB)));
}

void ArcFaceEngine::enable(){
	bEnable = true;
}
