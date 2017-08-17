#ifndef ARCFACEENGINE_H
#define ARCFACEENGINE_H

#include <QThread>
#include <QSemaphore>
#include <QVector>
#include <QImage>

#include "asvloffscreen.h"
#define MAX_FT_FACE (50)
#define FT_WORKMEM_SIZE (20 * 1024 * 1024)
#define FR_WORKMEM_SIZE (40 * 1024 * 1024)

class Face{
public:
    unsigned char *feature;
    size_t size;
    QString name;
    int id;
    QByteArray thumbnail;
    Face(){
        feature = nullptr;
        id = -1;
    }
 };


class ArcFaceEngine : public QThread
{
    Q_OBJECT
public:
    explicit ArcFaceEngine();
    virtual ~ArcFaceEngine();

    bool processFrame(unsigned char *frameData,int frameWidth,int frameHeight,int frameFormat);
    void addFace(int id,QString name,QByteArray feature,QByteArray thumbnail);
    void enable();


    MRECT mFaceRect[MAX_FT_FACE];
    int mFaceOrient[MAX_FT_FACE];
    QString mFaceName[MAX_FT_FACE];
    int mFaceIndex[MAX_FT_FACE];
    int mFaceNum;
    int mFaceValue[MAX_FT_FACE];
    QVector<Face> mRegisterFaces;

protected:
    virtual void run();

public slots:

private:
    void *hFTEngine;
    unsigned char *mFTWorkMem;

    void *hFREngine;
    unsigned char *mFRWorkMem;

    float mThreshold;

    bool bEnable;

    volatile bool bThreadRun;
    QSemaphore mSemToDoFR;
    QSemaphore mSemFRDone;

    bool bFaceChange;

    MRECT mThreadFaceRect[MAX_FT_FACE];
    int mThreadFaceOrient[MAX_FT_FACE];
    QString mThreadFaceName[MAX_FT_FACE];
    int mThreadFaceIndex[MAX_FT_FACE];
    int mThreadFaceNum;
    unsigned char *mThreadFrame;
    int mThreadFrameWidth;
    int mThreadFrameHeight;
    int mThreadFrameFormat;

    int mOldWidth;
    int mOldHeight;
    int mOldFormat;

    static int computeOverlapArea(int leftA, int bottomA, int rightA, int topA, int leftB, int bottomB, int rightB, int topB);
};

#endif // ARCFACEENGINE_H
