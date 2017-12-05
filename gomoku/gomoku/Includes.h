#pragma once

#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "Includes.h"
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <map>
#include <exception>
#include <vector>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <random>


//----------- INFO: USE TO STORE AND ACCESS INFORMATION ABOUT GAME RULES SUCH AS BOARD SIZE OR AMOUNT OF TIME TO PLAY A TURN
struct Info
{
	int size;// 20 for a map of 20 on 20
	double timeout;// timeout

	Info() {
		size = 20;
	}
	//put all the other info value
};

//----------- COLOR: ENUM WICH BIND BLACK PLAYER PIONS, WHITE PLAYER PIONS OR NONE
enum class Color
{
	NONE,
	WHITE,
	BLACK
};

enum class Result
{
	NONE,
	WIN,
	LOOSE
};



//----------- POSITION: USE TO STORE POSITION ON THE BOARD
struct Pos
{
	int x;
	int y;

	Pos(int _x, int _y) {
		x = _x;
		y = _y;
	}

	Pos() {
		x = 0;
		y = 0;
	}
};

static bool operator==(Pos a, Pos b) {
	if (a.x == b.x && a.y == b.y)
		return true;
	return false;
}

//----------- MOVE: USE TO DESCRIBE A PLAY AS A POSITION AND A PLAYER COLOR  
struct Move
{
	Pos pos;
	Color color;

	Move() {};

	Move(Pos _pos, Color _color) {
		color = _color;
		pos = _pos;
	}
};


//----------- NODE: USE TO CREATE THE THREE NODE FOR THE MONTE-CARLO THREE SEARCH
struct Node
{
	std::vector<Node *> childs;
	Node				*father;

	double		nbTry;
	double		nbWin;
	int		depth;
	Move	move;
	Result	res;

	Node() {
		father = NULL;
		nbTry = 0;
		nbWin = 0;
		depth = 0;
		res = Result::NONE;
	}

	Node(Move _move, int _depth, Node * _father) {
		father = _father;
		nbTry = 0;
		nbWin = 0;
		depth = _depth;
		move = _move;
		res = Result::NONE;
	}

	void addNode(Node *node) {
		childs.push_back(node);
	}
};

//----------- CLOCK: USE TO MEASURE DURATION, WORK AS A TIMER
struct Clock
{
	std::chrono::high_resolution_clock::time_point then;
	std::chrono::duration<double> timeToSpend;

	Clock() {}

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

#endif // !_INCLUDES_H_
