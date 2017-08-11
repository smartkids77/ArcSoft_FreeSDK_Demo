#include "camerasource.h"
#include "asvloffscreen.h"

#include <QDebug>
#include <QThread>

#ifdef WIN32
#include <evr.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <wmcodecdsp.h>
#include <wmsdkidl.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Ole32.lib")

template <class T> void COMSafeReleaseAndSetNULL(T **ppT)
{
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

#define CHECK_HR(cond)  \
        do{\
            HRESULT local_hr = cond; \
            if ((!(SUCCEEDED(local_hr)))){\
                qDebug()<<"assert here "<<__FILE__<<"-"<<__LINE__<<"-"<<__FUNCTION__; \
                return; \
			            }\
		        } while (0)

#else
#include <linux/videodev2.h>
#include <linux/usbdevice_fs.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

CameraSource::CameraSource(int width, int height, int format){
	mWidth = width;
	mHeight = height;
	mFormat = format;
	bOpen = false;
}

CameraSource::~CameraSource(){
    Close();
}

void CameraSource::ReadFrame(unsigned char *pBuffer,int bufsize){
	if (!bOpen) {
		return;
	}

#ifdef WIN32
	while (true) {
		DWORD streamIndex, flags;
		LONGLONG llVideoTimeStamp, llSampleDuration;
		IMFSample *videoSample = nullptr;
		CHECK_HR(((IMFSourceReader *)mSourceReader)->ReadSample(
			MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			&streamIndex,
			&flags,
			&llVideoTimeStamp,
			&videoSample
			));

		if (flags & MF_SOURCE_READERF_STREAMTICK) {
            //qDebug() << ("Stream tick.");
			QThread::msleep(10);
			continue;
		}

		if (nullptr != videoSample) {

			CHECK_HR(videoSample->SetSampleTime(llVideoTimeStamp));
			CHECK_HR(videoSample->GetSampleDuration(&llSampleDuration));

			IMFMediaBuffer *buf = nullptr;
			DWORD bufLength;
			CHECK_HR(videoSample->ConvertToContiguousBuffer(&buf));
			CHECK_HR(buf->GetCurrentLength(&bufLength));

			byte *byteBuffer;
			DWORD buffCurrLen = 0;
			DWORD buffMaxLen = 0;

			buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
            memcpy(pBuffer, byteBuffer, buffCurrLen);
			buf->Unlock();

			COMSafeReleaseAndSetNULL(&buf);
			COMSafeReleaseAndSetNULL(&videoSample);
			break;
		} else {
			QThread::msleep(10);
			continue;
		}
	}
#else
	struct v4l2_buffer v4l2buf;
	memset(&v4l2buf, 0, sizeof(v4l2buf));
	v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2buf.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(mV4l2_fd, VIDIOC_DQBUF, &v4l2buf);
	if (0 != ret) {
		qDebug() << "Command VIDIOC_DQBUF error:" << errno << " " << strerror(errno);
		return;
    }

    if (v4l2buf.bytesused != bufsize) {
        qDebug() << "v4l2buf.bytesused error: " << v4l2buf.bytesused;
        return;
    }

    memcpy(pBuffer, mV4l2_bufstart[v4l2buf.index], v4l2buf.bytesused);


	int index = v4l2buf.index;
	memset(&v4l2buf, 0, sizeof(v4l2buf));
	v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2buf.memory = V4L2_MEMORY_MMAP;
	v4l2buf.index = index;
    ret = ioctl(mV4l2_fd, VIDIOC_QBUF, &v4l2buf);
	if (0 != ret) {
		qDebug() << "Command VIDIOC_QBUF error:" << errno << " " << strerror(errno);
		return;
	}

#endif
}

void CameraSource::Open(unsigned int id,QString devicePath)
{
	if (bOpen) {
		return;
	}
#ifdef WIN32
    mVideoDeviceCount = 0;
	mSourceReader = nullptr;
    mVideoDevices = nullptr;

	IMFMediaSource *videoSource = nullptr;
    UINT32 videoDeviceCount = 0;
	IMFAttributes *videoConfig = nullptr;
	IMFActivate **videoDevices = nullptr;

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	MFStartup(MF_VERSION);

	// Get the first available webcam.
	CHECK_HR(MFCreateAttributes(&videoConfig, 1));

	// Request video capture devices.
	CHECK_HR(videoConfig->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID));

	CHECK_HR(MFEnumDeviceSources(videoConfig, &videoDevices, &videoDeviceCount));
	if (videoDeviceCount <= 0) {
		qDebug() << "No Camera";
		return;
	}

    if (id >= videoDeviceCount) {
        qDebug() << "mCameraID "<<id <<" "<<"is invalid";
        return;
    }

    CHECK_HR(videoDevices[id]->ActivateObject(IID_PPV_ARGS(&videoSource)));

	// Create a source reader.
	CHECK_HR(MFCreateSourceReaderFromMediaSource(
		videoSource,
		videoConfig,
		(IMFSourceReader **)&mSourceReader));
	COMSafeReleaseAndSetNULL(&videoSource);
	COMSafeReleaseAndSetNULL(&videoConfig);

	IMFMediaType *pSrcOutMediaType = nullptr;
	CHECK_HR(MFCreateMediaType(&pSrcOutMediaType));
	CHECK_HR(pSrcOutMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
	if (mFormat == ASVL_PAF_YUYV) {
		CHECK_HR(pSrcOutMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YUY2));
	} else if (mFormat == ASVL_PAF_I420) {
		CHECK_HR(pSrcOutMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_I420));
	} else {
        qDebug() << "Camera Format supported";
		return;
	}

	CHECK_HR(MFSetAttributeSize(pSrcOutMediaType, MF_MT_FRAME_SIZE, mWidth, mHeight));

	CHECK_HR(((IMFSourceReader *)mSourceReader)->SetCurrentMediaType(0, nullptr, pSrcOutMediaType));
	COMSafeReleaseAndSetNULL(&pSrcOutMediaType);
    mVideoDeviceCount = videoDeviceCount;
    mVideoDevices = (void**)videoDevices;

