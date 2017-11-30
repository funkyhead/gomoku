#pragma once

#ifndef _CMDMGR_H_
#define _CMDMGR_H_

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <map>
#include <exception>
#include <vector>
#include <algorithm>
//#include <math.h>
#include <chrono>

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



struct Info
{
	int size;// 20 for a map of 20 on 20
	int timeout;// timeout

	Info() {
		size = 20;
	}
	//put all the other info value
};

class CmdMgr {
	//private var
private:
	//temporary var
	Info				info;
	std::vector<Pos>	positions;
	std::ofstream		log;
	//end


	//public member
public:

	CmdMgr();
	~CmdMgr();

	CmdMgr(int i) {
		log.open("C:/Users/remi/Desktop/ia_log.txt");
	}


	void	parseCmd(std::string);
	//private member
private:

	std::vector<std::string>	split(const std::string & str, char sep);

	void checkCmd(const std::string &);
	void checkCmdWithPos(const std::string &, const std::string &);
	void checkAttr(const std::string &);
	bool isPosition(const std::string &);
	bool isBoarMove(const std::string &);
	bool isNum(const std::string & str);
};

#endif // !_CMDMGR_H_
