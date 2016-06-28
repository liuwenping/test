#include <QtGui>

#include "kcurfileanalyzer.h"
#include "drawgrah.h"
#include "shape.h"
#include "tree.h"
#include "view.h"

DrawGrah::DrawGrah(view *viewPtr)
{
	this->viewPtr = viewPtr;
	combinationing = false;
	curState = NOSTATE;
	m_startPoint = m_endPoint = QPoint(0, 0);
	m_tree = new tree;

	setMouseTracking(true);
}

DrawGrah::~DrawGrah()
{
	if (m_tree)
	{
		delete m_tree;
		m_tree = NULL;
	}
}

// void DrawGrah::paintEvent(QPaintEvent *event)
// {
// 	QPainter painter(this);
// 	painter.setRenderHint(QPainter::Antialiasing);
// 	
// 	drawShape(painter);
// 	drawHandle(painter);
// 
// 	if(curState != NOSTATE) 
// 		drawingDotLine(painter);
// }

void DrawGrah::setSelectedShapeList(shape *curShape)
{
	if(!m_tree->selectedShape.contains(curShape))
	{
		if(!combinationing)
			m_tree->selectedShape.clear();
		if (curShape)
		{
			m_tree->selectedShape.append(curShape);
		}
	}
}

void DrawGrah::onMousePress(QPoint curPoint)
{
	m_startPoint = curPoint;

	//实现点击测试，并设置当前状态
	QPair<shape *, int> result = itemAtHandle(curPoint);
	if(!result.first)
	{
		curShape = itemAt(curPoint);
		if(curShape)
		{
			curState = MOVING;
		}
	}
	else
	{
		curShape = result.first;
		if(result.second == 0)
			curState = ROTATING;
		else
			curState = ZOOMING;
	}

	//用来设置“selectedShape”里面的内容,将选中的图形放入selectedShape中///
	setSelectedShapeList(curShape);

	//设置点击后的光标样式
	setMouseCurSor(curPoint);
}

void DrawGrah::onMouseMove(QMouseEvent *event)
{
	m_endPoint = event->pos();

	setMouseCurSor(event->pos());
// 	if((curState == DRAWING) && (event->buttons() & Qt::LeftButton))
// 		update();
// 	if((curState == MOVING) && (event->buttons() & Qt::LeftButton))
// 		update();
// 	if((curState == ROTATING) && (event->buttons() & Qt::LeftButton))
// 		update();
}

void DrawGrah::onMouseRelease(QMouseEvent *event)
{
	m_endPoint = event->pos();
	if((curState == DRAWING) && (event->button() == Qt::LeftButton))
	{	
		curState = NOSTATE;
		createShape(m_shapeType, m_startPoint, m_endPoint);
	}
	if((curState == MOVING) && (event->button() == Qt::LeftButton))
	{
		curState = NOSTATE;
		updateSelectedShapeOffset(m_startPoint, m_endPoint);
	}
	if((curState == ZOOMING) && (event->button() == Qt::LeftButton))
	{
		curState = NOSTATE;
		//更新选中图形的所有数据//
	}
	if((curState == ROTATING) && (event->button() == Qt::LeftButton))
	{
		curState = NOSTATE;
		applyRotateToShape();
		//更新选中图形的所有数据//
	}
	setMouseCurSor(event->pos());
	//update();
}

void DrawGrah::onKeyPress(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
	{
		combinationing = true;
	}
}

void DrawGrah::onKeyRelease(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
	{
		combinationing = false;
	}
}

