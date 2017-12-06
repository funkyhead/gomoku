#include "stdafx.h"
#include "Mcts.h"

#define EXPLORATION_CONST 1.44
#define NUMBER_TO_ALIGN_TO_WIN 5

Mcts::Mcts()
{
	// Set the game informations here
	info.timeout = 5;
	info.boardSize = 19;

	// Initialize the root node
	rootNode = new Node();
	rootNode->depth = 0;
	rootNode->father = NULL;
	currentNode = rootNode;

	color = Color::WHITE;

	expandRange = 1;
	virtualBoard = createEmptyBoard(info.boardSize);
}

Mcts::~Mcts()
{
	while (!(rootNode)->childs.empty())
		deleteNodes((rootNode)->childs[0]);
}

Pos		Mcts::begin()
{
	// Start the timer
	timer.startClock(info.timeout);

	// If there is no move yet, create the first move in the middle of the board
	if (rootNode->childs.empty())
	{
		currentNode = new Node(Move(Pos(info.boardSize / 2, info.boardSize / 2), color), rootNode);
		rootNode->childs.push_back(currentNode);
	}
	else // In case of restart
		currentNode = selectBestChild(rootNode);

	while (!timer.timeOut())
	{
		select(currentNode);
	}
	return currentNode->move.pos;
}


Pos		Mcts::play(Pos competitorLastPlay)
{
	// Start the timer
	timer.startClock(info.timeout);

	//Set the currentNode to competitor lastPlay  
	Node *competitorNode = NULL;

	for (std::vector<Node*>::iterator it = currentNode->childs.begin(); it != currentNode->childs.end(); it++)
	{
		if ((*it)->move.pos == competitorLastPlay)
		{
			competitorNode = (*it);
			break;
		}
	}

	//if no actual node match his play create a new one;
	if (competitorNode == NULL)
	{
		competitorNode = new Node(Move(competitorLastPlay, Color((color == Color::WHITE) + 1)), currentNode);
		currentNode->childs.push_back(competitorNode);
	}

	setBoard(virtualBoard, competitorNode);			

				/* THIS IS WHERE THE RANGE FINDER CODE STARTS*/

	int minY = info.boardSize - 1;
	int minX = info.boardSize - 1;
	int maxY = 0;
	int maxX = 0;

	for (int x = 0; x < info.boardSize; x++)
	{
		for (int y = 0; y < info.boardSize; y++)
		{
			if (virtualBoard[x][y] != Color::NONE)
			{
				minY = minY > y ? y : minY;
				minX = minX > x ? x : minX;
				maxY = maxY < y ? y : maxY;
				maxX = maxX < x ? x : maxX;
			}
		}
	}
	minY -= expandRange;
	minX -= expandRange;
	maxY += expandRange;
	maxX += expandRange;
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

		/* THIS IS WHERE THE RANGE FINDER CODE ENDS*/

	while (!timer.timeOut())
	{
		//select(competitorNode);
		selectWithLimitedRange(competitorNode, minX, maxX, minY, maxY);
	}

	//get the Selection for currentDepth, and set the current Node.

	//return (currentNode = selectOneNode(competitorNode))->move.pos;		//TODO: do better selection node for the plays  /* is this even a TODO ?! */
	if ((currentNode = getNextMove(competitorNode)) == NULL)
	{
		throw (std::exception("getNextMove returned NULL"));
	}
	setBoard(virtualBoard, currentNode);
	return (currentNode)->move.pos;
}




double		Mcts::getNbSimulationForDepth(Node *node, int depth)
{
	if (node->depth == depth)
		return node->nbTries;
	else if (node->childs.empty())
		return 0;
	double nbSimulation = 0;
	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++)
		{
			nbSimulation += getNbSimulationForDepth((*it), depth);
		}
	return nbSimulation;
}


