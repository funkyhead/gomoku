#pragma once
#ifndef _ROOT_H_
#define	_ROOT_H_

#include "Includes.h"

#define NB_NODE_SEARCH 3
#define EXPLORATION_CONST 1.44
#define LOGFILE "C:/Users/remi/Desktop/ia_gomoku/board_log.txt"

class Root : public Node
{
private:
	double					nbSimulation;
	Color				color;
	Info				info;
	Node				*currentNode;
	Clock				timer;
	
public:

	Root();
	Root(const std::string &);
	~Root();

/*
	friend std::ostream& operator<<(std::ostream& os, const Root & root) {
		os << root.rootsNode << std::endl;
		os << root.nbSimulation << std::endl;
		os << root.color;
	}
	
	friend std::ostream& operator>>(std::istream& is, const Root & root) {
		is << root.rootsNode << root.nbSimulation << root.color;
	}*/

	Pos	play(Pos);


private:


	//runing Members for play
	void  select(Node *);
	Node *expand(Node*);
	void printBoard(Color ** board);
	void simulate(Node *);
	void update(Node *,bool);


	//utils for selection
	double getNbSimulationForDepth(Node * node, int depth);
	void logToFile(Node * node);
	Node * selectOneNode(Node * node);
	int setBoard(Color ** color, Node * node);


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