void DrawGrah::setMouseCurSor(QPoint curPoint)
{
	QPair<shape *, int> result = itemAtHandle(curPoint);

	//如果鼠标不在handle点上///
	if(!result.first)
	{
		result.first = itemAt(curPoint);
		if(result.first)
			viewPtr->setCursor(Qt::SizeAllCursor);
		else
			viewPtr->setCursor(Qt::ArrowCursor);
	}
	else
	{
		switch (result.second)
		{
		case 0:
			//设置旋转鼠标样式////
			{
				KCurFileAnalyzer KF;
				QCursor cs = KF.loadFile(QString(":/images/CURSOR_MoveToRotatePoint.cur"));
				viewPtr->setCursor(cs);
			}
			m_direct = tree::ROTHANDLE;
			break;
		case 1:
			m_direct = tree::TOPLEFT;
			viewPtr->setCursor(Qt::SizeFDiagCursor);
			break;
		case 2:
			m_direct = tree::TOPRIGHT;
			viewPtr->setCursor(Qt::SizeBDiagCursor);
			break;
		case 3:
			m_direct = tree::BOTTOMLEFT;
			viewPtr->setCursor(Qt::SizeBDiagCursor);
			break;
		case 4:
			m_direct = tree::BOTTOMRIGHT;
			viewPtr->setCursor(Qt::SizeFDiagCursor);
			break;
		case 5:
			m_direct = tree::TOPMIDDLE;
			viewPtr->setCursor(Qt::SizeVerCursor);
			break;
		case 6:
			m_direct = tree::BOTTOMMIDDLE;
			viewPtr->setCursor(Qt::SizeVerCursor);
			break;
		case 7:
			m_direct = tree::LEFTMIDDLE;
			viewPtr->setCursor(Qt::SizeHorCursor);
			break;
		case 8:
			m_direct = tree::RIGHTMIDDLE;
			viewPtr->setCursor(Qt::SizeHorCursor);
			break;
		}
	}

	if(curState == DRAWING || curState == ZOOMING)
		viewPtr->setCursor(Qt::CrossCursor);
	if(curState == ROTATING)
	{
		KCurFileAnalyzer KF;
		QCursor cs = KF.loadFile(QString(":/images/CURSOR_MoveToRotatePointandOption.cur"));
		viewPtr->setCursor(cs);
	}
}

void DrawGrah::drawingDotLine(QPainter &painter)
{
	if(curState == DRAWING)
	{
		painter.save();
		painter.setPen(Qt::DotLine);
		if(m_shapeType == tree::RECT)
		{
			painter.drawRect(QRect(m_startPoint, m_endPoint));
		}
		else if(m_shapeType == tree::CIRCLE)
		{
			painter.drawEllipse(QRect(m_startPoint, m_endPoint));
		}
		else if(m_shapeType == tree::TRIANGLE)
		{
			QPainterPath path;
			path.moveTo(m_startPoint.x(), m_endPoint.y());
			path.lineTo((m_startPoint.x() + m_endPoint.x()) / 2, m_startPoint.y());
			path.lineTo(m_endPoint);
			path.lineTo(m_startPoint.x(), m_endPoint.y());
			painter.drawPath(path);
		}
		painter.restore();
	}
	else if(curState == MOVING)
	{
		foreach(shape *moveShape, m_tree->selectedShape)
		{	
			painter.save();
			painter.setPen(Qt::DotLine);
			painter.translate(m_endPoint - m_startPoint);

			QPainterPath path = moveShape->m_path;
			QTransform trans;
			getAllMatrix(trans, moveShape);
			path = trans.map(path);
			painter.drawPath(path);
			
			painter.restore();	
		}
	}
	else if(curState == ZOOMING)
	{
		//用来大小变换///
// 		foreach(shape *zoomShape, m_tree->selectedShape)
// 		{	
// 			//用来画图形改变大小时的虚线
//  			painter.save();
//  			painter.setPen(Qt::DotLine);
//  			QPainterPath path = calcuZoomPath(zoomShape);
//  			painter.drawPath(path);
//  			painter.restore();	
// 		}
	}
	else if(curState == ROTATING)
	{
// 		qreal centerX, centerY;
// 		if(curShape->child.size() > 0)
// 		{
// 			centerX = curShape->m_choffx + curShape->width / 2;
// 			centerY = curShape->m_choffy + curShape->height / 2;
// 		}
// 		else
// 		{
// 			centerX = curShape->m_offx + curShape->width / 2;
// 			centerY = curShape->m_offy + curShape->height / 2;
// 		}
		qreal centerX = curShape->m_offx + curShape->width / 2; 
		qreal centerY = curShape->m_offy + curShape->height / 2;
		QPoint center(centerX, centerY);

		QPoint vector1 = m_startPoint - center; 
		QPoint vector2 = m_endPoint - center;

		double PI = 2 * asin(1.0);
		int product = vector1.x() * vector2.x() + vector1.y() * vector2.y();
		double mod_vector1 = sqrtl(vector1.x() * vector1.x() + vector1.y() * vector1.y());
		double mod_vector2 = sqrtl(vector2.x() * vector2.x() + vector2.y() * vector2.y());
		double deltaRot = acos(product / (mod_vector1 * mod_vector2));
		int rot = int(deltaRot * 180 / PI);

		int S = (m_startPoint.x() - m_endPoint.x()) * (centerY - m_endPoint.y()) - (m_startPoint.y() - m_endPoint.y()) * (centerX - m_endPoint.x());
		if(S > 0)
		{
			rot = 360 - rot;
		}
		foreach(shape *rotateShape, m_tree->selectedShape)
		{
			painter.save();
			painter.setPen(Qt::DotLine);

			QPainterPath path = rotateShape->m_path;
			QTransform trans;
			getAllMatrix(trans, rotateShape);

			qreal coreX = rotateShape->m_offx + rotateShape->width / 2; 
			qreal coreY = rotateShape->m_offy + rotateShape->height / 2;
			
			QTransform rotate;
			rotate.translate(coreX, coreY);
			rotate.rotate(rot);
			rotate.translate(-coreX, -coreY);
			trans = trans * rotate;

			path = trans.map(path);
			painter.drawPath(path);

			painter.restore();
		}
	}
}

