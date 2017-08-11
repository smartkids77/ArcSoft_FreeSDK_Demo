#include "videofilesource.h"
#include "asvloffscreen.h"

#include <QMediaPlayer>
#include <QAbstractVideoSurface>
#include <QAbstractVideoBuffer>
#include <QVideoFrame>
#include <QThread>

VideoFileSource::VideoFileSource()
{
    mWidth = -1;
    mHeight =  -1;
    mFormat =  -1;
    mDataSize = 0;
    mBuffer = nullptr;
    bOpen = false;
}

VideoFileSource::~VideoFileSource(){
    Close();
}

QList<QVideoFrame::PixelFormat> VideoFileSource::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_YUYV
                << QVideoFrame::Format_NV12
                << QVideoFrame::Format_NV21
                << QVideoFrame::Format_YUV420P
                << QVideoFrame::Format_ARGB32;
    }else if (handleType == QAbstractVideoBuffer::GLTextureHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_YUYV
                << QVideoFrame::Format_NV12
                << QVideoFrame::Format_NV21
                << QVideoFrame::Format_YUV420P
                << QVideoFrame::Format_ARGB32;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool VideoFileSource::isFormatSupported(const QVideoSurfaceFormat &format) const{
    return true;
}

bool VideoFileSource::start(const QVideoSurfaceFormat &format){
    QAbstractVideoSurface::start(format);
    return true;
}

void VideoFileSource::stop(){
    QAbstractVideoSurface::stop();
}

bool VideoFileSource::present(const QVideoFrame &frame){
    if(frame.isValid()){
        QVideoFrame cloneFrame(frame);

        QVideoFrame::PixelFormat format = cloneFrame.pixelFormat();
        if(format != QVideoFrame::Format_Invalid){
            if(mBuffer == nullptr){
                if(format == QVideoFrame::Format_ARGB32){

                    mWidth = cloneFrame.width();
                    mHeight = cloneFrame.height();
                    mFormat = ASVL_PAF_RGB32_B8G8R8A8;
                    mDataSize = mWidth*mHeight*4;
                    mBuffer = new unsigned char[mDataSize];

                }else if(format == QVideoFrame::Format_YUV420P){
                    mWidth = cloneFrame.width();
                    mHeight = cloneFrame.height();
                    mFormat = ASVL_PAF_I420;
                    mDataSize = mWidth*mHeight*3/2;
                    mBuffer = new unsigned char[mDataSize];
                }else{
                    qDebug()<<"unsupported QVideoFrame format";
                    qDebug()<<"format:"<<format;
                    qDebug()<<"width:"<<frame.width();
                    qDebug()<<"height:"<<frame.height();
                }
            }

            if(mFormat != -1){

            #if 1
                if(!bOpen){
                    qDebug()<<"format:"<<format;
                    qDebug()<<"width:"<<frame.width();
                    qDebug()<<"height:"<<frame.height();
                }
            #endif
                cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
                unsigned char *pBase = cloneFrame.bits();
                int pitch = frame.bytesPerLine();

                if(mFormat == ASVL_PAF_RGB32_B8G8R8A8){
                    for(int i=0;i<mHeight;i++){
                        memcpy(mBuffer+i*(mWidth*4),pBase+i*pitch,mWidth*4);
                    }
                }else{
                     //Todo:handle pitch when width != pitch
                     memcpy(mBuffer,pBase,mDataSize);
                }

                if(!bOpen){
                    bOpen = true;
                }
                cloneFrame.unmap();
            }
        }
    }

    return true;
}

void VideoFileSource::ReadFrame(unsigned char *pBuffer,int bufsize){
    if(mBuffer != nullptr){
        if(bufsize ==  mDataSize){
            memcpy(pBuffer,mBuffer,bufsize);
        }
    }
}

void VideoFileSource::Open(unsigned int id,QString devicePath){
    if (bOpen) {
        return;
    }

    mMediaPlayer.setVideoOutput(this);
    mMediaPlayer.setMedia(QUrl::fromLocalFile(devicePath));
    mMediaPlayer.play();
}

void VideoFileSource::Close(void){
    if (!bOpen) {
        return;
    }
    bOpen = false;

    if(mMediaPlayer.state() == QMediaPlayer::PlayingState){
        mMediaPlayer.stop();
    }

    if(mBuffer != nullptr){
        delete[] mBuffer;
        mBuffer = nullptr;
    }
}

bool VideoFileSource::Ready(void){
    return bOpen;
}

int VideoFileSource::Width(void){
    return mWidth;
}

int VideoFileSource::Height(void){
    return mHeight;
}

int VideoFileSource::Format(void){
    return mFormat;
}
