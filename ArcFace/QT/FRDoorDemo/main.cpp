#include <QApplication>
#include <QSurfaceFormat>

#include "window.h"


int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);

	Window window;
	window.show();
	return app.exec();
}
