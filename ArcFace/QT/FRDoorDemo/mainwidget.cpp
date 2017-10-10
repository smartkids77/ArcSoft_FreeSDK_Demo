#include "mainwidget.h"
#include "faceviewwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QList>
#include <QMimeData>
#include <QImage>
#include <QBuffer>
#include <QImageReader>

#include "camerasource.h"
#include "videofilesource.h"
#include "glhelper.h"
#include "arcfaceengine.h"
#include "asvloffscreen.h"


MainWidget::MainWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{

    bFrameMirrored = false;
    mCameraSource = nullptr;
    mVideoFileSource = nullptr;

    mFrameData = nullptr;

    mGLHelper = nullptr;

    mArcFaceEngine = new ArcFaceEngine();
    mArcFaceEngine->enable();

    setFixedSize(640, 360);
    setAcceptDrops(true);
}

MainWidget::~MainWidget()
{
    if(mArcFaceEngine){
        delete mArcFaceEngine;
        mArcFaceEngine = nullptr;
    }

    if(mFrameData){
        delete[] mFrameData;
        mFrameData = nullptr;
    }

    if(mCameraSource != nullptr){
        delete mCameraSource;
        mCameraSource = nullptr;
    }

    if(mVideoFileSource != nullptr){
        delete mVideoFileSource;
        mVideoFileSource = nullptr;
    }

    if (mGLHelper != nullptr) {
        mGLHelper->uninit();
        delete mGLHelper;
        mGLHelper = nullptr;
	}
}

void MainWidget::dragEnterEvent(QDragEnterEvent *event){
    event->acceptProposedAction();
}

void MainWidget::dropEvent(QDropEvent *event){

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    QString fileName = urls.first().toLocalFile();
    openFile(fileName);
}

void MainWidget::openCamera(){
    if(mFrameData){
        delete[] mFrameData;
        mFrameData = nullptr;
    }

    if(mVideoFileSource != nullptr){
        delete mVideoFileSource;
        mVideoFileSource = nullptr;
    }

    if(mCameraSource != nullptr){
        delete mCameraSource;
        mCameraSource = nullptr;
    }

    bFrameMirrored = true;
#ifdef WIN32
    mCameraSource = new CameraSource(800, 600, ASVL_PAF_I420);
#else
    mCameraSource = new CameraSource(800, 600, ASVL_PAF_YUYV);
#endif
    mCameraSource->Open(0,"");
}

void MainWidget::openFile(QString filename){
    if (!filename.isEmpty()) {
        if(mFrameData){
            delete[] mFrameData;
            mFrameData = nullptr;
        }

        if(mCameraSource != nullptr){
            delete mCameraSource;
            mCameraSource = nullptr;
        }

        if(mVideoFileSource != nullptr){
            delete mVideoFileSource;
            mVideoFileSource = nullptr;
        }

        bFrameMirrored = false;
        mVideoFileSource = new VideoFileSource();
        mVideoFileSource->Open(0,filename);
    }
}

void MainWidget::openVideoFile(){


#ifdef WIN32
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video File"),QDir::currentPath());
#else
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video File"),
                                                    QDir::currentPath(),"",nullptr,
                                                    QFileDialog::DontUseNativeDialog);
#endif
    openFile(fileName);
}

void MainWidget::loadFaceDB(){
    if(mArcFaceEngine){
        delete mArcFaceEngine;
        mArcFaceEngine = nullptr;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DB File"),QDir::currentPath(), tr("Image Files (*.db)"));

    if (!fileName.isEmpty()) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(fileName);
        db.open();
        QSqlQuery query;
        query.exec("SELECT id,name,feature,thumbnail FROM faceinfo");
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            QByteArray feature = query.value(2).toByteArray();
            QByteArray thumbnail = query.value(3).toByteArray();
            if(!mArcFaceEngine){
                mArcFaceEngine = new ArcFaceEngine();
            }
            mArcFaceEngine->addFace(id,name,feature,thumbnail);
        }
        db.close();
        if(mArcFaceEngine){
            mArcFaceEngine->enable();
        }
    }
}


QSize MainWidget::minimumSizeHint() const
{
	return QSize(mWidgetWidth, mWidgetHeight);
}

QSize MainWidget::sizeHint() const
{
	return QSize(mWidgetWidth, mWidgetHeight);
}

