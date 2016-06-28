#include <QtGui>

#include "view.h"
#include "drawgrah.h"

view::view(void)
{
	m_mainWidget = new DrawGrah(this);
	setCentralWidget(m_mainWidget);
	setMouseTracking(true);

	createAction();
	createMenu();
	createToolBar();
	createContextMenu();
	
	setWindowTitle(tr("DrawGrah"));
	setFixedSize(800, 500);

}

void view::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	m_mainWidget->drawShape(painter);
	m_mainWidget->drawHandle(painter);

	if(m_mainWidget->curState != DrawGrah::NOSTATE) 
		m_mainWidget->drawingDotLine(painter);
}

void view::mousePressEvent(QMouseEvent *event)
{
	m_mainWidget->onMousePress(event->pos());
}

void view::mouseMoveEvent(QMouseEvent *event)
{
	m_mainWidget->onMouseMove(event);
	if(((m_mainWidget->curState == DrawGrah::DRAWING) 
		|| (m_mainWidget->curState == DrawGrah::MOVING) 
		|| (m_mainWidget->curState == DrawGrah::ROTATING)) 
		&& (event->buttons() & Qt::LeftButton))
		update();
}

void view::mouseReleaseEvent(QMouseEvent *event)
{
	m_mainWidget->onMouseRelease(event);
	update();
}

void view::keyPressEvent(QKeyEvent *event)
{
	m_mainWidget->onKeyPress(event);
}

void view::keyReleaseEvent(QKeyEvent *event)
{
	m_mainWidget->onKeyRelease(event);
	update();
}

void view::createAction()
{
	m_newAction = new QAction(tr("&New"), this);
	m_newAction->setIcon(QIcon(":/images/new.png"));
	m_newAction->setShortcut(QKeySequence::New);
	connect(m_newAction, SIGNAL(triggered()), this, SLOT(newFile()));

 	m_exitAction = new QAction(tr("&Exit"), this);
 	m_exitAction->setShortcut(tr("Ctrl+Q"));
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

	m_groupAction = new QAction(tr("&Group"), this);
	m_groupAction->setShortcut(tr("Ctrl+G"));
	connect(m_groupAction, SIGNAL(triggered()), this, SLOT(createGroupShape()));

	m_splitAction = new QAction(tr("&Split"), this);
	m_splitAction->setShortcut(tr("Ctrl+U"));
	connect(m_splitAction, SIGNAL(triggered()), this, SLOT(splitGroupShape()));

	m_deleteAction = new QAction(tr("&Delete"), this);
	m_deleteAction->setShortcut(tr("Delete"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteShape()));

	m_basicShapeAction = new QAction(tr("&BasicShape"), this);
	m_basicShapeAction->setShortcut(tr("Ctrl+Shift+B"));
	connect(m_basicShapeAction, SIGNAL(triggered()), this, SLOT(basicShapeToolBar()));

	m_complexShapeAction = new QAction(tr("&ComplexShape"), this);
	m_complexShapeAction->setShortcut(tr("Ctrl+Shift+C"));
	connect(m_complexShapeAction, SIGNAL(triggered()), this, SLOT(complexShapeToolBar()));

	m_showGrid = new QAction(tr("&Show"), this);
	m_showGrid->setShortcut(tr("Ctrl+Alt+S"));
	connect(m_showGrid, SIGNAL(triggered()), this, SLOT(drawGrid()));

	m_aboutAction = new QAction(tr("About"), this);
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	m_aboutQtAction = new QAction(tr("About &Qt"), this);
	m_aboutQtAction->setShortcut(tr("Ctrl+Shift+Q"));
	connect(m_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	m_rectAction = new QAction(tr("Rect"), this);
	m_rectAction->setIcon(QIcon(":/images/rect.png"));
	connect(m_rectAction, SIGNAL(triggered()), this, SLOT(createRectShape()));

	m_circleAction = new QAction(tr("Circle"), this);
	m_circleAction->setIcon(QIcon(":/images/circle.png"));
	connect(m_circleAction, SIGNAL(triggered()), this, SLOT(createCircleShape()));

	m_triangleAction = new QAction(tr("Triangle"), this);
	m_triangleAction->setIcon(QIcon(":/images/triangle.png"));
	connect(m_triangleAction, SIGNAL(triggered()), this, SLOT(createTriangleShape()));
}

void view::createMenu()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_fileMenu->addAction(m_newAction);
	m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_exitAction);

	m_editMenu = menuBar()->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_groupAction);
	m_editMenu->addAction(m_splitAction);
	m_editMenu->addAction(m_deleteAction);

	m_insertMenu = menuBar()->addMenu(tr("&Insert"));
	m_insertMenu->addAction(m_basicShapeAction);
	m_insertMenu->addAction(m_complexShapeAction);

	m_optionMenu = menuBar()->addMenu(tr("&Option"));
	m_optionMenu->addAction(m_showGrid);

	m_helpMenu = menuBar()->addMenu(tr("&Help"));
	m_helpMenu->addAction(m_aboutAction);
	m_helpMenu->addAction(m_aboutQtAction);
}