void DrawGrah::applyRotateToShape()
{
	qreal centerX = curShape->m_offx + curShape->width / 2; 
	qreal centerY = curShape->m_offy + curShape->height / 2;
	QPoint center(centerX, centerY);

	QPoint vector1 = m_startPoint - center; 
	QPoint vector2 = m_endPoint - center;

	double PI = 2 * asin(1.0);
	int product = vector1.x() * vector2.x() + vector1.y() * vector2.y();
	double mod_vector1 = sqrtl(vector1.x() * vector1.x() + vector1.y() * vector1.y());
	double mod_vector2 = sqrtl(vector2.x() * vector2.x() + vector2.y() * vector2.y());
	double deltaRot = acos(product / (mod_vector1 * mod_vector2));
	int rot = int(deltaRot * 180 / PI);

	int S = (m_startPoint.x() - m_endPoint.x()) * (centerY - m_endPoint.y()) - (m_startPoint.y() - m_endPoint.y()) * (centerX - m_endPoint.x());
	if(S > 0)
	{
		rot = 360 - rot;
	}
	foreach(shape *rotateShape, m_tree->selectedShape)
	{
		rotateShape->rot = (rotateShape->rot + rot) % 360;
	}
}

//下面的函数用来计算改变大小后的path////
 QPainterPath DrawGrah::calcuZoomPath(shape *zoomShape)
 {
 	QPoint offset;
 	qreal width;
 	qreal height;
 
 	QTransform trans, reverseTrans;
 	getAllMatrix(trans, zoomShape);
 	
 	getAllReverseMatrix(reverseTrans, zoomShape);
 
 
 	QPainterPath path;
 	QPoint start = reverseTrans.map(m_startPoint);
 	QPoint end = reverseTrans.map(m_endPoint);
 	QPoint translate = end - start;
 
 	switch (m_direct)
 	{
	case tree::ROTHANDLE:
 		
 		break;
 	case tree::TOPLEFT:
 		
 		break;
 	case tree::BOTTOMRIGHT:
 
 		break;
 	}
 
 	//path = trans.map(zoomShape->m_path);
 	return path;
 }

 void DrawGrah::getAllReverseMatrix(QTransform &trans, shape *shapeNode)
 {
	 if(shapeNode->parent == NULL)
	 {
		 qreal centralX = shapeNode->m_offx + shapeNode->width / 2;
		 qreal centralY = shapeNode->m_offy + shapeNode->height / 2;
		 // QTransform flip;
		 // flip.translate(centralX, centralY);
		 // flip.scale(shapeNode->flipH ? -1 : 1, shapeNode->flipV ? -1 : 1);
		 // flip.translate(-centralX, -centralY);

		 QTransform rotate;
		 rotate.translate(centralX, centralY);
		 rotate.rotate(-shapeNode->rot);
		 rotate.translate(-centralX, -centralY);

		 trans = rotate;
		 /*trans = flip * rotate;*/
	 }
 }

