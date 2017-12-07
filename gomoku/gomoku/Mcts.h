#pragma once

#ifndef _MCTS_H_
#define	_MCTS_H_

#include "Includes.h"
#include "ThreadPool.h"

class Mcts
{
private:
	Color				color;
	Info				info;
	Node				*rootNode;
	Node				*currentNode;
	Clock				timer;
	Color				**virtualBoard;
	int					expandRange;

public:
	Mcts();
	~Mcts();

	Pos			play(Pos);
	Pos			begin();
	void		simulate(Node *);
	bool		testBoard();
	void		restart();

private:

	void		select(Node *);
	void		selectWithLimitedRange(Node *, int minX, int maxX, int minY, int maxY);
	void		expand(Node*);
	void		expandWithLimitedRange(Node*, int minX, int maxX, int minY, int maxY);
	void		printBoard(Color ** board);
	void		update(Node *, double);


	double		getNbSimulationForDepth(Node * node, int depth);
	Node *		getNextMove(Node * node);
	Result		expectedOutput(Node * node);
	Node *		selectBestChild(Node * node);
	Node *		selectOneNodeWithMctsAlgo(Node * node);
	void		setBoard(Color ** color, Node * node);						// ERIC, PLEASE FINISH THIS TODO
	Color		**createEmptyBoard(int size);

	bool		isFivePositionAligned(Color **& board, Pos currentPos, Color color);
	bool		VerticalAlign(Color **& board, Pos & currentPos, Color color);
	bool		HorizontalAlign(Color **& board, Pos currentPos, Color color);
	bool		RightDiagonalAlign(Color **& board, Pos currentPos, Color color);
	bool		LeftDiagonalAlign(Color **& board, Pos currentPos, Color color);

	void		deleteNodes(Node *node);
	Pos			getRandomPos(int boardSize);
};

#endif