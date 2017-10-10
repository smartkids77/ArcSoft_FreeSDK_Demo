#include <QDebug>
#include <QElapsedTimer>

#include "arcfaceengine.h"

#include "arcsoft_fsdk_face_recognition.h"
#include "arcsoft_fsdk_face_tracking.h"
#include "key.h"


ArcFaceEngine::ArcFaceEngine()
{

     doFRFrameCount = 0;
     bReady = false;
     mThreshold = 0.4f;

     mFaceNum = 0;
     for(int k=0;k<MAX_FT_FACE;k++){
         mFaceID[k] = -1;
     }

     mUniqueIncID = 0;

     int ft_workmem_size = 20 * 1024 * 1024;
     mFTWorkMem = new unsigned char[ft_workmem_size];
     int fr_workmem_size = 40 * 1024 * 1024;
     mFRWorkMem = new unsigned char[fr_workmem_size];

     hFTEngine = nullptr;
     hFREngine = nullptr;


     int ret = AFT_FSDK_InitialFaceEngine((MPChar)APPID, (MPChar)FT_SDKKEY, mFTWorkMem, ft_workmem_size, &hFTEngine, AFT_FSDK_OPF_0_HIGHER_EXT, 16, MAX_FT_FACE);
     if(ret != 0){
         qDebug() <<"fail to AFT_FSDK_InitialFaceEngine():"<<ret;
         return;
     }
#if 0
    if (hFTEngine) {
        const AFT_FSDK_Version*pVersionInfo = AFT_FSDK_GetVersion(hFTEngine);
        qDebug() << pVersionInfo->lCodebase << " " << pVersionInfo->lMajor << " " << pVersionInfo->lMinor << " " << pVersionInfo->lBuild;
        qDebug() << pVersionInfo->Version;
        qDebug() << pVersionInfo->BuildDate;
        qDebug() << pVersionInfo->CopyRight;
    }
#endif

     ret = AFR_FSDK_InitialEngine((MPChar)APPID, (MPChar)FR_SDKKEY, mFRWorkMem, fr_workmem_size, &hFREngine);
     if(ret != 0){
         qDebug() <<"fail to AFR_FSDK_InitialEngine():"<<ret;
         return;
     }

 #if 0
     if (hFREngine) {
         const AFR_FSDK_Version*pVersionInfo = AFR_FSDK_GetVersion(hFREngine);
         qDebug() << pVersionInfo->lCodebase << " " << pVersionInfo->lMajor << " " << pVersionInfo->lMinor << " " << pVersionInfo->lBuild;
         qDebug() << pVersionInfo->Version;
         qDebug() << pVersionInfo->BuildDate;
         qDebug() << pVersionInfo->CopyRight;
     }
 #endif

     bReady = true;
}

ArcFaceEngine::~ArcFaceEngine(){

    if (hFTEngine) {
        AFT_FSDK_UninitialFaceEngine(hFTEngine);
    }

    if (hFREngine) {
        AFR_FSDK_UninitialEngine(hFREngine);
    }

    if(mFTWorkMem){
        delete[] mFTWorkMem;
        mFTWorkMem = nullptr;
    }

    if(mFRWorkMem){
        delete[] mFRWorkMem;
        mFRWorkMem = nullptr;
    }

    int facenum = mRegisterFaces.count();
    for (int i = 0; i <facenum; i++) {
         FaceInfo faceinfo = mRegisterFaces[i];
         delete faceinfo.pFeature;
    }
}

void ArcFaceEngine::removeFace(int id){
    int facenum = mRegisterFaces.count();
    for (int i = 0; i <facenum; i++) {
        FaceInfo faceinfo = mRegisterFaces[i];
        if(faceinfo.id == id){
            delete[] faceinfo.pFeature;
            mRegisterFaces.remove(i);
            break;
        }
    }
}


