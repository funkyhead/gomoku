#include "stdafx.h"
#include "CmdMgr.h"

CmdMgr::CmdMgr()
{
	log.open("C:/Users/remi/Desktop/ia_gomoku/piskvork/ia_log.txt");

}

CmdMgr::~CmdMgr()
{
	log.close();
}

void CmdMgr::parseCmd(std::string cmd)
{
	log << "parsecmd : " << cmd << std::endl;

	std::vector<std::string> splitCmd(split(cmd, ' '));

	if (splitCmd.size() == 1){
		checkCmd(splitCmd[0]);
	}
	else if (splitCmd.size() == 2) {
		checkCmdWithPos(splitCmd[0], splitCmd[1]);
	}
	else
	{
		throw std::invalid_argument("Invalid argument number : " +  std::string(" ") + cmd);
	}

}

std::vector<std::string>	CmdMgr::split(const std::string & str, char sep) {
	std::vector<std::string>	words;

	for (unsigned int p = 0, q = 0; p < str.size(); p = q) {
		unsigned int n = str.find(sep, p + 1) - q - (p != 0);
		q = str.find(sep, p + 1);
		words.push_back(str.substr(p + (p != 0), n));
	}
	return words;
}
 
void CmdMgr::checkCmd(const std::string &cmd)
{
	log << "checkCmd :" << cmd << std::endl;

	if (cmd == "DONE") {
		
	}
	else if (cmd == "END") {

	}
	else if (cmd == "BOARD") {

	}
	else if (cmd == "LOG") {
		//root.logNode();
		std::cout << info.boardSize << std::endl;
	}
	else if (cmd == "BEGIN") {
		Pos pos = mcts.begin();
		std::cout << std::to_string(pos.x) << "," << std::to_string(pos.y) << std::endl;
	}
	else if (cmd == "RESTART") {
		mcts.restart();
		std::cout << "OK" << std::endl;
	}
	else if (isPosition(cmd)) {

	}
	else {
		throw std::invalid_argument(cmd + std::string(" does not fit the Protocol"));
	}
}

Pos randomPos(int size) {
	int	x, y;

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, size); // guaranteed unbiased

	x = uni(rng);
	y = uni(rng);

	return Pos(x, y);
}



void CmdMgr::checkCmdWithPos(const  std::string & cmd, const std::string & attr)
{
	log << "check cmd with arg : " << cmd << " - - " << attr << std::endl;
	/*
	if (!isPosition(attr)) {
		std::invalid_argument(attr + std::string(" does not fit any position"));
	}*/
	 if (cmd == "START") {//temp for alpha test with random, pls bind in gomokuMaster class
		info.boardSize = atoi(attr.c_str());
		std::cout << "OK" << std::endl;
	}
	else if (cmd == "TURN") {//temp ---------------------- temp
		std::vector<std::string> args(split(attr, ','));
		Pos pos = Pos(atoi(args[0].c_str()), atoi(args[1].c_str()));
		positions.push_back(Pos(atoi(args[0].c_str()), atoi(args[1].c_str())));
		//Pos pos = randomPos(info.boardSize);
		/*
		while ( std::find(positions.begin(), positions.end(), pos) != positions.end())
			pos = randomPos(info.boardSize);
		positions.push_back(pos);*/
		pos = mcts.play(pos);
		std::cout << std::to_string(pos.x) << "," << std::to_string(pos.y) << std::endl;
	}
	else {
		throw std::invalid_argument(cmd + std::string(" does not fit the Protocol"));
	}
}

void CmdMgr::checkAttr(const std::string & attr)
{

}



bool CmdMgr::isPosition(const std::string & str)
{
	std::vector<std::string> vals(split(str, ','));
	if (vals.size() != 2) {
		//error throw
	}
	else if (isNum(vals[0]) && isNum(vals[1]))
		return true;
	else
		;//erro throw 
	return false;
}

bool CmdMgr::isBoarMove(const std::string & str)
{
	std::vector<std::string> vals(split(str, ','));
	if (vals.size() != 3) {
		//error throw
	}
	else if (isNum(vals[0]) && isNum(vals[1]) && isNum(vals[2]))
		return true;
	else
		;//erro throw 
	return false;
}

bool CmdMgr::isNum(const std::string & str)
{
	for (unsigned int i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}