void DrawGrah::deleteSelectedShape()
{
	foreach(shape *delShape, m_tree->selectedShape)
	{
		m_tree->child.removeOne(delShape);
		deleteShape(delShape);
	}
	m_tree->selectedShape.clear();
	//update();
}

void DrawGrah::groupShape()
{
	createGroupShape();
	//update();
}

void DrawGrah::splitGroup()
{
	splitGroupShape();
	//update();
}

void DrawGrah::createShape(tree::enumShape shapeType, QPoint startPoint, QPoint endPoint)
{
	shape *tempShape = new shape;
	QPainterPath tempPath;
	tempPath.addRect(QRect(startPoint, endPoint));
	QPointF start = tempPath.boundingRect().topLeft();
	QPointF end = tempPath.boundingRect().bottomRight();
	QPainterPath path;
	if(shapeType == tree::RECT)
	{
		tempShape->name = "Rectangle";
		path.addRect(QRectF(start, end));
	}
	else if(shapeType == tree::CIRCLE)
	{
		tempShape->name = "Circle";
		path.addEllipse(QRectF(start, end));
	}
	else if(shapeType == tree::TRIANGLE)
	{
		tempShape->name = "Triangle";
		path.moveTo(start.x(), end.y());
		path.lineTo((start.x() + end.x()) / 2, start.y());
		path.lineTo(end);
		path.lineTo(start.x(), end.y());
	}
	tempShape->m_path = path;
	tempShape->m_offx = path.boundingRect().topLeft().x();
	tempShape->m_offy = path.boundingRect().topLeft().y();
	tempShape->width = path.boundingRect().width();
	tempShape->height = path.boundingRect().height();
	if(startPoint.x() > endPoint.x()) tempShape->flipH = true;
	if(startPoint.y() > endPoint.y()) tempShape->flipV = true;
	tempShape->parent = NULL;
	tempShape->color = randomColor();

	m_tree->child.append(tempShape);
}

void DrawGrah::createGroupShape()
{
	//如果当前有两个以上的图片被选中，则进行组合，否则什么都不做///
	if(m_tree->selectedShape.size() > 1)
	{
		shape *groupShape = new shape; 
		QPainterPath path;
		foreach(shape *childShape, m_tree->selectedShape)
		{
			m_tree->child.removeOne(childShape);
			groupShape->child.append(childShape);
			childShape->parent = groupShape;
			QTransform trans;
			getAllMatrix(trans, childShape);
			path.addPath(trans.map(childShape->m_path));
		}
		groupShape->name = "Rectangle";
		groupShape->m_path.addRect(path.boundingRect());
		groupShape->m_offx = path.boundingRect().topLeft().x();
		groupShape->m_offy = path.boundingRect().topLeft().y();
		groupShape->width = path.boundingRect().width();
		groupShape->height = path.boundingRect().height();
		groupShape->parent = NULL;

		m_tree->child.append(groupShape);
		m_tree->selectedShape.clear();
		m_tree->selectedShape.append(groupShape);
	}
}

