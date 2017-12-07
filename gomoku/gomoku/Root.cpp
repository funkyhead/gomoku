#include "stdafx.h"
#include "Root.h"


#define LOG3(X, Y, Z) std::cout << X  << Y << Z << std::endl;
#define LOG2(X, Y) std::cout << X  << Y << std::endl;
#define LOG(X) std::cout << X << std::endl;


Root::Root()
{
	father = NULL;
	currentNode = this;
	color = Color::WHITE;
	info.timeout = 5;
	info.boardSize = 19;
	depth = 0;
	logFile = std::ofstream(LOGFILE, std::ofstream::app);
	expandRange = 3;
	pool = new ThreadPool(std::thread::hardware_concurrency());

	virtualBoard = new Color*[info.boardSize];
	for (int i = 0; i < info.boardSize; i++) {
		virtualBoard[i] = new Color[info.boardSize];
		for (int j = 0; j < info.boardSize; j++) {
			virtualBoard[i][j] = Color::NONE;
		}
	}
	//std::cout << "board has been created" << std::endl;
}

Root::Root(const std::string &) : Root()
{

}

Root::~Root()
{
	while (!(this)->childs.empty()) {
		deleteNodes((this)->childs[0]);
	}
	logFile.close();
}

void Root::logNode() {
	Node *node = currentNode;
	logFile << "nb simu !: " << nbTries << std::endl;
	for (std::vector<Node *>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		//if ((*it)->res != Result::NONE)
		logToFile(*it);
	}
}

Pos Root::begin() {
	timer.startClock(info.timeout);
	//Set the currentNode to competitor lastPlay

	if (this->childs.empty()) {

		currentNode = new Node(Move(Pos(info.boardSize / 2, info.boardSize / 2), color), this);
		this->childs.push_back(currentNode);
	}
	else {

		currentNode = selectBestChild(this);
	}
	while (!timer.timeOut()) {
		select(currentNode);
		//logFile << timer.getRunTime().count();
	}
	return currentNode->move.pos;
}
 

Pos Root::play(Pos competitorLastPlay)
{
	if (competitorLastPlay.x == 0 && competitorLastPlay.y == 0)
		logNode();
	//launch timer
	timer.startClock(info.timeout);


	//Set the currentNode to competitor lastPlay  
	Node *competitorNode = NULL;

	for (std::vector<Node*>::iterator it = currentNode->childs.begin(); it != currentNode->childs.end(); it ++) {
		if ((*it)->move.pos == competitorLastPlay)
		{
			competitorNode = (*it);
			break;
		}
	}

	//if no actual node match his play create a new one;
	if (competitorNode == NULL) {
		competitorNode = new Node( Move(competitorLastPlay, Color((color == Color::WHITE) + 1)), currentNode);
		currentNode->childs.push_back(competitorNode);
	}
	//------------------------------------------------	

	//-------------------------------------------------
	setBoard(virtualBoard, competitorNode);
//	printBoard(virtualBoard);
	//std::cout << "befor main loop" << std::endl;
	//main loop : Selection->Selection->...-p!oiexpand->simulate->update;

	//expandRange = 1;
	while (!timer.timeOut()) 
	{
		select(competitorNode);
		//selectWithLimitedRange(competitorNode, minX,maxX, minY, maxY);
	}

	//get the Selection for currentDepth, and set the current Node.

	//return (currentNode = selectOneNode(competitorNode))->move.pos;//TODO: do better selection node for the plays
	currentNode = getNextMove(competitorNode);
	setBoard(virtualBoard, currentNode);
	//printBoard(virtualBoard);
	//setBoard(virtualBoard, currentNode);
	//if (isFivePositionAligned(virtualBoard, Pos(currentNode->move.pos.y, currentNode->move.pos.x), currentNode->move.color)) {
		//logFile << "Won" << std::endl;
//	}
	//-------------------------------------------------
	
	return (currentNode)->move.pos;
}




double Root::getNbSimulationForDepth(Node *node, int depth) {
	if (node->depth == depth)
		return node->nbTries;
	else if (node->childs.empty())
		return 0;
	double nbSimulation = 0;
	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		nbSimulation += getNbSimulationForDepth((*it), depth);
	}
	return nbSimulation;
}


