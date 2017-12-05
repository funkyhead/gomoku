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
	info.timeout = 7;
	info.size = 8;
	depth = 0;
	logFile = std::ofstream(LOGFILE, std::ofstream::app);

	virtualBoard = new Color*[info.size];
	for (int i = 0; i < info.size; i++) {
		virtualBoard[i] = new Color[info.size];
		for (int j = 0; j < info.size; j++) {
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
	Node *node = currentNode->father;
	logFile << "nb childs !: " << node->childs.size() << std::endl;
	for (std::vector<Node *>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		//if ((*it)->res != Result::NONE)
		logToFile(*it);
	}
}

Pos Root::begin() {
	timer.startClock(info.timeout);
	//Set the currentNode to competitor lastPlay

	if (this->childs.empty()) {

		currentNode = new Node(Move(Pos(info.size / 2, info.size / 2), color), 1, this);
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
	if (competitorLastPlay.x == 0)
		logNode();
	//
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
		competitorNode = new Node( Move(competitorLastPlay, Color((color == Color::WHITE) + 1)), currentNode->depth + 1, currentNode);//TODO -- change constuctor and maybe erae depth
		currentNode->childs.push_back(competitorNode);
	}
	//------------------------------------------------	

	//-------------------------------------------------
	setBoard(virtualBoard, competitorNode);
//	printBoard(virtualBoard);
	//std::cout << "befor main loop" << std::endl;
	//main loop : Selection->Selection->...-p!oiexpand->simulate->update;
	while (!timer.timeOut()) {
		select(competitorNode);
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
		return node->nbTry;
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
	logFile << " PLAYER[ "<<  player << " ]  WIN/PLAY[ " << node->nbWin / node->nbTry << " ]  MOVE[" << node->move.pos.x << ", "<< move.pos.y << "]" << std::endl;
	logFile << " WIN[ " << (int) node->res<< " ]" << std::endl;
	for (int i = 0; i < info.size; i++) {
		for (int j = 0; j < info.size; j++) {
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
	double	maxSelectionValue = 0;


	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		setBoard(virtualBoard, *it);

		double selectionValue = ((*it)->nbWin / (*it)->nbTry);


		if (selectionValue >= maxSelectionValue && expectedOutput(*it) != Result::LOOSE) {
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}


Result	Root::expectedOutput(Node *node){
	if (node->res != Result::NONE || node->childs.empty()) {
		return node->res;
	}
	return expectedOutput(selectBestChild(node));
}

Node *Root::selectBestChild(Node *node) {

	//std::cout << "running mcts select with : " << node->childs.size() << " childs" << std::endl;

	Node *selectedNode = NULL;
	double	maxSelectionValue = 0;

	//double nbSimulationForDepth = getNbSimulationForDepth(this, node->depth + 1);
	//std::cout << "nb simulation for depth : " << nbSimulationForDepth << std::endl;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {
		setBoard(virtualBoard, *it);
		if ((*it)->res == Result::WIN)
			return *it;
		double selectionValue = ((*it)->nbWin / (*it)->nbTry);


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
		nbSimulationForDepth += (*it)->nbTry;
	}
	//std::cout << "nb simulation for depth : " << nbSimulationForDepth << std::endl;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {

		double selectionValue = (  (*it)->nbWin / (*it)->nbTry)   + (EXPLORATION_CONST  *  sqrtf(  log(nbSimulationForDepth / (*it)->nbTry) ) );
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
	//std::cout << "select lvl : " << node->depth << std::endl;

	if (node->depth == info.size * info.size || node == NULL)
		return;
	if (node->childs.empty())//if The current node as not been expand previously expand it now
		expand(node);
	else//else select the new node to explore with MCTS algorithm
	{
		select(selectOneNodeWithMctsAlgo(node));
	}
	//std::cout << "end of select" << std::endl;

}



void Root::expand(Node*node)
{
//	std::cout << "expand" << std::endl;


	//code repetition in simulate -- look for possible patch
	Color **board = new Color*[info.size];
	for (int i = 0; i < info.size; i++) {
		board[i] = new Color[info.size];
		for (int j = 0; j < info.size; j++) {
			board[i][j] = Color::NONE;
		}
	}

	setBoard(board, node);
	//std::cout << "print in expand" << std::endl;
	//printBoard(board);
	for (int i = 0; i < info.size; i ++) {
		for (int j = 0; j < info.size; j ++) {
			if (board[i][j] == Color::NONE) {
				Node *newChild = new Node( Move( Pos(j, i), Color((node->move.color == Color::WHITE) + 1 )), node->depth + 1, node);
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

	for (int i = 0; i < info.size; i++) {
		for (int j = 0; j < info.size; j++) {
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
	for (int i = 0; i < info.size; i++) {
		for (int j = 0; j < info.size; j++) {
			color[i][j] = Color::NONE;
		}
	}
	while (node->father != NULL) {
		color[node->move.pos.y][node->move.pos.x] = node->move.color;
		node = node->father;
	}
}

void Root::simulate(Node *node)
{

	
	//std::cout << "simulate" << std::endl;

	//Creating a board with actual depth play
	Color **board = virtualBoard;

	setBoard(board, node);

	int nbPlay = node->depth;

	//set the ref on play
	Color	currentPlayer = node->move.color;

	//set the ref
	Pos		lastPlay = node->move.pos;

	//set the player changeManagement
	int		sign = 1;//work as a switch between player wanted to name it switch but it's not possible

	if (currentPlayer == Color::BLACK)
		sign = -1;


	int totalNbPlays = info.size * info.size;

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

		//Get a valid Play
		lastPlay = randomPos(info.size);
		while  (board[lastPlay.y][lastPlay.x] != Color::NONE)///try to randomize differently
			lastPlay = randomPos(info.size);

		board[lastPlay.y][lastPlay.x] = currentPlayer;

		sign *= -1;

		nbPlay += 1;

	}

	//std::cout << "end simulate" << std::endl;

	if (nbPlay == totalNbPlays)//see If no update is better
		update(node, 0.5);
	else
		update(node, currentPlayer == node->move.color);
	//std::cout << "end of update" << std::endl;
}



void Root::update(Node * node, double win)
{
//	std::cout << "update" << std::endl;

	while (node != NULL) {
		node->nbTry += 1;
		node->nbWin += win;
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

	for (int y = currentPos.y + 1; y < info.size && board[x][y] == color ; y++)
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

	for (int x = currentPos.x + 1; x < info.size && board[x][y] == color; x++)
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
	Color **board = new Color*[info.size];
	for (int i = 0; i < info.size; i++) {
		board[i] = new Color[info.size];
		for (int j = 0; j < info.size; j++) {
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

	while (x < info.size && y >= 0 && board[x][y] == color)
	{
		x++;
		y--;
		max++;
	}
	y = currentPos.y + 1;
	x = currentPos.x - 1;
	while (x >= 0 && y < info.size && board[x][y] == color)
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

	while (x < info.size && y < info.size && board[x][y] == color)
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
