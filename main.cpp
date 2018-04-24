#include <iostream>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#include "pisqpipe.h"
#include <windows.h>
#include <vector>
using namespace std;
const char *infotext="name=\"Random\", author=\"Zhou Yeju\", version=\"3.2\", country=\"Hong Kong\", www=\"none\"";

#define MAX_BOARD 100
int board[MAX_BOARD][MAX_BOARD];
static unsigned seed;
vector<int> my_preMove;
vector<int> enemy_preMove;
// variables to record last moves


void brain_init() 
{
  if(width<5 || height<5){
    pipeOut("ERROR size of the board");
    return;
  }
  if(width>MAX_BOARD || height>MAX_BOARD){
    pipeOut("ERROR Maximal board size is %d", MAX_BOARD);
    return;
  }
  seed=start_time;
  pipeOut("OK");
}

void brain_restart()
{
  int x,y;
  for(x=0; x<width; x++){
    for(y=0; y<height; y++){
      board[x][y]=0;
    }
  }
  pipeOut("OK");
}

int isFree(int x, int y)
{
  return x>=0 && y>=0 && x<width && y<height && board[x][y]==0;
}

void brain_my(int x,int y)
{
  if(isFree(x,y)){
    board[x][y]=1;
  }else{
    pipeOut("ERROR my move [%d,%d]",x,y);
  }
}

void brain_opponents(int x,int y) 
{
  if(isFree(x,y)){
    board[x][y]=2;
  }else{
    pipeOut("ERROR opponents's move [%d,%d]",x,y);
  }
}

void brain_block(int x,int y)
{
  if(isFree(x,y)){
    board[x][y]=3;
  }else{
    pipeOut("ERROR winning move [%d,%d]",x,y);
  }
}

int brain_takeback(int x,int y)
{
  if(x>=0 && y>=0 && x<width && y<height && board[x][y]!=0){
    board[x][y]=0;
    return 0;
  }
  return 2;
}

unsigned rnd(unsigned n)
{
  seed=seed*367413989+174680251;
  return (unsigned)(UInt32x32To64(n,seed)>>32);
}

// evaluate chess types