void Root::logToFile(Node *node) {
	Color **board = virtualBoard;
	setBoard(virtualBoard, node);

	char player = 'X';
	if (node->move.color == Color::WHITE)
		player = 'O';
	logFile << " PLAYER[ "<<  player << " ]  WIN/PLAY[ " << node->nbWins / node->nbTries << " ]  MOVE[" << (int)(node->move.pos.x) << ", "<< (int)(node->move.pos.y) << "]" << std::endl;
	logFile << " WIN[ " << (int) node->res<< " ]" << std::endl;
	for (int i = 0; i < info.boardSize; i++) {
		for (int j = 0; j < info.boardSize; j++) {
			if (board[i][j] == Color::WHITE)
				logFile << "O";
			else if (board[i][j] == Color::BLACK)
				logFile << "X";
			else
				logFile << "_";
			logFile << " ";
		}
		logFile << std::endl;
	}
	logFile << std::endl;
}



Node *Root::getNextMove(Node *node) {
	Node *selectedNode = NULL;
	double	maxSelectionValue = -1;


	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		//setBoard(virtualBoard, *it);
		Node *child = selectBestChild(*it);

		if ((child->nbWins / child->nbTries) == 1)
			logFile << "find a winning play for the opponent" << std::endl;
		double selectionValue = ((*it)->nbWins / (*it)->nbTries) - (child->nbWins / child->nbTries);


		if (selectionValue >= maxSelectionValue)
		{
			//if (expectedOutput(*it) != Result::LOOSE)
			//{
				maxSelectionValue = selectionValue;
				selectedNode = (*it);
			//}
		}
	}
	return selectedNode;
}


Result	Root::expectedOutput(Node *node){
	if (node->res != Result::NONE || node->childs.empty()) {
		//if (node->res == Result::LOOSE)
		//std::cout << "I'm not fucking blind FUUUUCK" << std::endl;
		return node->res;
	}
	return expectedOutput(selectBestChild(node));
}

