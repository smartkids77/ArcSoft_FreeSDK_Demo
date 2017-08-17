#ifndef FRAMESOURCEINTERFACE_H
#define FRAMESOURCEINTERFACE_H

class FrameSource
{

public:
    virtual void ReadFrame(unsigned char *pBuffer,int bufsize)=0;

    virtual void Open(unsigned int id,QString devicePath)=0;
    virtual void Close(void)=0;
    virtual bool Ready(void)=0;
    virtual int Width(void)=0;
    virtual int Height(void)=0;
    virtual int Format(void)=0;
};
#endif // FRAMESOURCEINTERFACE_H