int* brain_evaluateLine(string s, int left_noneEnemy, int right_noneEnemy, int left_first, int right_last, int numOfPlayer){
	int* chessType = new int[8];
	for(int i = 0; i < 8; ++i) chessType[i] = 0;
	bool leftFree = left_noneEnemy < left_first;
	bool rightFree = right_noneEnemy > right_last;
	int length = right_last - left_first + 1;
	// no chance to get 5 in a row
	if(right_noneEnemy - left_noneEnemy < 4) return chessType;
	// current length = 1
	else if(length == 1){
		if(leftFree && rightFree) ++chessType[4];
	}
	
	// current length = 2
	else if(length == 2){
		// live 2
		if(leftFree && rightFree) ++chessType[3];
		// dead 2
		else ++chessType[7];
	}

	// current length = 3
	else if(length == 3){
		// 3 in a row
		if(numOfPlayer == 3){
			// live 3
			if(leftFree && rightFree) ++chessType[2];
			// dead 3
			else ++chessType[6];
		}
		else {
			if(leftFree && rightFree) ++chessType[3];
			else ++chessType[7];
		}
	}
	
	// current length = 4
	else if(length == 4){
		// 4 in a row
		if(numOfPlayer == 4){
			// live 4
			if(leftFree && rightFree) ++chessType[1];
			// dead 4
			else ++chessType[5];
		}
		// leap 3
		else if(numOfPlayer == 3){
			// live 3
			if(leftFree && rightFree) ++chessType[2];
			// dead 3
			else ++chessType[6];
		}
		// leap 2 dead 2
		else {
			if(leftFree && rightFree) ++chessType[3];
			else ++chessType[7];
		}
	}
	// current length = 5
	else if(length == 5){
		// 5 in a row 
		if(numOfPlayer == 5) ++chessType[0];
		// leap 4
		else if(numOfPlayer == 4) ++chessType[5];
		else if(numOfPlayer == 3) {
			// xx__x
			if(s[left_first + 1] != '0'){
				if(leftFree) ++chessType[3];
				else ++chessType[7];
			}
			// x__xx
			else if(s[right_last - 1] != '0'){
				if(rightFree) ++chessType[3];
				else ++chessType[7];
			}
		}
		else ++chessType[3];
	}
	// current length = 6
	else if(length == 6){
		// 6 in a row
		if(numOfPlayer == 6) ++chessType[0];
		// leap 5
		else if(numOfPlayer == 5) ++chessType[5];
		// leap 4
		else if(numOfPlayer == 4){
			// xx__xx
			if(s[left_first + 2] == '0' && s[left_first + 3] == '0') ++chessType[3];
			// x__xxx
			else if(s[left_first + 1] == '0' && s[left_first + 2] == '0'){
				// live 3
				if(rightFree) ++chessType[2];
				else ++chessType[6];
			}
			// xxx__x
			else if(s[left_first + 3] == '0' && s[left_first + 4] == '0'){
				// live 3
				if(leftFree) ++chessType[2];
				else ++chessType[6];
			}
			// x_xx_x
			else {
				// dead 3
				if(left_first == left_noneEnemy && right_last == right_noneEnemy) ++chessType[6];
				// live 3
				else ++chessType[2];
			}
		}
		else if(numOfPlayer == 3){
			if(leftFree && rightFree) ++chessType[7];
			else ++chessType[3];
		}
	}
	
	// current length = 7
	else if(length == 7){
		// 7 in a row
		if(numOfPlayer == 7) ++chessType[0];
		// leap 6
		else if(numOfPlayer == 6){
			// 5 in a row
			if(s[left_first + 1] == '0' || s[right_last - 1] == '0') ++chessType[0];
			// xx_xxxx
			if(s[left_first + 2] == '0'){
				// live 4
				if(rightFree) ++chessType[1];
				// dead 4
				else ++chessType[5];
			}
			// xxxx_xx
			if(s[right_last - 2] == '0'){
				// live 4
				if(leftFree) ++chessType[1];
				// dead 4
				else ++chessType[5];
			}
		}
		// leap 5
		else if(numOfPlayer == 5){
			// xxxx__x
			if(s[right_last - 1] == '0' && s[right_last - 2] == '0'){
				if(leftFree) ++chessType[1];
				else ++chessType[5];
			}
			// x__xxxx
			else if(s[left_first + 1] == '0' && s[left_first + 2] == '0'){
				if(rightFree) ++chessType[1];
				else ++chessType[5];
			}
			// xx__xxx
			else if(s[left_first + 2] == '0' && s[left_first + 3] == '0'){
				if(rightFree) ++chessType[2];
				else ++chessType[6];
			}
			// xxx__xx
			else if(s[right_last - 2] == '0' && s[right_last - 3] == '0'){
				if(leftFree) ++chessType[2];
				else ++chessType[6];
			}
			// xx_x_xx
			else if(s[left_first + 2] == '0' && s[left_first + 4] == '0'){
				if(left_noneEnemy == left_first && right_noneEnemy == right_last) ++chessType[6];
				else ++chessType[2];
			}
			// x_xx_xx
			else ++chessType[5];
		}
		// leap 4
		else if(numOfPlayer == 4){
			// xx___xx
			if(s[left_first + 1] != '0' && s[right_last - 1] != '0') ++chessType[3];
			// x_???xx
			else if(s[left_first + 1] == '0'){
				if(rightFree) ++chessType[2];
				else ++chessType[6];
			}
			// xx???_x
			else if(s[right_last - 1] == '0'){
				if(leftFree) ++chessType[2];
				else ++chessType[6];
			}
		}
		// leap 3
		else if(numOfPlayer == 3){
			// x____xx or xx____x
			if((s[right_last - 1] != '0' && rightFree) || (s[left_first + 1] != '0' && leftFree)) ++chessType[3];
			else ++chessType[7];
		}
		else ++chessType[7];
	}
	// current length = 8
	else if(length == 8){
		// leap 3
		if(numOfPlayer == 3){
			// xx_____x
			if(s[left_first + 1] == '0' || s[left_first + 2] == '0'){
				if(leftFree) ++chessType[3];
				else ++chessType[7];
			}
			// x_____xx
			else if(s[right_last - 1] == '0' || s[right_last - 2] == '0'){
				if(rightFree) ++chessType[3];
				else ++chessType[7];
			}			
		}
		else if(numOfPlayer == 4){
			// xxx____x or x_xx___x or xx_x___x
			if((s[left_first + 1] != '0' && s[left_first + 2] != '0') || (s[left_first + 2] != '0' && s[left_first + 3] != '0') || (s[left_first + 1] != '0' && s[left_first + 3] != '0')){
				if(leftFree) ++chessType[2];
				else ++chessType[6];
			}
			// x____xxx or x___xx_x or x___x_xx
			else if((s[right_last - 1] != '0' && s[right_last - 2] != '0') || (s[right_last - 2] != '0' && s[right_last - 3] != '0') || (s[right_last - 1] != '0' && s[right_last - 3] != '0')){
				if(rightFree) ++chessType[2];
				else ++chessType[6];
			}
		}
		else if(numOfPlayer == 5){
			// xxxx___x
			if(s[left_first + 1] != '0' && s[left_first + 2] != '0' && s[left_first + 3] != '0'){
				if(leftFree) ++chessType[1];
				else ++chessType[5];
			}
			// x___xxxx
			else if(s[right_last - 1] != '0' && s[right_last - 2] != '0' && s[right_last - 3] != '0'){
				if(rightFree) ++chessType[1];
				else ++chessType[5];
			}
			// x_xx_xx
			else if(s[left_first + 1] == '0' || s[right_last - 1] == '0') ++chessType[5];
		}
	}
	return chessType;
}

