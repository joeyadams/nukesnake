#include <QApplication>
#include <QWidget>
#include <time.h>
#include <stdlib.h>
#include "../engine/game.h"

//the gameplay area
//note that there can only be one instance of QNukeSnake at a time; the game engine is non-reentrant.
class QNukeSnake : public QWidget
{
	//Q_OBJECT
	
public:
	QNukeSnake(QWidget *parent = 0);
	~QNukeSnake();

protected:
	void paintEvent(QPaintEvent *event);

//private:
	
};

QNukeSnake::QNukeSnake(QWidget *parent)
	: QWidget(parent)
{
	resize(640, 480);
	NS_init();
	NS_newgame(width()/16, height()/16, C_NewGame); //start with a 2 player game
}

void QNukeSnake::paintEvent(QPaintEvent *event)
{
	
}

QNukeSnake::~QNukeSnake()
{
	NS_uninit();
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	QApplication app(argc, argv);
	QNukeSnake qnukesnake;
	qnukesnake.show();
	return app.exec();
}