void DrawGrah::splitGroupShape()
{
	foreach(shape *childShape, m_tree->selectedShape)
	{
		//如果有孩子则为组合图形//
		QList<shape *> childList = childShape->child;
		if(childList.size() > 0)
		{
			// 	stackParent.append(childShape);
			// 	updateSplitGroupShape(childShape);
			// 	stackParent.removeLast();	

			QTransform trans;
			getAllMatrix(trans, childShape);

			for(int i = childList.size() - 1; i >= 0; --i)
			{
				qreal coreX = childList[i]->m_offx + childList[i]->width / 2;
				qreal coreY = childList[i]->m_offy + childList[i]->height / 2;

				QPoint core(coreX, coreY);

				QPoint rotateCore = trans.map(core);

				//QPoint offset = rotateCore - core;
				qreal dx = rotateCore.x() - childList[i]->width / 2 - childList[i]->m_offx;
				qreal dy = rotateCore.y() - childList[i]->height / 2 - childList[i]->m_offy;
				updateGroupShapeOffset(childList[i], QPoint(dx, dy));

				childList[i]->m_offx = rotateCore.x() - childList[i]->width / 2;
				childList[i]->m_offy = rotateCore.y() - childList[i]->height / 2;



				reCalcuPath(childList[i]);

				childList[i]->rot = (childShape->rot + childList[i]->rot) % 360;
				childList[i]->parent = NULL;
				m_tree->child.append(childList[i]);
				m_tree->selectedShape.append(childList[i]);
			}
			m_tree->selectedShape.removeOne(childShape);
			m_tree->child.removeOne(childShape);
		}
	}
}

void DrawGrah::updateSelectedShapeOffset(QPoint start, QPoint end)
{
	QPoint shift;
	foreach(shape *moveShape, m_tree->selectedShape)
	{
		if(moveShape->child.size() > 0)
		{
			// 			QTransform trans;
			// 			getAllMatrix(trans, moveShape);
			// 			trans = trans.inverted();
			// 			start = trans.map(start);
			// 			end = trans.map(end);
			shift = end - start;
			moveShape->m_offx += shift.x();
			moveShape->m_offy += shift.y();
			reCalcuPath(moveShape);
			updateGroupShapeOffset(moveShape, shift);

		}
		else
		{
			shift = end - start;
			moveShape->m_offx += shift.x();
			moveShape->m_offy += shift.y();
			reCalcuPath(moveShape);
		}
	}
}

void DrawGrah::updateGroupShapeOffset(shape *curShape, QPoint shift)
{
	foreach (shape *updateShape, curShape->child)
	{
		if(updateShape->child.size() > 0)
		{
			updateShape->m_offx += shift.x();
			updateShape->m_offy += shift.y();
			reCalcuPath(updateShape);
			updateGroupShapeOffset(updateShape, shift);
		}
		else
		{
			updateShape->m_offx += shift.x();
			updateShape->m_offy += shift.y();
			reCalcuPath(updateShape);
		}
	}
}

void DrawGrah::reCalcuPath(shape *moveShape)
{
	QPainterPath path;
	QPoint offset = QPoint(moveShape->m_offx, moveShape->m_offy);
	qreal w = moveShape->width;
	qreal h = moveShape->height;
	if (moveShape->name == "Rectangle")
	{
		path.addRect(QRect(offset, QPoint(offset.x() + w, offset.y() + h)));
	} 
	else if (moveShape->name == "Circle")
	{
		path.addEllipse(QRect(offset, QPoint(offset.x() + w, offset.y() + h)));
	}
	else if(moveShape->name == "Triangle")
	{
		path.moveTo(offset.x(), offset.y() + h);
		path.lineTo(offset.x() + w / 2, offset.y());
		path.lineTo(offset.x() + w, offset.y() + h);
		path.lineTo(offset.x(), offset.y() + h);
	}
	moveShape->m_path = path;
}

void DrawGrah::drawShape(QPainter &painter)
{
	foreach(shape *tempChild, m_tree->child)
	{
		//如果有孩子，即为组合图形///
		if(tempChild->child.size() > 0)
		{
			m_tree->stackParent.append(tempChild);
			drawGroupShape(tempChild, painter);
			m_tree->stackParent.removeLast();
		}
		else
		{
			drawSingalShape(tempChild, painter);
		}
	}
}

void DrawGrah::drawGroupShape(shape *group, QPainter &painter)
{
	foreach(shape *tempChild, group->child)
	{
		//如果有孩子，即为组合图形//
		if(tempChild->child.size() > 0)
		{
			m_tree->stackParent.append(tempChild);
			drawGroupShape(tempChild, painter);
			m_tree->stackParent.removeLast();
		}
		else
		{
			drawSingalShape(tempChild, painter);
		}
	}
}