// calculate parameters of a string/line of chess
int* brain_stringToEvaluate(string s, int origin, int current_player){
	char player = current_player + '0';
	char enemy = (player == '1') ? '2' : '1'; 
	int left_noneEnemy = origin; 
	int right_noneEnemy = origin;
	// longest line without enemy chess
	while(left_noneEnemy > 0){
		if(s[left_noneEnemy - 1] == enemy) break;
		--left_noneEnemy;
	}
	while(right_noneEnemy < s.size() - 1){
		if(s[right_noneEnemy + 1] == enemy) break;
		++right_noneEnemy;
	}
	int left_first = left_noneEnemy;
	int right_last = right_noneEnemy;
	// longest line enclosed by my chess
	while(left_first < origin){
		if(s[left_first] == player) break;
		++left_first;
	}
	while(right_last > origin){
		if(s[right_last] == player) break;
		--right_last;
	}
	int numOfPlayer = 0;
	for(int i = left_first; i <= right_last; ++i){
		if(s[i] == player) ++numOfPlayer;
	}
	return brain_evaluateLine(s, left_noneEnemy, right_noneEnemy, left_first, right_last, numOfPlayer);
}

// convert a line of chess pieces into string, evaluate the score
int brain_evaluate_lineToString(int x, int y, int player) {
  //horizontal
    string row = "";
    for (int i = max(x - 5, 0); i <= min(x + 5, width - 1); ++i) {
    	char temp = board[i][y] + '0';
    	row += temp;
    }
    int* horizontal = brain_stringToEvaluate(row, x - max(x - 5, 0), player);
    //vertical
    string col = "";
    for (int i = max(y - 5, 0); i <= min(y + 5, height - 1); ++i) {
    	char temp = board[x][i] + '0';
    	col += temp;
    }
    int* vertical = brain_stringToEvaluate(col, y - max(y - 5, 0), player);
  //left bottom to right upper diagonal
    int sum = x + y;
	int x1 = max(max(x - 5, 0), sum - min(y + 5, height - 1));
	int x2 = min(min(x + 5, width - 1), sum - max(y - 5, 0));
    string diagonal_1 = "";
    for (int i = x1; i <= x2; ++i) {
    	char temp = board[i][sum - i] + '0';
    	diagonal_1 += temp;
    } 
	int* _diagonal_1 = brain_stringToEvaluate(diagonal_1, x - x1, player);
  //left upper to right bottom diagonal
	int sub = x - y;
	x1 = max(max(x - 5, 0), max(y - 5, 0) + sub);
	x2 = min(min(x + 5, width - 1), min(y + 5, height - 1) + sub);
    string diagonal_2 = "";
    for (int i = x1; i <= x2; ++i) {
    	char temp = board[i][i - sub] + '0';
    	diagonal_2 += temp;
    }
    int* _diagonal_2 = brain_stringToEvaluate(diagonal_2, x - x1, player);
	// total score
    int total[8];
	for(int i=0;i<8;i++){
		total[i] = horizontal[i] + vertical[i] + _diagonal_1[i] + _diagonal_2[i];
	}	
	delete[] horizontal;
	delete[] vertical;
	delete[] _diagonal_1;
	delete[] _diagonal_2;
	if(total[0] >= 1) return 1000000;
	else if(total[1] >= 1) return 100000;
	else if(total[2] + total[5] >= 2) return 100000;
	else return(total[2] * 10000 + total[3] * 1000 + total[4] * 100 + total[5] * 10000 + total[6] * 1000 + total[7] * 100); 
}