Node	*Mcts::getNextMove(Node *node)
{
	Node		*selectedNode = NULL;
	double		maxSelectionValue = 0;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		setBoard(virtualBoard, *it);
		double selectionValue = ((*it)->nbWins / (*it)->nbTries);

		if (selectionValue >= maxSelectionValue && expectedOutput(*it) != Result::LOOSE)
		{
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}


Result	Mcts::expectedOutput(Node *node) {
	if (node->res != Result::NONE || node->childs.empty()) {
		return node->res;
	}
	return expectedOutput(selectBestChild(node));
}

Node	*Mcts::selectBestChild(Node *node) {

	Node	*selectedNode = NULL;
	double	maxSelectionValue = 0;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		if ((*it)->res == Result::WIN)
			return *it;
		double selectionValue = ((*it)->nbWins / (*it)->nbTries);

		if (selectionValue >= maxSelectionValue)
		{
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}

Node	*Mcts::selectOneNodeWithMctsAlgo(Node *node) {

	Node	*selectedNode = NULL;
	double	maxSelectionValue = 0;
	double	nbSimulationForDepth = 0;

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		nbSimulationForDepth += (*it)->nbTries;
	}

	for (std::vector<Node*>::iterator it = node->childs.begin(); it != node->childs.end(); it++)
	{
		double selectionValue = ((*it)->nbWins / (*it)->nbTries) + (EXPLORATION_CONST  *  sqrtf(log(nbSimulationForDepth / (*it)->nbTries)));

		if (selectionValue >= maxSelectionValue) {//&& (*it)->res == Result::NONE) {
			maxSelectionValue = selectionValue;
			selectedNode = (*it);
		}
	}
	return selectedNode;
}


void Mcts::restart()
{
	currentNode = rootNode;
}

void  Mcts::select(Node *node)
{
	if (node->depth == info.boardSize * info.boardSize || node == NULL)
		return;
	if (node->childs.empty())//if The current node as not been expand previously expand it now
		expand(node);
	else//else select the new node to explore with MCTS algorithm
	{
		select(selectOneNodeWithMctsAlgo(node));
	}

}

void Mcts::selectWithLimitedRange(Node * node, int minX, int maxX, int minY, int maxY) {

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


void Mcts::expand(Node*node)
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

	//for (int i = minX; i < maxX; i ++) {
	//for (int j = minY; j < maxY; j ++) {
	for (int i = 0; i < info.boardSize; i++) {
		for (int j = 0; j < info.boardSize; j++) {
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

void Mcts::expandWithLimitedRange(Node * node, int minX, int maxX, int minY, int maxY) {
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


	for (int i = minX; i < maxX; i++) {
		for (int j = minY; j < maxY; j++) {
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

void Mcts::printBoard(Color **board) {

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




void	Mcts::setBoard(Color **board, Node *node)
{
	for (int i = 0; i < info.boardSize; i++)
	{
		for (int j = 0; j < info.boardSize; j++)
			board[i][j] = Color::NONE;
	}
	while (node->father != NULL)
	{
		board[node->move.pos.y][node->move.pos.x] = node->move.color;
		node = node->father;
	}
}

void Mcts::simulate(Node *node)
{
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


	int totalNbPlays = info.boardSize * info.boardSize;

	if (isFivePositionAligned(board, Pos(lastPlay.y, lastPlay.x), currentPlayer)) {
		node->res = Result(2 - (currentPlayer == color));
	}
	while (!isFivePositionAligned(board, Pos(lastPlay.y, lastPlay.x), currentPlayer) && nbPlay < totalNbPlays) {

		currentPlayer = Color((int)currentPlayer + sign);

		//Get a valid Play
		lastPlay = getRandomPos(info.boardSize);
		while (board[lastPlay.y][lastPlay.x] != Color::NONE)// TODO try to randomize differently
			lastPlay = getRandomPos(info.boardSize);

		board[lastPlay.y][lastPlay.x] = currentPlayer;

		sign *= -1;

		nbPlay += 1;

	}

	if (nbPlay == totalNbPlays)//see If no update is better
		update(node, 0.5);
	else
		update(node, currentPlayer == node->move.color);
}



void Mcts::update(Node * node, double win)
{
	while (node != NULL)
	{
		node->nbTries += 1;
		node->nbWins += win;
		node = node->father;
		if (win == 1)
			win = 0;
		else if (win == 0)
			win = 1;
	}
}

bool	Mcts::isFivePositionAligned(Color **& board, Pos currentPos, Color color) {
	return (VerticalAlign(board, currentPos, color) ||
		HorizontalAlign(board, currentPos, color) ||
		RightDiagonalAlign(board, currentPos, color) ||
		LeftDiagonalAlign(board, currentPos, color));
}

bool	Mcts::VerticalAlign(Color **& board, Pos &currentPos, Color color) {
	int max = 1;
	int x = currentPos.x;

	for (int y = currentPos.y + 1; y < info.boardSize && board[x][y] == color; y++)
	{
		max++;
	}
	for (int y = currentPos.y - 1; y >= 0 && board[x][y] == color; y--)
	{
		max++;
	}

	if (max >= NUMBER_TO_ALIGN_TO_WIN)
		return true;
	return false;
}

bool	Mcts::HorizontalAlign(Color **& board, Pos currentPos, Color color) {
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

	if (max >= NUMBER_TO_ALIGN_TO_WIN)
		return true;
	return false;
}

bool Mcts::testBoard() {
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

Color		**Mcts::createEmptyBoard(int size)
{
	Color	**board;

	board = new Color*[size];
	for (int i = 0; i < size; i++)
	{
		board[i] = new Color[size];
		for (int j = 0; j < size; j++)
			board[i][j] = Color::NONE;
	}
	return board;
}



bool	Mcts::RightDiagonalAlign(Color **& board, Pos currentPos, Color color) {
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

	if (max >= NUMBER_TO_ALIGN_TO_WIN)
		return true;
	return false;
}

bool	Mcts::LeftDiagonalAlign(Color **& board, Pos currentPos, Color color) {
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

	if (max >= NUMBER_TO_ALIGN_TO_WIN)
		return true;
	return false;
}

void Mcts::deleteNodes(Node *node)
{
	while (!node->childs.empty())
	{
		deleteNodes(node->childs[0]);
	}
	delete node;
}

Pos Mcts::getRandomPos(int size)
{
	int	x, y;
	x = rand() % size;
	y = rand() % size;

	return Pos(x, y);
}