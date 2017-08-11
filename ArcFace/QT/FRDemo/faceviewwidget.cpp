#include "faceviewwidget.h"
#include <QtWidgets>

FaceViewWidget::FaceViewWidget(QWidget *parent,int id,QString name,QImage image) : QDialog(parent)
{
    mID = id;
    mName = name;

    setWindowTitle(mName);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel *pLabel = new QLabel;
    pLabel->setPixmap(QPixmap::fromImage(image).scaled(128,128,Qt::KeepAspectRatio));
    mainLayout->addWidget(pLabel);
    setLayout(mainLayout);
}


void FaceViewWidget::onBroadCast(int id,float fScore)
{
    if(id == mID){
        setWindowTitle(mName+"   "+QString::number(fScore));
        activateWindow();
    }else{
        setWindowTitle(mName);
    }
}

void FaceViewWidget::closeEvent(QCloseEvent *event)
{
    emit removeFaceSignal(mID);
    event->accept();
}
