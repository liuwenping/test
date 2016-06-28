#ifndef DRAWGRAH_H
#define DRAWGRAH_H

#include <QtGui>
#include "tree.h"

class view;

class DrawGrah : public QWidget
{
	Q_OBJECT

public:
	
	enum state{NOSTATE, DRAWING, MOVING, ZOOMING, ROTATING};
	DrawGrah(view *viewPtr);
	~DrawGrah();
 
 	//void paintEvent(QPaintEvent *event);


 	void onMousePress(QPoint curPoint);
 	void onMouseMove(QMouseEvent *event);
 	void onMouseRelease(QMouseEvent *event);
 	void onKeyPress(QKeyEvent *event);
 	void onKeyRelease(QKeyEvent *event);

	void groupShape();
	void splitGroup();
	void setSelectedShapeList(shape *curShape);
	void setMouseCurSor(QPoint curPoint);
	void deleteSelectedShape();
	void drawingDotLine(QPainter &painter);

	void applyRotateToShape();
	void drawShape(QPainter &painter);
	void drawHandle(QPainter &painter);
	void drawItselfHandle(shape *curShape, QTransform &trans, QPainter &painter);
	void drawSingalShape(shape *singleShape, QPainter &painter);
	void drawGroupShape(shape *group, QPainter &painter);
	void drawItself(shape *curShape, QTransform &trans, QPainter &painter);
	void getAllMatrix(QTransform &trans, shape *shapeNode);
	void getAllReverseMatrix(QTransform &trans, shape *shapeNode);
	void createShape(tree::enumShape shapeType, QPoint startPoint, QPoint endPoint);
	void createGroupShape();
	void splitGroupShape();
	void updateSplitGroupShape(shape *curShape);
	void updateSelectedShapeOffset(QPoint start, QPoint end);
	void updateGroupShapeOffset(shape *curShape, QPoint shift);
	void reCalcuPath(shape *moveShape);
	shape *itemAt(QPoint curPoint);
	QPair<shape *, int> itemAtHandle(QPoint curPoint);
	int judgeHandleAt(shape *curShape, QTransform &trans, QPoint curPoint);
	
	QColor randomColor();
	void deleteShape(shape *delShape);
	QPainterPath calcuZoomPath(shape *zoomShape);

	shape *curShape;
	state curState;
	tree::zoomDirect m_direct;
	tree *m_tree;
	tree::enumShape m_shapeType;

private:
	view *viewPtr;
	bool combinationing;
	QPoint m_startPoint;
	QPoint m_endPoint;
};

#endif // DRAWGRAH_H
