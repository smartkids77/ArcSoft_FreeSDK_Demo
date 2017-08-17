#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QString>

class VideoFileSource;
class CameraSource;
class GLHelper;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

#include "arcfaceengine.h"

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

protected:
	void initializeGL() override;
	void paintGL() override;
    void resizeGL(int width, int height) override;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

public slots:
	void openCamera();
    void openVideoFile();

    void loadFaceDB();

private:

    GLHelper* mGLHelper;
    CameraSource *mCameraSource;
    VideoFileSource *mVideoFileSource;

    unsigned char *mFrameData;
    int mFrameDataSize;
    int mFrameWidth;
    int mFrameHeight;
    int mFrameFormat;
    bool bFrameMirrored;

	int mWidgetWidth;
	int mWidgetHeight;

    ArcFaceEngine *mArcFaceEngine;

    void openFile(QString filename);

};

#endif
