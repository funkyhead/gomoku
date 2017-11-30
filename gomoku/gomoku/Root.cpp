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
	info.timeout = 5.0;
	info.size = 20;
	depth = 0;
}

Root::Root(const std::string &) : Root()
{
}

Root::~Root()
{
	while (!(this)->childs.empty()) {
		deleteNodes((this)->childs[0]);
	}
}




Pos Root::play(Pos competitorLastPlay)
{
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
		competitorNode = new Node(Move(competitorLastPlay, Color((color == Color::WHITE) + 1)), currentNode->depth + 1, currentNode);
		currentNode->childs.push_back(competitorNode);
		//std::cout << "play : create Competitor node" << std::endl;
	}

	//main loop : Selection->Selection->...->expand->simulate->update;
	while (!timer.timeOut()) {
		select(competitorNode);
	}
	//get the Selection for currentDepth, and set the current Node.

	//std::cout << "nb childre for last adversery play : " << competitorNode->childs.size() << std::endl;
	//Sleep(100000);
	return (currentNode = selectOneNode(competitorNode))->move.pos;
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




Node *Root::selectOneNode(Node *node) {

	Node *selectedNode = NULL;
	double	maxSelectionValue = 0;
	double nbSimulationForDepth = getNbSimulationForDepth(this, node->depth + 1);

	//std::cout << "nb simulation for depth : " << nbSimulationForDepth << std::endl;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++) {

		/*
		std::cout << "nbTry for Node : " <<  (*it)->nbTry << std::endl;
		std::cout << "computed value : " << nbSimulationForDepth / (*it)->nbTry << std::endl;
		std::cout << "computed value with log: " << log(nbSimulationForDepth / (*it)->nbTry) << std::endl;
		*/

		double selectionValue = (  (*it)->nbWin / (*it)->nbTry)   + (EXPLORATION_CONST  *  sqrtf(  log(nbSimulationForDepth / (*it)->nbTry) ) );
		

		if (selectionValue > maxSelectionValue) {
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}




void  Root::select(Node *node)
{
	if (node->childs.empty())
		expand(node);
	else
	{
		select(selectOneNode(node));
	}
}



Node *Root::expand(Node*node)
{
	//code repetition in simulate -- look for possible patch
	Color **board = new Color*[info.size];
	for (int i = 0; i < info.size; i ++) {
		board[i] = new Color[info.size];
		for (int j = 0; j < info.size; j ++) {
			board[i][j] = Color::NONE;
		}
	}
	int nbPlay = setBoard(board, node);

	for (int i = 0; i < info.size; i ++) {
		for (int j = 0; j < info.size; j ++) {
			if (board[i][j] == Color::NONE) {
				Node *newChild = new Node(Move(Pos(j, i), color), node->depth + 1, node);
				node->childs.push_back(newChild);
				simulate(newChild);
				//std::cout << "nb try after simulation for child :  " << newChild->nbTry << std::endl;
			}
		}
	}
	delete board;
	return selectOneNode(node);
}

void Root::printBoard(Color **board) {

	for (int i = 0; i < info.size; i++) {
		for (int j = 0; j < info.size; j++) {
			if (board[i][j] == Color::WHITE)
				std::cout << "O";
			else if (board[i][j] == Color::BLACK)
				std::cout << "X";
			else
				std::cout << "#";
			std::cout << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


int	Root::setBoard(Color **color, Node *node) {
	int nbPlay = 0;
	while (node != NULL) {
		nbPlay += 1;
		color[node->move.pos.y][node->move.pos.x] = node->move.color;
		node = node->father;
	}
	return nbPlay;
}

void Root::simulate(Node *node)
{
	//Creating a board with actual depth play
	Color **board = new Color*[info.size];
	for (int i = 0; i < info.size; i++) {
		board[i] = new Color[info.size];
		for (int j = 0; j < info.size; j++) {
			board[i][j] = Color::NONE;
		}
	}
	setBoard(board, node);
	int nbPlay = node->depth;


	//std::cout << nbPlay << std::endl;;

	//set the ref on play
	Color	currentPlayer = node->move.color;


	//std::cout << "currentColor" << (int)currentPlayer << std::endl;
	//Sleep(1000);

	Pos		lastPlay = node->move.pos;

	//set the player changeManagement
	int		sign = 1;//work asa switchbetween player wanted to name it switch but it's not possible
	if (currentPlayer == Color::BLACK)
		sign = -1;
	int totalNbPlays = info.size * info.size;


	while (!isFivePositionAligned(board, lastPlay, currentPlayer) && nbPlay != totalNbPlays) {
		currentPlayer = Color((int)currentPlayer + sign);

		lastPlay = randomPos(info.size);
		while  (board[lastPlay.y][lastPlay.x] != Color::NONE)
			lastPlay = randomPos(info.size);
		board[lastPlay.y][lastPlay.x] = currentPlayer;
		sign *= -1;
		nbPlay += 1;
	}


	//printBoard(board);
	//Sleep(100000);

	if (nbPlay == totalNbPlays)
		update(node, false);
	delete board;
	update(node, (currentPlayer == color));
}



void Root::update(Node * node, bool win)
{
	while (node != NULL) {
		node->nbTry += 1;
		node->nbWin += win;
		node = node->father;
	}
}






bool	Root::isFivePositionAligned(Color **& board, Pos currentPos, Color color) {
	return (VerticalAlign(board, currentPos, color) ||
		HorizontalAlign(board, currentPos, color) ||
		RightDiagonalAlign(board, currentPos, color) ||
		LeftDiagonalAlign(board, currentPos, color));
}

bool	Root::VerticalAlign(Color **& board, Pos &currentPos, Color color) {
	int max = -1;
	int x = currentPos.x;

	for (int y = currentPos.y; y < info.size && board[x][y] == color ; y++)
	{
		max++;
	}
	for (int y = currentPos.y; y >= 0 && board[x][y] == color; y--)
	{
		max++;
	}
	if (max >= 5)
		return true;
	return false;
}

bool	Root::HorizontalAlign(Color **& board, Pos currentPos, Color color) {
	int max = -1;
	int y = currentPos.y;

	for (int x = currentPos.x; x < info.size && board[x][y] == color; x++)
	{
		max++;
	}
	for (int x = currentPos.x; x >= 0 && board[x][y] == color; x--)
	{
		max++;
	}
	if (max >= 5)
		return true;
	return false;
}

bool	Root::RightDiagonalAlign(Color **& board, Pos currentPos, Color color) {
	int max = -1;
	int y = currentPos.y;
	int x = currentPos.x;

	while (x < info.size && y >= 0 && board[x][y] == color)
	{
		x++;
		y--;
		max++;
	}
	y = currentPos.y;
	x = currentPos.x;
	while (x >= 0 && y < info.size && board[x][y] == color)
	{
		x--;
		y++;
		max++;
	}

	if (max >= 5)
		return true;
	return false;
}

bool	Root::LeftDiagonalAlign(Color **& board, Pos currentPos, Color color) {
	int max = -1;
	int y = currentPos.y;
	int x = currentPos.x;

	while (x < info.size && y < info.size && board[x][y] == color)
	{
		x++;
		y++;
		max++;
	}
	y = currentPos.y;
	x = currentPos.x;
	while (x >= 0 && y >= 0 && board[x][y] == color)
	{
		x--;
		y--;
		max++;
	}

	if (max >= 5)
		return true;
	return false;
}


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
