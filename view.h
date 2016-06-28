#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>

class DrawGrah;

class view : public QMainWindow
{
	Q_OBJECT

public:
	view();
	~view();

	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
 	void mouseMoveEvent(QMouseEvent *event);
 	void mouseReleaseEvent(QMouseEvent *event);
 	void keyPressEvent(QKeyEvent *event);
 	void keyReleaseEvent(QKeyEvent *event);

private slots:
	void newFile();
	void createGroupShape();
	void splitGroupShape();
	void deleteShape();
	void basicShapeToolBar();
	void complexShapeToolBar();
	void drawGrid();
	void about();
	void createRectShape();
	void createCircleShape();
	void createTriangleShape();

private:
	void createAction();
	void createMenu();
	void createContextMenu();
	void createToolBar();
	

	DrawGrah *m_mainWidget;

	QMenu *m_fileMenu;
	QMenu *m_editMenu;
	QMenu *m_insertMenu;
	QMenu *m_optionMenu;
	QMenu *m_helpMenu;
	QToolBar *m_basicShapeToolBar;
	QToolBar *m_complexShapeToolBar;
	QAction *m_newAction;
	QAction *m_exitAction;
	QAction *m_groupAction;
	QAction *m_splitAction;
	QAction *m_deleteAction;
	QAction *m_basicShapeAction;
	QAction *m_complexShapeAction;
	QAction *m_showGrid;
	QAction *m_aboutAction;
	QAction *m_aboutQtAction;
	QAction *m_rectAction;
	QAction *m_circleAction;
	QAction *m_triangleAction;

};

#endif