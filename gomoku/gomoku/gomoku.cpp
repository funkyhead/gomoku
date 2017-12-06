// gomoku.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "CmdMgr.h"
#include "Root.h"


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
