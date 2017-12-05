// gomoku.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "CmdMgr.h"
#include "Root.h"


//*
int main()
{

	CmdMgr	cmdMgr;
	
	while (1) {

		std::string tmp;
		std::getline(std::cin, tmp);

		try {
			cmdMgr.parseCmd(tmp);
		}
		catch (const std::exception & e)
		{
			//std::cout << "ERROR " << e.what;
		}
		
	}
}
//*/
//-------------- test one play on root
/*
int main() {
	Root root;
	//root.begin();
	std::cout << "playing" << std::endl;
	Sleep(1000);
	Node *tmpNode = new Node(Move(Pos(0, 0), Color::BLACK), 0, NULL);
	//tmpNode =
	root.childs.push_back(tmpNode);
	
	std::cout << "playing" << std::endl;
	Sleep(1000);
	tmpNode->childs.push_back(new Node(Move(Pos(0, 0), Color((tmpNode->move.color == Color::WHITE) + 1)), tmpNode->depth + 1, tmpNode));
	tmpNode = tmpNode->childs[0];


	std::cout << "playing" << std::endl;
	Sleep(1000);
	tmpNode->childs.push_back(new Node(Move(Pos(0, 1), Color((tmpNode->move.color == Color::WHITE) + 1)), tmpNode->depth + 1, tmpNode));
	tmpNode = tmpNode->childs[0];


	std::cout << "playing" << std::endl;
	Sleep(1000);
	tmpNode->childs.push_back(new Node(Move(Pos(1, 0), Color((tmpNode->move.color == Color::WHITE) + 1)), tmpNode->depth + 1, tmpNode));
	tmpNode = tmpNode->childs[0];


	std::cout << "playing" << std::endl;
	Sleep(1000);
	tmpNode->childs.push_back(new Node(Move(Pos(0, 2), Color((tmpNode->move.color == Color::WHITE) + 1)), tmpNode->depth + 1, tmpNode));
	tmpNode = tmpNode->childs[0];


	std::cout << "playing" << std::endl;
	Sleep(1000);
	tmpNode->childs.push_back(new Node(Move(Pos(2, 0), Color((tmpNode->move.color == Color::WHITE) + 1)), tmpNode->depth + 1, tmpNode));
	tmpNode = tmpNode->childs[0];


	std::cout << "will simulate" << std::endl;
	Sleep(1000);
	root.simulate(tmpNode);
	//root.logNode();
}
/*
/// ---------------- Clock Struct usage example

int main() {
	Clock timer;
	while (1) {
		timer.startClock(5.00);
		while (!timer.timeOut()) {

			std::cout << timer.getRunTime().count() << std::endl;
		}
	}
	return 0;
}
*/