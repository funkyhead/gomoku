#pragma once
#ifndef _ROOT_H_
#define	_ROOT_H_

#include "Includes.h"

#define EXPLORATION_CONST 1.44
#define LOGFILE "C:/Users/remi/Desktop/ia_gomoku/board_log.txt"
#define MAX 5
class Root : public Node
{
private:
	//double					nbSimulation;
	Color				color;
	Info				info;
	Node				*currentNode;
	Node				*rootNode;
	Clock				timer;
	std::ofstream		logFile;
	Color				**virtualBoard;
	
public:

	Root();
	Root(const std::string &);
	~Root();

	/*
	friend std::ostream& operator<<(std::ostream& os, const Root & root) {
		//os << root.rootsNode << std::endl;
		os << root.color;
		os << root.childs;
		os << root.nbWin;
		os << root.nbTry;
	}
	
	friend std::ostream& operator>>(std::istream& is, const Root & root) {
		is << root.rootsNode << root.nbSimulation << root.color;
	}*/

	Pos	play(Pos);
	Pos begin();


	void simulate(Node *);

	void logNode();
	bool testBoard();
	void restart();

private:

	//runing Members for play
	void  select(Node *);
	void expand(Node*);
	void printBoard(Color ** board);
	void update(Node *, double);


	//utils for selection
	double getNbSimulationForDepth(Node * node, int depth);
	void logToFile(Node * node);
	Node * getNextMove(Node * node);
	Result expectedOutput(Node * node);
	Node * selectBestChild(Node * node);
	Node * selectOneNodeWithMctsAlgo(Node * node);
	void setBoard(Color ** color, Node * node);


	//utils to aknowlege winning play
	bool isFivePositionAligned(Color **& board, Pos currentPos, Color color);
	bool VerticalAlign(Color **& board, Pos & currentPos, Color color);
	bool HorizontalAlign(Color **& board, Pos currentPos, Color color);
	bool RightDiagonalAlign(Color **& board, Pos currentPos, Color color);
	bool LeftDiagonalAlign(Color **& board, Pos currentPos, Color color);



	//deprecated , not use anywhere
	std::vector<Pos>	getAllPreviousSelfMove(Node *node);
	std::vector<Move>	getAllPreviousMove(Node *node);




	void deleteNodes(Node *node) {
		while (!node->childs.empty()) {
			deleteNodes(node->childs[0]);
		}
		delete node;
	}

	Pos randomPos(int size) {
		int	x, y;
		x = rand() % size;
		y = rand() % size;

		return Pos(x, y);
	}
};

#endif // !_ROOT_H_