void view::createToolBar()
{
	m_basicShapeToolBar = addToolBar(tr("&Basic"));
	m_basicShapeToolBar->addAction(m_rectAction);
	m_basicShapeToolBar->addAction(m_circleAction);
	m_basicShapeToolBar->addAction(m_triangleAction);
	m_basicShapeToolBar->setVisible(false);

	m_complexShapeToolBar = addToolBar(tr("&Complex"));
	m_complexShapeToolBar->setVisible(false);

}

void view::createContextMenu()
{
	m_mainWidget->addAction(m_groupAction);
	m_mainWidget->addAction(m_splitAction);
	m_mainWidget->addAction(m_deleteAction);
	m_mainWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void view::createRectShape()
{
	m_mainWidget->curState = DrawGrah::DRAWING;
	m_mainWidget->m_shapeType = tree::RECT;
}

void view::createCircleShape()
{
	m_mainWidget->curState = DrawGrah::DRAWING;
	m_mainWidget->m_shapeType = tree::CIRCLE;
}

void view::createTriangleShape()
{
	m_mainWidget->curState = DrawGrah::DRAWING;
	m_mainWidget->m_shapeType = tree::TRIANGLE;
}

void view::newFile()
{

}

void view::createGroupShape()
{
	m_mainWidget->groupShape();
	update();
}

void view::splitGroupShape()
{
	m_mainWidget->splitGroup();
	update();
}

void view::deleteShape()
{
	m_mainWidget->deleteSelectedShape();
	update();
}

void view::basicShapeToolBar()
{
	m_complexShapeToolBar->setVisible(false);
	m_basicShapeToolBar->setVisible(true);
}

void view::complexShapeToolBar()
{	
	m_basicShapeToolBar->setVisible(false);

	//由于没有实现画复杂的图形，估复杂图形工具条暂时不让其显示
	m_complexShapeToolBar->setVisible(false);

}

void view::drawGrid()
{


}

void view::about()
{
	QMessageBox::about(this, tr("About Spreadsheet"),
		tr("<h2>This is A Simple test</h2>"));
}

#define SAFE_DELETE(p) { \
	if (p) { \
		delete p; p = NULL; \
	} \
}

view::~view()
{
	SAFE_DELETE(m_mainWidget);
	SAFE_DELETE(m_fileMenu);
	SAFE_DELETE(m_editMenu);
	SAFE_DELETE(m_insertMenu);
	SAFE_DELETE(m_optionMenu);
	SAFE_DELETE(m_helpMenu);
	SAFE_DELETE(m_basicShapeToolBar);
	SAFE_DELETE(m_complexShapeToolBar);
	SAFE_DELETE(m_newAction);
	SAFE_DELETE(m_exitAction);
	SAFE_DELETE(m_groupAction);
	SAFE_DELETE(m_splitAction);
	SAFE_DELETE(m_deleteAction);
	SAFE_DELETE(m_basicShapeAction);
	SAFE_DELETE(m_complexShapeAction);
	SAFE_DELETE(m_showGrid);
	SAFE_DELETE(m_aboutAction);
	SAFE_DELETE(m_aboutQtAction);
}	
	
	