void DrawGrah::drawSingalShape(shape *singleShape, QPainter &painter)
{
	QTransform trans;
	getAllMatrix(trans, singleShape);
	drawItself(singleShape, trans, painter);
}

void DrawGrah::drawItself(shape *curShape, QTransform &trans, QPainter &painter)
{
	painter.save();
	painter.setBrush(curShape->color);
	painter.setTransform(trans);
	painter.drawPath(curShape->m_path);
	painter.restore();
}

void DrawGrah::drawHandle(QPainter &painter)
{
	foreach(shape *tempShape, m_tree->selectedShape)
	{
		QTransform trans;
		getAllMatrix(trans, tempShape);
		drawItselfHandle(tempShape, trans, painter);
	}
}

void DrawGrah::drawItselfHandle(shape *curShape, QTransform &trans, QPainter &painter)
{
	qreal offx = curShape->m_offx;
	qreal offy = curShape->m_offy;
	qreal width = curShape->width; 
	qreal height = curShape->height;
	QPoint ridous(3, 3);
	painter.save();
	painter.setBrush(Qt::white);
	painter.setTransform(trans);
	//painter.drawText(QRect(QPoint(m_offx, m_offy) + ridous, QPoint(m_offx, m_offy) - ridous),Qt::AlignCenter,QString("1")); //用来测试画出来的小圆的位置
	painter.drawEllipse(QRect(QPoint(offx, offy) + ridous, QPoint(offx, offy) - ridous));											//转换前的上左//
	painter.drawEllipse(QRect(QPoint(offx + width, offy) + ridous, QPoint(offx + width, offy) - ridous));							//转换前的上右//
	painter.drawEllipse(QRect(QPoint(offx, offy + height) + ridous, QPoint(offx, offy + height) - ridous));							//转换前的下左//
	painter.drawEllipse(QRect(QPoint(offx + width, offy + height) + ridous, QPoint(offx + width, offy + height) - ridous));			//转换前的下右//
	painter.drawEllipse(QRect(QPoint(offx + width / 2, offy) + ridous, QPoint(offx + width / 2, offy) - ridous));					//转换前的上中//
	painter.drawEllipse(QRect(QPoint(offx + width / 2, offy + height) + ridous, QPoint(offx + width / 2, offy + height) - ridous));	//转换前的下中//
	painter.drawEllipse(QRect(QPoint(offx, offy + height / 2) + ridous, QPoint(offx, offy + height / 2) - ridous));					//转换前的左中//
	painter.drawEllipse(QRect(QPoint(offx + width, offy + height / 2) + ridous, QPoint(offx + width, offy + height / 2) - ridous));	//转换前的右中//
	painter.setBrush(Qt::green);
	painter.drawEllipse(QRect(QPoint(offx + width / 2, offy - 15) + ridous, QPoint(offx + width / 2, offy - 15) - ridous));			//旋转手柄//////
	painter.restore();
}

