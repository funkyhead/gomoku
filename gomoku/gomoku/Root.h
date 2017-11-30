#pragma once
#ifndef _ROOT_H_
#define	_ROOT_H_

#include "CmdMgr.h"

#define NB_NODE_SEARCH 3
#define EXPLORATION_CONST 1.44

struct Node
{
	std::vector<Node> childs;
	int		nbTry;
	int		nbWin;
	bool	color;
	Pos		position;

	Node() {
		nbTry = 0;
		nbWin = 0;
	}

	Node(bool _color) {
		nbTry = 0;
		nbWin = 0;
		color = _color;
	}

	void addNode(Node node) {
		childs.push_back(node);
	}
};

struct Clock
{
	std::chrono::high_resolution_clock::time_point then;
	std::chrono::duration<double> timeToSpend;

	Clock(){}

	void startClock(double _timeToSpend) {
		then = std::chrono::high_resolution_clock::now();
		timeToSpend = std::chrono::duration<double>(_timeToSpend);
	}

	std::chrono::duration<double> getRunTime() {
		return (std::chrono::high_resolution_clock::now() - then);
	}

	bool	timeOut() {
		return (std::chrono::high_resolution_clock::now() - then >= timeToSpend);
	}
};

class Root
{
private:
	std::vector<Node>	rootsNode;
	int					nbSimulation;
	
public:

	Root();
	Root(const std::string &);
	~Root();

	Pos	play(double runTime);

private:

	void select();
	void expand();
	bool simulate();
	void update();
};

#endif // !_ROOT_H_
