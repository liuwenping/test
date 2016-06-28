#include "tree.h"
#include "shape.h"


tree::tree()
{
	clearSelectedShape();
	clearChild();
	clearStackParent();
}


tree::~tree()
{

}

void tree::addSelectedShape(shape *curShape)
{
	selectedShape.append(curShape);
}

void tree::addChild(shape *curShape)
{
	child.append(curShape);
}

void tree::addStackParent(shape *curShape)
{
	stackParent.append(curShape);
}


void tree::clearSelectedShape()
{
	selectedShape.clear();
}

void tree::clearChild()
{
	child.clear();
}

void tree::clearStackParent()
{
	stackParent.clear();
}

QList<shape *> tree::getSelectedShape()
{
	return selectedShape;
}

QList<shape *> tree::getChild()
{
	return child;
}

QList<shape *> tree::getStackParent()
{
	return stackParent;
}