#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QPushButton>

class MainWidget;

class Window : public QWidget
{
	Q_OBJECT

public:
	Window();
private:
    MainWidget *mainWidget;
    QPushButton *loadBtn;
	QPushButton *cameraBtn;
    QPushButton *videoBtn;
};

#endif
