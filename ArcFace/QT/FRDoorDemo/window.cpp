#include <QtWidgets>
#include <QDebug>

#include "mainwidget.h"
#include "window.h"


Window::Window()
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainWidget = new MainWidget;
    mainLayout->addWidget(mainWidget);
    loadBtn = new QPushButton(tr("load DB"), this);
	cameraBtn = new QPushButton(tr("camera"), this);
    videoBtn = new QPushButton(tr("video"), this);

    mainLayout->addWidget(loadBtn);
	mainLayout->addWidget(cameraBtn);
    mainLayout->addWidget(videoBtn);

    connect(loadBtn, &QPushButton::clicked, mainWidget, &MainWidget::loadFaceDB);
    connect(cameraBtn, &QPushButton::clicked, mainWidget, &MainWidget::openCamera);
    connect(videoBtn, &QPushButton::clicked, mainWidget, &MainWidget::openVideoFile);

	setLayout(mainLayout);

	setWindowTitle(tr("AFR Demo"));
}
