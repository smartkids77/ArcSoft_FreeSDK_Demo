#include <QtWidgets>
#include <QDebug>

#include "mainwidget.h"
#include "window.h"


Window::Window()
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainWidget = new MainWidget;
    mainLayout->addWidget(mainWidget);
	registerBtn = new QPushButton(tr("register"), this);
	cameraBtn = new QPushButton(tr("camera"), this);
    videoBtn = new QPushButton(tr("video"), this);
	mainLayout->addWidget(registerBtn);
	mainLayout->addWidget(cameraBtn);
    mainLayout->addWidget(videoBtn);

    connect(registerBtn, &QPushButton::clicked, mainWidget, &MainWidget::registerFace);
    connect(cameraBtn, &QPushButton::clicked, mainWidget, &MainWidget::openCamera);
    connect(videoBtn, &QPushButton::clicked, mainWidget, &MainWidget::openVideoFile);
	setLayout(mainLayout);

	setWindowTitle(tr("AFR Demo"));
}

