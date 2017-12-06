#pragma once

#ifndef _CMDMGR_H_
#define _CMDMGR_H_

#include "Includes.h"
#include "Root.h"

class CmdMgr {
private:
	//temporary var
	Info				info;
	std::vector<Pos>	positions;
	std::ofstream		log;
	//Root				root;
	Root				mcts;

public:
	CmdMgr();
	~CmdMgr();
	CmdMgr(int i)
	{
		log.open("C:/Users/remi/Desktop/ia_log.txt");
	}
	void	parseCmd(std::string);

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