// check if current position has at least one neighbour
bool hasNeighbour(int x,int y){
	// horizontal
	for(int i = max(x - 2, 0); i <= min(x + 2, width - 1); ++i){
		if(board[i][y] != 0) return true;
	}
	// vertical
	for(int i = max(y - 2, 0); i <= min(y + 2, height - 1); ++i){
		if(board[x][i] != 0) return true;
	}
	// diagonal 1
	int sub = x - y;
	int x1 = max(max(x - 2, 0), max(y - 2, 0) + sub);
	int x2 = min(min(x + 2, width - 1), min(y + 2, height - 1) + sub);
	for(int i = x1; i <= x2; ++i){
		if(board[i][i-sub] != 0) return true;
	}
	int sum = x + y;
	x1 = max(max(x - 2, 0), sum - min(y + 2, height - 1));
	x2 = min(min(x + 2, width - 1), sum - max(y - 2, 0));
	for(int i = x1; i <= x2; ++i){
		if(board[i][sum-i] != 0) return true;
	}
	return false;
}

// find empty positions with neighbours in 2 units distance
vector<int> brain_avail(){
	vector<int> avail;
	for(int i = 0; i < width; ++i){
		for(int j = 0; j < height; ++j){
			if(board[i][j] == 0){
				if(hasNeighbour(i, j)){
					avail.push_back(i);
					avail.push_back(j);
				}
			}
		}
	}
	return avail;
}