Node *Root::selectBestChild(Node *node) {

	//std::cout << "running mcts select with : " << node->childs.size() << " childs" << std::endl;

	Node *selectedNode = node;
	double	maxSelectionValue = 0;

	//double nbSimulationForDepth = getNbSimulationForDepth(this, node->depth + 1);
	//std::cout << "nb simulation for depth : " << nbSimulationForDepth << std::endl;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		setBoard(virtualBoard, *it);
		if ((*it)->res == Result::WIN)
			return *it;
		double selectionValue = ((*it)->nbWins / (*it)->nbTries);


		if (selectionValue >= maxSelectionValue) {
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}

Node *Root::selectOneNodeWithMctsAlgo(Node *node) {

	//std::cout << "running mcts select with : "<< node->childs.size() << " childs" << std::endl;


	Node *selectedNode = NULL;
	double	maxSelectionValue = 0;

	//double nbSimulationForDepth = getNbSimulationForDepth(this, node->depth + 1);
	double nbSimulationForDepth = 0;
	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		nbSimulationForDepth += (*it)->nbTries;
	}
	//std::cout << "nb simulation for depth : " << nbSimulationForDepth << std::endl;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {

		double selectionValue = (  (*it)->nbWins / (*it)->nbTries)   + (EXPLORATION_CONST  *  sqrtf(  log(nbSimulationForDepth / (*it)->nbTries) ) );
		//if (isFivePositionAligned())

		if (selectionValue >= maxSelectionValue){//&& (*it)->res == Result::NONE) {
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}


void Root::restart() {
	//logToFile(currentNode);
	currentNode = this;
}

void  Root::select(Node *node)
{
	//std::cout << "Begin SELECT" << std::endl;

	//std::cout << "select lvl : " << node->depth << std::endl;

	if (node == NULL || node->depth == info.boardSize * info.boardSize)
		return;
	if (node->childs.empty())//if The current node as not been expand previously expand it now
		expand(node);
	else//else select the new node to explore with MCTS algorithm
	{
		select(selectOneNodeWithMctsAlgo(node));
	}
	//std::cout << "end of select" << std::endl;
	//std::cout << "END SELECT" << std::endl;
}

void Root::selectWithLimitedRange(Node * node, int minX, int maxX, int minY, int maxY) {

	if (node->depth == info.boardSize * info.boardSize || node == NULL)
		return;
	if (node->childs.empty())
	{//if The current node as not been expand previously expand it now
		if (node->move.pos.x >= minX && node->move.pos.x <= maxX && node->move.pos.y >= minY && node->move.pos.y <= maxY)
			expandWithLimitedRange(node, minX, maxX, minY, maxY);
		else
			expand(node);
	}
	else//else select the new node to explore with MCTS algorithm
	{
		if (node->move.pos.x >= minX && node->move.pos.x <= maxX && node->move.pos.y >= minY && node->move.pos.y <= maxY)
			selectWithLimitedRange(selectOneNodeWithMctsAlgo(node), minX, maxX, minY, maxY);
		else
			select(selectOneNodeWithMctsAlgo(node));
	}

}


void Root::expand(Node*node)
{
//	std::cout << "expand" << std::endl;
	

	//code repetition in simulate -- look for possible patch
	Color **board = new Color*[info.boardSize];
	for (int i = 0; i < info.boardSize; i++) {
		board[i] = new Color[info.boardSize];
		for (int j = 0; j < info.boardSize; j++) {
			board[i][j] = Color::NONE;
		}
	}

	setBoard(board, node);
	//std::cout << "print in expand" << std::endl;
	//printBoard(board);
	RestrainedPos rpos = getRestrainedPos(board, 3);

	//std::cout << "minY = " << minY << " maxY = " << maxY << " minX = " << minX << " maxX = " << maxX << std::endl;

	for (int i = rpos.max.y - 1; i >= rpos.min.y; i --) {
		for (int j = rpos.max.x - 1; j >= rpos.min.x; j --) {
	//for (int i = 0; i < info.boardSize; i++) {
		//for (int j = 0; j < info.boardSize; j++) {
			if (board[i][j] == Color::NONE) {
				Node *newChild = new Node( Move( Pos(j, i), Color((node->move.color == Color::WHITE) + 1 )), node);
				node->childs.push_back(newChild);
				if (node->res == Result::LOOSE)
				{
					newChild->res = Result::LOOSE;
					update(newChild, node->move.color == color);
				}
				else if (node->res == Result::WIN)
				{
					newChild->res = Result::WIN;
					update(newChild, node->move.color != color);
				}
				else
					simulate(newChild);
					//pool->enqueue(&Root::simulate, this, newChild);
			}
		}
	}
	//std::cout << "nb child : " << node->childs.size() <<  std::endl;
	//Sleep(10000);
	deleteBoard(board);
}

void Root::expandWithLimitedRange(Node * node, int minX, int maxX, int minY, int maxY) {
	Color **board = new Color*[info.boardSize];
	for (int i = 0; i < info.boardSize; i++) {
		board[i] = new Color[info.boardSize];
		for (int j = 0; j < info.boardSize; j++) {
			board[i][j] = Color::NONE;
		}
	}

	setBoard(board, node);
	//std::cout << "print in expand" << std::endl;
	//printBoard(board);


	for (int i = minX; i < maxX; i ++) {
	for (int j = minY; j < maxY; j ++) {
			if (board[i][j] == Color::NONE) {
				Node *newChild = new Node(Move(Pos(j, i), Color((node->move.color == Color::WHITE) + 1)), node);
				node->childs.push_back(newChild);
				if (node->res == Result::LOOSE)
				{
					newChild->res = Result::LOOSE;
					update(newChild, node->move.color == color);
				}
				else if (node->res == Result::WIN)
				{
					newChild->res = Result::WIN;
					update(newChild, node->move.color != color);
				}
				else
					simulate(newChild);
			}
		}
	}
	//std::cout << "nb child : " << node->childs.size() <<  std::endl;
	//Sleep(10000);
}

void Root::printBoard(Color **board) {

	for (int i = 0; i < info.boardSize; i++) {
		for (int j = 0; j < info.boardSize; j++) {
			if (board[i][j] == Color::WHITE)
				std::cout << "O";
			else if (board[i][j] == Color::BLACK)
				std::cout << "X";
			else
				std::cout << "_";
			std::cout << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << std::endl;
}




void	Root::setBoard(Color **color, Node *node) {//TODO: SHOULD use currentBoard to construct, only need to expand currentboard, use virtual board to avoid creating/deleting
//	std::cout << "set board" << std::endl;
	for (int i = 0; i < info.boardSize; i++) {
		for (int j = 0; j < info.boardSize; j++) {
			color[i][j] = Color::NONE;
		}
	}
	while (node->father != NULL) {
		color[node->move.pos.y][node->move.pos.x] = node->move.color;
		node = node->father;
	}
}

RestrainedPos Root::getRestrainedPos(Color ** board, int range)
{
	RestrainedPos pos;

	int minY = info.boardSize - 1;
	int minX = info.boardSize - 1;
	int maxY = 0;
	int maxX = 0;

	for (int x = 0; x < info.boardSize; x++)
	{
		for (int y = 0; y < info.boardSize; y++)
		{
			if (board[y][x] != Color::NONE)
			{
				minY = minY > y ? y : minY;
				minX = minX > x ? x : minX;
				maxY = maxY < y ? y : maxY;
				maxX = maxX < x ? x : maxX;
			}
		}
	}
	minY -= range;
	minX -= range;
	maxY += range;
	maxX += range;
	minY = minY < 0 ? 0 : minY;
	minX = minX < 0 ? 0 : minX;
	maxY = maxY > info.boardSize - 1 ? info.boardSize - 1 : maxY;
	maxX = maxX > info.boardSize - 1 ? info.boardSize - 1 : maxX;

	if (minX >= maxX || minY >= maxY) {
		std::cout << "OUPS : " << std::endl;
		std::cout << "minY = " << minY << " maxY = " << maxY << " minX = " << minX << " maxX = " << maxX << std::endl;
		Sleep(100000);
		exit(0);
	}
	pos.min.x = minX;
	pos.min.y = minY;
	pos.max.x = maxX;
	pos.max.y = maxY;
	return (pos);
}

void	Root::deleteBoard(Color **board)
{
	for (int i = info.boardSize - 1; i >= 0; i--) {
		delete board[i];
	}
	delete board;
}

void Root::simulate(Node *node)
{
	//std::cout << "simulate" << std::endl;

	//Creating a board with actual depth play
	Color **board = new Color*[info.boardSize];
	for (int i = 0; i < info.boardSize; i++) {
		board[i] = new Color[info.boardSize];
		for (int j = 0; j < info.boardSize; j++) {
			board[i][j] = Color::NONE;
		}
	}

	//TODO SHOULD OPTI BOARD
	mtx.lock();
	setBoard(board, node);
	mtx.unlock();
	int nbPlay = node->depth;

	//set the ref on play
	Color	currentPlayer = node->move.color;

	//set the ref
	Pos		lastPlay = node->move.pos;

	//set the player changeManagement
	int		sign = 1;//work as a switch between player wanted to name it switch but it's not possible

	if (currentPlayer == Color::BLACK)
		sign = -1;


	int totalNbPlays = info.boardSize * info.boardSize;

	if (isFivePositionAligned(board, Pos(lastPlay.y, lastPlay.x), currentPlayer)) {
		node->res = Result(2 - (currentPlayer == color));

		//update(node, 1);
		//currentNode = node;
		//return;
	}
	//printBoard(board);
	while (!isFivePositionAligned(board, Pos(lastPlay.y, lastPlay.x), currentPlayer) && nbPlay < totalNbPlays) {
		//Switch currentPlayer
		//std::cout << "simulation loop , nb play : " << nbPlay << "  totlanbplay :" <<  totalNbPlays << std::endl;

		currentPlayer = Color((int)currentPlayer + sign);

		//int r = 1;
		//Get a valid Play
		//int i = 0;
		//lastPlay = randomPos(info.boardSize, getRestrainedPos(board,r));
		lastPlay = randomPos(info.boardSize);

		while (board[lastPlay.y][lastPlay.x] != Color::NONE)
		{
			/*if (i >= r * r)
			{
				r++;
				i = 0;
			}*/
			//lastPlay = randomPos(info.boardSize, getRestrainedPos(board, r));
			lastPlay = randomPos(info.boardSize);
			//i++;
		}
		board[lastPlay.y][lastPlay.x] = currentPlayer;

		sign *= -1;

		nbPlay += 1;

	}

	//std::cout << "end simulate" << std::endl;

	mtx.lock();
	if (nbPlay == totalNbPlays)//see If no update is better
		update(node, 0.5, nbPlay);
	else
		update(node, currentPlayer == node->move.color, nbPlay);
	mtx.unlock();

	deleteBoard(board);
	//std::cout << " END simulate" << std::endl;

	//std::cout << "end of update" << std::endl;
}

void Root::update(Node * node, double win)
{
	//	std::cout << "update" << std::endl;
	double weight;

	weight = 1;
	//std::cout << weight << std::endl;
	//Sleep(1000);
	while (node != NULL) {
		node->nbTries += 1 * weight;
		node->nbWins += win * weight;
		node = node->father;
		if (win == 1) {
			win = 0;
			//std::cout << "change win" << std::endl;
		}
		else if (win == 0)
			win = 1;
	}
}


void Root::update(Node * node, double win, double nbplay)
{
//	std::cout << "update" << std::endl;
	double weight;

	weight = 1/(nbplay + 1 - (double)(node->depth));
	//weight = 1;

	//std::cout << weight << std::endl;
	//Sleep(1000);
	while (node != NULL) {
		node->nbTries += 1 *weight;
		node->nbWins += win *weight;
		node = node->father;
		if (win == 1) {
			win = 0;
			//std::cout << "change win" << std::endl;
		}
		else if (win == 0)
			win = 1;
	}
}




//UTILS TO

bool	Root::isFivePositionAligned(Color **& board, Pos currentPos, Color color) {	
	return (VerticalAlign(board, currentPos, color) ||
		HorizontalAlign(board, currentPos, color) ||
		RightDiagonalAlign(board, currentPos, color) ||
		LeftDiagonalAlign(board, currentPos, color));
}

bool	Root::VerticalAlign(Color **& board, Pos &currentPos, Color color) {
	int max = 1;
	int x = currentPos.x;

	for (int y = currentPos.y + 1; y < info.boardSize && board[x][y] == color ; y++)
	{
		max++;
	}
	for (int y = currentPos.y - 1; y >= 0 && board[x][y] == color; y--)
	{
		max++;
	}

	if (max >= MAX)
		return true;
	return false;
}

bool	Root::HorizontalAlign(Color **& board, Pos currentPos, Color color) {
	int max = 1;
	int y = currentPos.y;

	for (int x = currentPos.x + 1; x < info.boardSize && board[x][y] == color; x++)
	{
		max++;
	}
	for (int x = currentPos.x - 1; x >= 0 && board[x][y] == color; x--)
	{
		max++;
	}

	if (max >= MAX)
		return true;
	return false;
}

bool Root::testBoard() {
	Color **board = new Color*[info.boardSize];
	for (int i = 0; i < info.boardSize; i++) {
		board[i] = new Color[info.boardSize];
		for (int j = 0; j < info.boardSize; j++) {
			if (i == 0)
				board[i][j] = Color::WHITE;
			else
				board[i][j] = Color::NONE;
		}
	}
	printBoard(board);
	//if (HorizontalAlign(board, Pos(0, 0), Color::WHITE))
		//return true;
	if (isFivePositionAligned(board, Pos(0, 0), Color::WHITE))
		return true;
	return false;
}

bool	Root::RightDiagonalAlign(Color **& board, Pos currentPos, Color color) {
	int max = 1;
	int y = currentPos.y - 1;
	int x = currentPos.x + 1;

	while (x < info.boardSize && y >= 0 && board[x][y] == color)
	{
		x++;
		y--;
		max++;
	}
	y = currentPos.y + 1;
	x = currentPos.x - 1;
	while (x >= 0 && y < info.boardSize && board[x][y] == color)
	{
		x--;
		y++;
		max++;
	}

	if (max >= MAX)
		return true;
	return false;
}

bool	Root::LeftDiagonalAlign(Color **& board, Pos currentPos, Color color) {
	int max = 1;
	int y = currentPos.y + 1;
	int x = currentPos.x + 1;

	while (x < info.boardSize && y < info.boardSize && board[x][y] == color)
	{
		x++;
		y++;
		max++;
	}
	y = currentPos.y - 1;
	x = currentPos.x - 1;
	while (x >= 0 && y >= 0 && board[x][y] == color)
	{
		x--;
		y--;
		max++;
	}

	if (max >= MAX)
		return true;
	return false;
}



//DEPRECATED

std::vector<Pos> Root::getAllPreviousSelfMove(Node *node)
{
	std::vector<Pos> positions;
	while (node != NULL) {
		positions.push_back(node->move.pos);
		node = node->father;
	}
	return	positions;
}

std::vector<Move> Root::getAllPreviousMove(Node *node)
{
	std::vector<Move> moves;
	while (node != NULL) {
		moves.push_back(node->move);
		node = node->father;
	}
	return	moves;
}
