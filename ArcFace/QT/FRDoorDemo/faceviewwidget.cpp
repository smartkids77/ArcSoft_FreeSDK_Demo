#include "faceviewwidget.h"
#include <QtWidgets>

FaceViewWidget::FaceViewWidget(QWidget *parent,QString name,QImage image) : QDialog(parent)
{
    mName = name;

    setWindowTitle(mName);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel *pLabel = new QLabel;
    pLabel->setPixmap(QPixmap::fromImage(image).scaled(128,128,Qt::KeepAspectRatio));
    mainLayout->addWidget(pLabel);
    setLayout(mainLayout);

    mTimer = startTimer(3000);
}

FaceViewWidget::~FaceViewWidget(){
    if ( mTimer != 0 ){
        killTimer(mTimer);
        mTimer = 0;
    }
}

void FaceViewWidget::timerEvent(QTimerEvent *event ){
    if (mTimer != 0 ){
        killTimer(mTimer);
        mTimer = 0;
    }
    done(0);
}


