#include <QtGui>

#include "shape.h"

shape::shape()
{
	m_offx = 0;
	m_offy = 0;
	width = 0;
	height = 0;
	flipH = false;
	flipV = false;
	rot = 0;

	ridous = QPoint(3, 3);
}


shape::~shape()
{

}