bool ArcFaceEngine::processFrame(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat){
   if(!bReady){
       return false;
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
       frameSize = inputImg.i32Width*inputImg.i32Height*3/2;
   } else if (ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat) {
       inputImg.pi32Pitch[0] = inputImg.i32Width;
       inputImg.pi32Pitch[1] = inputImg.i32Width;
       inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
       frameSize = inputImg.i32Width*inputImg.i32Height*3/2;
   } else if (ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat) {
       inputImg.pi32Pitch[0] = inputImg.i32Width;
       inputImg.pi32Pitch[1] = inputImg.i32Width;
       inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
       frameSize = inputImg.i32Width*inputImg.i32Height*3/2;
   } else if (ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat) {
       inputImg.pi32Pitch[0] = inputImg.i32Width * 2;
       frameSize = inputImg.i32Width*inputImg.i32Height*2;
   } else if (ASVL_PAF_RGB32_B8G8R8A8 == inputImg.u32PixelArrayFormat) {
       inputImg.pi32Pitch[0] = inputImg.i32Width * 4;
       frameSize = inputImg.i32Width*inputImg.i32Height*4;
   } else {
       qDebug() << "unsupported Image format:" << inputImg.u32PixelArrayFormat;
   }
   LPAFT_FSDK_FACERES faceResult = nullptr;
   int ret = AFT_FSDK_FaceFeatureDetect(hFTEngine, &inputImg, &faceResult);

   if (ret != 0) {
       mFaceNum = 0;
       //qDebug() << "fail to AFT_FSDK_FaceFeatureDetect():" << ret;
   } else {
       int oldFaceNum = mFaceNum;
       MRECT oldFaceRect[MAX_FT_FACE];
       int oldFaceID[MAX_FT_FACE];
       memcpy(oldFaceRect,mFaceRect,sizeof(MRECT)*oldFaceNum);
       for(int k=0;k<oldFaceNum;k++){
           oldFaceID[k] = mFaceID[k];
       }

       mFaceNum = faceResult->nFace;


       for (int i = 0; i < mFaceNum; i++) {
       #if 0
           qDebug()<<"face "<<i<<":("<<
                faceResult->rcFace[i].left<<","<<faceResult->rcFace[i].top<<","<<
                faceResult->rcFace[i].right<<","<<faceResult->rcFace[i].bottom<<")";
       #endif
           mFaceOrient[i] = faceResult->lfaceOrient;
           mFaceRect[i].left = faceResult->rcFace[i].left;
           mFaceRect[i].top = faceResult->rcFace[i].top;
           mFaceRect[i].right = faceResult->rcFace[i].right;
           mFaceRect[i].bottom = faceResult->rcFace[i].bottom;
       }

       if((doFRFrameCount>=10)&&(mRegisterFaces.count()>0)){
           doFRFrameCount = 0;
           for (int i = 0; i < mFaceNum; i++) {
            #if 1
               QElapsedTimer timer;
               timer.start();
            #endif
               AFR_FSDK_FACEMODEL localFaceModels = { 0 };
               int ret = ExtractFRFeature(frameData,frameWidth,frameHeight,frameFormat,&mFaceRect[i],mFaceOrient[i],&localFaceModels);
            #if 1
               //qDebug() << "AFR_FSDK_ExtractFRFeature took" << timer.elapsed() << "milliseconds";
           #endif
               if(ret == 0){
                   float fMaxScore = 0.0f;
                   int iMaxIndex = -1;
                   int facenum = mRegisterFaces.count();
                   for (int j = 0; j <facenum; j++) {
                       FaceInfo faceinfo = mRegisterFaces[j];
                       float fScore = 0.0f;
                       AFR_FSDK_FACEMODEL targetFaceModels = { 0 };
                       targetFaceModels.lFeatureSize = faceinfo.featureSize;
                       targetFaceModels.pbFeature = faceinfo.pFeature;
                       ret = AFR_FSDK_FacePairMatching(hFREngine, &targetFaceModels, &localFaceModels, &fScore);
                       if(ret == 0){
                           if(fScore>fMaxScore){
                               iMaxIndex = j;
                               fMaxScore = fScore;
                           }
                       }
                   }

                   if((iMaxIndex>=0)&&(fMaxScore>mThreshold)){
                       mFaceID[i] = mRegisterFaces[iMaxIndex].id;
                       mFaceName[i] = mRegisterFaces[iMaxIndex].name;
                   }else{
                       mFaceID[i] = -1;
                       mFaceName[i] = "";
                   }
               }else{
                    //qDebug() << "AFR_FSDK_ExtractFRFeature failed";
               }
           }
       }
    }
    doFRFrameCount++;
    return true;
}