#else

    char path[128];
    snprintf(path, sizeof(path), "/dev/video%d", id);

	struct stat st;
    if (-1 == stat(path, &st)) {
        qDebug() << "cannot identify " << path;
		return;
	}

	if (!S_ISCHR(st.st_mode)) {
        qDebug() << path << " is no device";
		return;
	}


    mV4l2_fd = open((const char *)path, O_RDWR, 0);
    if (-1 == mV4l2_fd) {
        qDebug() << "cannot open " << path << ":" << errno << " " << strerror(errno);
		return;
	}

	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = mWidth;
	fmt.fmt.pix.height = mHeight;
	if (mFormat == ASVL_PAF_NV21) {
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21;
	} else if (mFormat == ASVL_PAF_YUYV) {
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	} else {
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	}
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    int ret = ioctl(mV4l2_fd, VIDIOC_S_FMT, &fmt);
	if (0 != ret) {
		qDebug() << "Command VIDIOC_S_FMT error:" << errno << " " << strerror(errno);
		return;
	}

	struct v4l2_streamparm params;
	memset(&params, 0, sizeof(params));
	params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	params.parm.capture.timeperframe.numerator = 1;
	params.parm.capture.timeperframe.denominator = 30;
    ret = ioctl(mV4l2_fd, VIDIOC_S_PARM, &params);
	if (ret != 0) {
		qDebug() << "Command VIDIOC_S_PARM error:" << errno << " " << strerror(errno);
		return;
	}

	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(mV4l2_fd, VIDIOC_REQBUFS, &req);
	if (ret != 0) {
		qDebug() << "Command VIDIOC_REQBUFS error:" << errno << " " << strerror(errno);
		return;
	}

	if (req.count != 4) {
		qDebug() << "req.count == " << req.count;
		return;
	}
    mV4l2_bufcount = req.count;

	struct v4l2_buffer v4l2buf;
    for (int i = 0; i < mV4l2_bufcount; ++i) {
		memset(&v4l2buf, 0, sizeof(v4l2buf));
		v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2buf.memory = V4L2_MEMORY_MMAP;
		v4l2buf.index = i;
        ret = ioctl(mV4l2_fd, VIDIOC_QUERYBUF, &v4l2buf);
		if (ret != 0) {
			qDebug() << "VIDIOC_QUERYBUF";
			return;
		}

        mV4l2_buflength[i] = v4l2buf.length;
        mV4l2_bufstart[i] = mmap(NULL, v4l2buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, mV4l2_fd, v4l2buf.m.offset);
        if (MAP_FAILED == mV4l2_bufstart[i]) {
			qDebug() << "MAP_FAILED";
			return;
		}
	}

    for (int i = 0; i < mV4l2_bufcount; ++i) {
		memset(&v4l2buf, 0, sizeof(v4l2buf));
		v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2buf.memory = V4L2_MEMORY_MMAP;
		v4l2buf.index = i;
        if (0 != ioctl(mV4l2_fd, VIDIOC_QBUF, &v4l2buf)) {
			qDebug() << "VIDIOC_QBUF";
			return;
		}
	}

	enum v4l2_buf_type v4l2type;
	memset(&v4l2type, 0, sizeof(v4l2type));
	v4l2type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 != ioctl(mV4l2_fd, VIDIOC_STREAMON, &v4l2type)) {
		qDebug() << "VIDIOC_STREAMON";
		return;
	}

#endif
    bOpen = true;
}

void CameraSource::Close(void){
	if (!bOpen) {
		return;
	}
    bOpen = false;

#ifdef WIN32
    IMFSourceReader *sourceReader = (IMFSourceReader *)mSourceReader;
    COMSafeReleaseAndSetNULL(&sourceReader);
    for (unsigned int i = 0; i < mVideoDeviceCount; i++){
        IMFActivate *videoDevices = (IMFActivate *)mVideoDevices[i];
        videoDevices->ShutdownObject();
        COMSafeReleaseAndSetNULL(&videoDevices);
    }
    CoUninitialize();
#else
    enum v4l2_buf_type v4l2type;
    v4l2type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 != ioctl (mV4l2_fd, VIDIOC_STREAMOFF, &v4l2type)) {
        qDebug() << "Command VIDIOC_STREAMOFF error:" << errno << " " << strerror(errno);
        return;
    }

    for (int i = 0; i < mV4l2_bufcount ; ++i) {
        if (0 != munmap (&mV4l2_bufstart[i], mV4l2_buflength[i])) {
            qDebug() << "Command munmap error:" << errno << " " << strerror(errno);
            return;
        }
    }

    if(mV4l2_fd != 0){
        close (mV4l2_fd);
        mV4l2_fd = 0;
    }
#endif

}

bool CameraSource::Ready(void){
	return bOpen;
}

int CameraSource::Width(void){
    return mWidth;
}

int CameraSource::Height(void){
    return mHeight;
}

int CameraSource::Format(void){
    return mFormat;
}

