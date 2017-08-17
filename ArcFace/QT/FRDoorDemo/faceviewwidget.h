#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QDialog>

class FaceViewWidget : public QDialog
{
    Q_OBJECT
public:
    explicit FaceViewWidget(QWidget *parent,QString name,QImage image);
    virtual ~FaceViewWidget();
protected:
    void timerEvent( QTimerEvent *event );
private:
    QString mName;
    int mTimer;
};

#endif // FACEWIDGET_H