// min-max search with alpha-beta pruning
vector<int> brain_minmax(int depth, int alpha, int beta){
	// evaluate the score at the bottom depth
	if(depth == 0){
		vector<int> result;
		result.push_back(-1);
		result.push_back(-1);
		int player_score = 0;
		int enemy_score = 0;
		for(int i = 0; i < my_preMove.size() - 1; i += 2){
			int x = my_preMove[i];
			int y = my_preMove[i + 1];
			player_score += brain_evaluate_lineToString(x, y, 1);
		}
		for(int i = 0; i < enemy_preMove.size() - 1; i += 2){
			int x = enemy_preMove[i];
			int y = enemy_preMove[i + 1];
			enemy_score += brain_evaluate_lineToString(x, y, 2);
		}
		result.push_back(player_score - enemy_score);
		return result;
	}
	// max func
	else if(depth%2 == 0){
		vector<int> avail_max = brain_avail();
		int max_x = -1;
		int max_y = -1;
		while(!avail_max.empty()){
			if(alpha >= beta) break;
			int y = avail_max.back();
			avail_max.pop_back();
			int x = avail_max.back();
			avail_max.pop_back();
			board[x][y] = 1;
			my_preMove.push_back(x);
			my_preMove.push_back(y);
			vector<int> minmax = brain_minmax(depth - 1, alpha, beta);
			if(minmax[2] > alpha) {
				alpha = minmax[2];
				max_x = x;
				max_y = y;
			}
			board[x][y] = 0;
			my_preMove.pop_back();
			my_preMove.pop_back();
		}
		vector<int> best_max;
		best_max.push_back(max_x);
		best_max.push_back(max_y);
		best_max.push_back(alpha);
		return best_max;
	}
	// min func
	else if(depth%2 == 1){
		vector<int> avail_min = brain_avail();
		int min_x = -1;
		int min_y = -1;
		while(!avail_min.empty()){
			if(alpha >= beta) break;
			int y = avail_min.back();
			avail_min.pop_back();
			int x = avail_min.back();
			avail_min.pop_back();
			board[x][y] = 2;
			enemy_preMove.push_back(x);
			enemy_preMove.push_back(y);
			vector<int> minmax = brain_minmax(depth - 1, alpha, beta);
			if(minmax[2] < beta) {
				beta = minmax[2];
				min_x = x;
				min_y = y;
			}
			board[x][y] = 0;
			enemy_preMove.pop_back();
			enemy_preMove.pop_back();
		}
		vector<int> best_min;
		best_min.push_back(min_x);
		best_min.push_back(min_y);
		best_min.push_back(beta);
		return best_min;
	}
}

// best option for next move
vector<int> brain_nextMove(int player){
	vector<int> avail = brain_avail();
	int _x = -1;
	int _y = -1;
	int score = 0;
	while(!avail.empty()){
		int y = avail.back();
		avail.pop_back();
		int x = avail.back();
		avail.pop_back();
		board[x][y] = player;
		int s = brain_evaluate_lineToString(x, y, player);
		if(s >= score){
			score = s;
			_x = x;
			_y = y;
		}
		board[x][y] = 0;
	}
	vector<int> result;
	result.push_back(_x);
	result.push_back(_y);
	result.push_back(score);
	return result;
}

void init_preMove(){
	while(!my_preMove.empty()){
		my_preMove.pop_back();
	}
	while(!enemy_preMove.empty()){
		enemy_preMove.pop_back();
	}
}

// take a move
void brain_turn() 
{
	if(terminateAI != 0) return;
	if(brain_avail().size() == 0) {
		do_mymove(width / 2, height / 2);
		return;
	}
	vector<int> enemy_nextMove = brain_nextMove(2); 
	vector<int> my_nextMove = brain_nextMove(1);
	if(my_nextMove[2] >= 1000000 && isFree(my_nextMove[0], my_nextMove[1])){
    	init_preMove();
	 do_mymove(my_nextMove[0], my_nextMove[1]);
	 return;
	}
    if(enemy_nextMove[2] >= 100000 && isFree(enemy_nextMove[0], enemy_nextMove[1])){
    	init_preMove();
	 do_mymove(enemy_nextMove[0], enemy_nextMove[1]);
	 return;
	}
	else{
		int depth = 4;
		vector<int> move=brain_minmax(depth,-10000000,10000000);
		init_preMove();
		do_mymove(move[0],move[1]);
		return;
	}
}

void brain_end()
{
}

#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x,int y)
{
  HDC dc;
  HWND wnd;
  RECT rc;
  char c;
  wnd=GetForegroundWindow();
  dc= GetDC(wnd);
  GetClientRect(wnd,&rc);
  c=(char)(board[x][y]+'0');
  TextOut(dc, rc.right-15, 3, &c, 1);
  ReleaseDC(wnd,dc);
}

#endif
