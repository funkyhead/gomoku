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

/*
//-------------- test one play on root
int main() {
Root root;

root.play(Pos(10, 10));

Sleep(10000);
return 0;
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