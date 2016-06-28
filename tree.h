#ifndef TREE_H
#define TREE_H

#include <QtGui>

class shape;

class tree
{
public:
	enum zoomDirect{ROTHANDLE = 0, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, TOPMIDDLE, BOTTOMMIDDLE, LEFTMIDDLE, RIGHTMIDDLE};
	enum enumShape{RECT = 1, CIRCLE, TRIANGLE};
	tree();
	~tree();
	
	void addSelectedShape(shape *curShape);
	void addChild(shape *curShape);
	void addStackParent(shape *curShape);

	void clearSelectedShape();
	void clearChild();
	void clearStackParent();

	QList<shape *> getSelectedShape();
	QList<shape *> getChild();
	QList<shape *> getStackParent();

	
	QList<shape *> selectedShape;
	QList<shape *> child;
	QList<shape *> stackParent;
};

#endif