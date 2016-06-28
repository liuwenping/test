#ifndef SHAPE_H
#define SHAPE_H

#include <QtGui>

class shape
{
public:
	shape();
	~shape();

	QString name;	
	QPainterPath m_path;
	qreal m_offx;
	qreal m_offy;
	qreal width;
	qreal height;
//------------------------------------------
//为组合提供的另外两对参数，用来计算scale
//  	qreal m_choffx;
//  	qreal m_choffy;
//  	qreal chwidth;
//  	qreal chheight;
//------------------------------------------
	bool flipH;
	bool flipV;
	int rot;


	QPoint ridous;
	QColor color;
	shape *parent;
	QList<shape *> child;
};

#endif