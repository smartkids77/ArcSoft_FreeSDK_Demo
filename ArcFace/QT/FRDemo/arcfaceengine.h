#ifndef ARCFACEENGINE_H
#define ARCFACEENGINE_H

#include <QObject>
#include <QVector>

#include "asvloffscreen.h"
#include "arcsoft_fsdk_face_recognition.h"
#include "arcsoft_fsdk_face_tracking.h"

#define MAX_FT_FACE (50)

class FaceInfo{
public:
    unsigned char *pFeature;
    int featureSize;
    QString name;
    int id;
    FaceInfo(){
        pFeature = NULL;
        id = -1;
    }
 };


class ArcFaceEngine : public QObject
{
    Q_OBJECT
public:
    explicit ArcFaceEngine();
    virtual ~ArcFaceEngine();

    bool processFrame(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat);
    void removeFace(int id);
    void updateFaceName(int id,QString name);
    int registerFace(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat,MRECT *pRect,int faceOrient,int *pID);
    int recognitionFace(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat,MRECT *pRect,int faceOrient,int *pID,float *pfScore);

    MRECT mFaceRect[MAX_FT_FACE];
    int mFaceOrient[MAX_FT_FACE];
    QString mFaceName[MAX_FT_FACE];
    int mFaceID[MAX_FT_FACE];
    int mFaceNum;
    int mUniqueIncID;

public slots:

private:
    void *hFTEngine;
    unsigned char *mFTWorkMem;

    void *hFREngine;
    unsigned char *mFRWorkMem;

    QVector<FaceInfo> mRegisterFaces;

    float mThreshold;

    bool bReady;
    int doFRFrameCount;

    QString getFaceNameByID(int id);
    int ExtractFRFeature(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat,MRECT *pRect,int faceOrient,AFR_FSDK_FACEMODEL *pFaceModels);
};

#endif // ARCFACEENGINE_H