void MainWidget::initializeGL()
{
    if (mGLHelper == NULL) {
        mGLHelper = new GLHelper();
	}

	initializeOpenGLFunctions();
	glClearDepthf(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glLineWidth(1.0);
	glEnable(GL_MULTISAMPLE);
}

void MainWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QPainter painter(this);
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", 10));
    painter.beginNativePainting();
    bool bNativePainting = true;

    FrameSource *frameSource = nullptr;

    if (mCameraSource != nullptr) {
        frameSource = mCameraSource;
    }else if(mVideoFileSource != nullptr){
        frameSource = mVideoFileSource;
    }

    if((frameSource != nullptr)&&(frameSource->Ready())){
        if(mFrameData == nullptr){
            mFrameFormat = frameSource->Format();
            mFrameWidth = frameSource->Width();
            mFrameHeight = frameSource->Height();

            if ((mFrameFormat == ASVL_PAF_NV21) || (mFrameFormat == ASVL_PAF_NV12) || (mFrameFormat == ASVL_PAF_I420)) {
                mFrameDataSize = mFrameWidth*mFrameHeight * 3 / 2;
            }else if(mFrameFormat == ASVL_PAF_YUYV){
                mFrameDataSize = mFrameWidth*mFrameHeight *2;
            }else {
                mFrameDataSize = mFrameWidth*mFrameHeight *4;
            }

            mFrameData = new unsigned char[mFrameDataSize];
            if(mFrameWidth>mFrameHeight){
                mWidgetWidth = 640;
                mWidgetHeight = mFrameHeight*mWidgetWidth/mFrameWidth;
            }else{
                mWidgetHeight = 640;
                mWidgetWidth = mFrameWidth*mWidgetHeight/mFrameHeight;
            }

            setFixedSize(mWidgetWidth, mWidgetHeight);
            parentWidget()->adjustSize();
        }else{
            frameSource->ReadFrame(mFrameData,mFrameDataSize);
            mGLHelper->setMirror(bFrameMirrored);
            mGLHelper->drawTexture(mFrameData, mFrameFormat, mFrameWidth, mFrameHeight);

            if(mArcFaceEngine&&mArcFaceEngine->processFrame(mFrameData,mFrameWidth,mFrameHeight,mFrameFormat)){

                for (int i = 0; i < mArcFaceEngine->mFaceNum; i++) {

                     float rectPoints[8];
                     MRECT *rect = &mArcFaceEngine->mFaceRect[i];
                     rectPoints[0] = (float)rect->left;
                     rectPoints[1] = (float)rect->top;
                     rectPoints[2] = (float)rect->right;
                     rectPoints[3] = (float)rect->top;
                     rectPoints[4] = (float)rect->right;
                     rectPoints[5] = (float)rect->bottom;
                     rectPoints[6] = (float)rect->left;
                     rectPoints[7] = (float)rect->bottom;

                     mGLHelper->drawPoints(4, rectPoints, mFrameWidth, mFrameHeight, 1.0f, 1.0f, 0.0f, 1.0f, GL_LINE_LOOP);
                 }

                 painter.endNativePainting();
                 bNativePainting = false;
                 for (int i = 0; i < mArcFaceEngine->mFaceNum; i++) {
                    if(mArcFaceEngine->mFaceIndex[i]>=0){
                        int leftInWidget = mArcFaceEngine->mFaceRect[i].left*mWidgetWidth/mFrameWidth;
                        int topInWidget = mArcFaceEngine->mFaceRect[i].top*mWidgetHeight/mFrameHeight;
                        int rightInWidget = mArcFaceEngine->mFaceRect[i].right*mWidgetWidth/mFrameWidth;

                        if(bFrameMirrored){
                            painter.drawText(QPointF(mWidgetWidth-1-rightInWidget,topInWidget), mArcFaceEngine->mFaceName[i]);
                        }else{
                            painter.drawText(QPointF(leftInWidget,topInWidget), mArcFaceEngine->mFaceName[i]);
                        }
                    #if 1
                        if(mArcFaceEngine->mFaceValue[i]>(2*30)){
                            QBuffer buffer(&mArcFaceEngine->mRegisterFaces[mArcFaceEngine->mFaceIndex[i]].thumbnail);
                            buffer.open( QIODevice::ReadOnly );
                            QImageReader reader(&buffer, "PNG");
                            QImage image = reader.read();

                            QWidget *parentwidget = parentWidget();
                            QPoint pos = parentwidget->mapToGlobal(QPoint(0,0));
                            QSize size = parentwidget->size();
                            FaceViewWidget *faceview = new FaceViewWidget(this,mArcFaceEngine->mFaceName[i],image);
                            faceview->move(pos.x()+size.width(), pos.y());
                            faceview->show();
                            mArcFaceEngine->mFaceValue[0] = -(6*30);
                        }
                    #endif
                    }
                }
            }
        }
    }

    if(bNativePainting){
        painter.endNativePainting();
    }
	update();
}

void MainWidget::resizeGL(int width, int height)
{
	glViewport(mWidgetWidth, mWidgetHeight, 0, 0);
}
