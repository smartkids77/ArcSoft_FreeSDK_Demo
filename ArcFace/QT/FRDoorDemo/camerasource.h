#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include "framesource.h"

class CameraSource :public QObject,public FrameSource
{
	Q_OBJECT
public:
    CameraSource(int width, int height, int format);
    virtual ~CameraSource();

    virtual void ReadFrame(unsigned char *pBuffer,int bufsize);
    virtual void Open(unsigned int id,QString devicePath);
    virtual void Close(void);
    virtual bool Ready(void);
    virtual int Width(void);
    virtual int Height(void);
    virtual int Format(void);
private:
	int mWidth;
	int mHeight;
	int mFormat;
    bool bOpen;
#ifdef WIN32
    unsigned int mVideoDeviceCount;
    void **mVideoDevices;
    void *mSourceReader;
#else
    int mV4l2_fd;
    void *mV4l2_bufstart[4];
    size_t mV4l2_buflength[4];
    int mV4l2_bufcount;
#endif

};

#endif // CAMERA_H
