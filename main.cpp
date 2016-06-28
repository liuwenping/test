#include "view.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	view w;
	w.show();
	return a.exec();
}