int ArcFaceEngine::ExtractFRFeature(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat,MRECT *pRect,int faceOrient,AFR_FSDK_FACEMODEL *pFaceModels){

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
       qDebug() << "unsupported Image format:"<<inputImg.u32PixelArrayFormat;
    }

    AFR_FSDK_FACEINPUT faceInput;
    faceInput.lOrient = faceOrient;
    faceInput.rcFace.left = pRect[0].left;
    faceInput.rcFace.top = pRect[0].top;
    faceInput.rcFace.right = pRect[0].right;
    faceInput.rcFace.bottom = pRect[0].bottom;

    return AFR_FSDK_ExtractFRFeature(hFREngine, &inputImg, &faceInput, pFaceModels);
}

int ArcFaceEngine::registerFace(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat,MRECT *pRect,int faceOrient,int *pID){
    AFR_FSDK_FACEMODEL localFaceModels = { 0 };
    int ret = ExtractFRFeature(frameData,frameWidth,frameHeight,frameFormat,pRect,faceOrient,&localFaceModels);
    if(ret == 0){
        FaceInfo faceinfo;
        faceinfo.name = "";
        faceinfo.id = mUniqueIncID;
        faceinfo.featureSize = localFaceModels.lFeatureSize;
        faceinfo.pFeature = new MByte[faceinfo.featureSize];
        memcpy(faceinfo.pFeature, localFaceModels.pbFeature, faceinfo.featureSize);
        mRegisterFaces.push_back(faceinfo);

        *pID = faceinfo.id;
        mUniqueIncID++;
    }
    return 0;
}

int ArcFaceEngine::recognitionFace(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat,MRECT *pRect,int faceOrient,int *pID,float *pfScore){
    int facenum = mRegisterFaces.count();
    *pID = -1;
    if(facenum<=0){
        return -1;
    }

    AFR_FSDK_FACEMODEL localFaceModels = { 0 };
    int ret = ExtractFRFeature(frameData,frameWidth,frameHeight,frameFormat,pRect,faceOrient,&localFaceModels);
    *pfScore = 0.0f;
    if(ret == 0){
         for (int i = 0; i <facenum; i++) {
             FaceInfo faceinfo = mRegisterFaces[i];
             MFloat fScore = 0.0f;
             AFR_FSDK_FACEMODEL targetFaceModels = { 0 };
             targetFaceModels.lFeatureSize = faceinfo.featureSize;
             targetFaceModels.pbFeature = faceinfo.pFeature;
             ret = AFR_FSDK_FacePairMatching(hFREngine, &targetFaceModels, &localFaceModels, &fScore);
             if(ret == 0){
                 if(fScore>(*pfScore)){
                     *pID = faceinfo.id;
                     (*pfScore) = fScore;
                 }
             }
         }
    }
    if(((*pID)>=0)&&((*pfScore)>mThreshold)){

    }else{
        *pID = -1;
    }
    return 0;
}

void ArcFaceEngine::updateFaceName(int id,QString name){
    int facenum = mRegisterFaces.count();
    for (int i = 0; i <facenum; i++) {
       FaceInfo faceinfo = mRegisterFaces[i];
       if(faceinfo.id == id){
            faceinfo.name = name;
            mRegisterFaces[i] = faceinfo;
            return;
       }
    }
}


QString ArcFaceEngine::getFaceNameByID(int id){
    int facenum = mRegisterFaces.count();
    for (int i = 0; i <facenum; i++) {
       FaceInfo faceinfo = mRegisterFaces[i];
       if(faceinfo.id == id){
            return faceinfo.name;
       }
    }
    return "";
}
