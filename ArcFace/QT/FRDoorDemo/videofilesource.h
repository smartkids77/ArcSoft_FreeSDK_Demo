#ifndef VIDEOFILESOURCE_H
#define VIDEOFILESOURCE_H

#include <QObject>
#include <QMediaPlayer>
#include <QAbstractVideoSurface>
#include "framesource.h"

class VideoFileSource :  public QAbstractVideoSurface,public FrameSource
{
    Q_OBJECT
public:
    explicit VideoFileSource();
    virtual ~VideoFileSource();

    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType)const;
    virtual bool isFormatSupported(const QVideoSurfaceFormat &format)const;
    virtual bool start(const QVideoSurfaceFormat &format);
    virtual void stop();
    virtual bool present(const QVideoFrame &frame);

    virtual void ReadFrame(unsigned char *pBuffer,int bufsize);
    virtual void Open(unsigned int id,QString devicePath);
    virtual void Close(void);
    virtual bool Ready(void);
    virtual int Width(void);
    virtual int Height(void);
    virtual int Format(void);
signals:

public slots:

private:
    int mWidth;
    int mHeight;
    int mFormat;
    int mDataSize;
    bool bOpen;
    unsigned char *mBuffer;
    QMediaPlayer mMediaPlayer;
};

#endif // VIDEOFILESOURCE_H