int DrawGrah::judgeHandleAt(shape *curShape, QTransform &trans, QPoint curPoint)
{
	qreal offx = curShape->m_offx;
	qreal offy = curShape->m_offy;
	qreal width = curShape->width; 
	qreal height = curShape->height;
	QPoint ridous(3, 3);
	QVector<QPainterPath> handlePath(9);

	handlePath[0].addEllipse(QRect(QPoint(offx + width / 2, offy - 15) + ridous, QPoint(offx + width / 2, offy - 15) - ridous)); //旋转手柄//
	handlePath[1].addEllipse(QRect(QPoint(offx, offy) + ridous, QPoint(offx, offy) - ridous));
	handlePath[2].addEllipse(QRect(QPoint(offx + width, offy) + ridous, QPoint(offx + width, offy) - ridous));
	handlePath[3].addEllipse(QRect(QPoint(offx, offy + height) + ridous, QPoint(offx, offy + height) - ridous));
	handlePath[4].addEllipse(QRect(QPoint(offx + width, offy + height) + ridous, QPoint(offx + width, offy + height) - ridous));
	handlePath[5].addEllipse(QRect(QPoint(offx + width / 2, offy) + ridous, QPoint(offx + width / 2, offy) - ridous));
	handlePath[6].addEllipse(QRect(QPoint(offx + width / 2, offy + height) + ridous, QPoint(offx + width / 2, offy + height) - ridous));
	handlePath[7].addEllipse(QRect(QPoint(offx, offy + height / 2) + ridous, QPoint(offx, offy + height / 2) - ridous));
	handlePath[8].addEllipse(QRect(QPoint(offx + width, offy + height / 2) + ridous, QPoint(offx + width, offy + height / 2) - ridous));


	for(int i = 0; i < 9; i++)
	{
		handlePath[i] = trans.map(handlePath[i]);
		if (handlePath[i].contains(curPoint)) //转换后的小圆卷包含当前这一点则返回这一点的下标///
		{
			return i;
		}
	}
	return -1;
}

void DrawGrah::getAllMatrix(QTransform &trans, shape *shapeNode)
{
	qreal centralX = shapeNode->m_offx + shapeNode->width / 2;
	qreal centralY = shapeNode->m_offy + shapeNode->height / 2;
	QTransform flip;
	flip.translate(centralX, centralY);
	flip.scale(shapeNode->flipH ? -1 : 1, shapeNode->flipV ? -1 : 1);
	flip.translate(-centralX, -centralY);

	QTransform rotate;
	rotate.translate(centralX, centralY);
	rotate.rotate(shapeNode->rot);
	rotate.translate(-centralX, -centralY);

	trans = flip * rotate;

	if(shapeNode->parent != NULL)
	{
		for(int i = m_tree->stackParent.size() - 1; i >= 0; --i)
		{
			qreal coreX = m_tree->stackParent[i]->m_offx + m_tree->stackParent[i]->width / 2;
			qreal coreY = m_tree->stackParent[i]->m_offy + m_tree->stackParent[i]->height / 2;

			QTransform parentFlip;
			parentFlip.translate(coreX, coreY);
			parentFlip.scale(m_tree->stackParent[i]->flipH ? -1 : 1, m_tree->stackParent[i]->flipV ? -1 : 1);
			parentFlip.translate(-coreX, -coreY);

			QTransform parentRotate;
			parentRotate.translate(coreX, coreY);
			parentRotate.rotate(m_tree->stackParent[i]->rot);
			parentRotate.translate(-coreX, -coreY);

			trans = trans * parentFlip * parentRotate;
		}
	}
}

void DrawGrah::deleteShape(shape *delShape)
{
	int count = delShape->child.size();
	if(count > 0)
	{
		foreach(shape *tempShape, delShape->child)
		{
			deleteShape(tempShape);
		}
	}
	else
	{
		if(delShape)
		{
			delete delShape;
			delShape = NULL;
		}
	}
}

QColor DrawGrah::randomColor()
{
	return QColor::fromHsv(qrand() % 256, 255, 190);
}

shape *DrawGrah::itemAt(QPoint curPoint)
{
	for(int i = m_tree->child.size() - 1; i >= 0; --i)
	{
		shape *tempChild = m_tree->child[i];
		QTransform trans;
		getAllMatrix(trans, tempChild);
		QPainterPath path = trans.map(tempChild->m_path);
		if(path.contains(curPoint))
		{
			return tempChild;
		}
	}
	return 0;
}

QPair<shape *, int> DrawGrah::itemAtHandle(QPoint curPoint)
{
	QPair<shape *, int> result(0, -1);
	for(int i = m_tree->selectedShape.size() - 1; i >= 0; --i)
	{
		shape *tempShape = m_tree->selectedShape[i];
		QTransform trans;
		getAllMatrix(trans, tempShape);
		result.second = judgeHandleAt(tempShape, trans, curPoint);
		if(result.second != -1) 
		{
			result.first = tempShape;
			break;
		}
	}
	return result;
}