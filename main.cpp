/*****************************************
Program written on 11/11/2023
Basic single-player TicTacToe game vs computer
3 levels of difficulty: easy, hard, insane
*******************************************/

#include <iostream>
#include <array>
#include <unordered_set>
#include <ctype.h>
#include <cstdlib>

using namespace std;

#define SIZE 9	// 3x3 board = 9

void new_board(array<char, SIZE>& b);
void new_emptySlots(unordered_set<int>& e_s);
void print_board(const array<char, SIZE>& b);
int get_userInput(const unordered_set<int>& empty_slots, const array<char, SIZE>& board);
void print_bad_input();
void update_board(array<char, SIZE>& board, unordered_set<int>& empty_slots, int choice, char symbol);
void tictactoe(array<char, SIZE>& board, unordered_set<int>& empty_slots, char difficulty);
void computer_turn(array<char, SIZE>& board, unordered_set<int>& empty_slots, char difficulty);
bool easy_mode(array<char, SIZE>& board, unordered_set<int>& empty_slots);
bool hard_mode(array<char, SIZE>& board, unordered_set<int>& empty_slots);
bool insane_mode(array<char, SIZE>& board, unordered_set<int>& empty_slots); 
bool defend_user_center_trick(array<char, SIZE>& board, unordered_set<int>& empty_slots);
void choose_corner(array<char, SIZE>& board, unordered_set<int>& empty_slots, int choice);
bool defend_user_diagonal_trick(array<char, SIZE>& board, unordered_set<int>& empty_slots);
bool check_cornerR(array<char, SIZE>& board, int slot, int next_slot, int count, char symbol);
bool middle_is_open(array<char, SIZE>& board, unordered_set<int>& empty_slots);
bool check_if_won(const array<char, SIZE>& board, char player_symbol);
//bool check_if_wonI(const array<char, SIZE>& board, char symbol, int i1, int i2, int i3);
bool check_three_in_row(const array<char, SIZE>& board, char symbol, int slot, int next_slot, int count);
int check_one_away(const array<char, SIZE>& board, char symbol);
int check_one_awayR(const array<char, SIZE>& board, int slot, int next_slot, int count, char symbol);
int check_gap(const array<char, SIZE>& board, char symbol);
int check_gapR(const array<char, SIZE>& board, int slot, int next_slot, char symbol);


bool check_tie(const unordered_set<int>& empty_slots, const array<char, SIZE>& b);
char choose_difficulty();
bool first_move();

int main() {
	array<char, SIZE> board;	// 3x3 tictactoe board using array class
	unordered_set<int> empty_slots;	// set that keeps track of empty slots

	while (true) {
		char mode = choose_difficulty();	// gets char, will determine easy, hard, insane, or q to quit
		if (mode == 'q')	// q means quit
			break;
		else {
			// must clear board and set of empty slots to prepare for new game
			new_board(board);	// SIZE is 9, index 0 - 8 will have values 1 - 9 to start
			new_emptySlots(empty_slots);	// SIZE is 9, empty slots will have 1 - 9 to start
		}
		if (!first_move())	//	// if user declines first move, !(false) == true, computer gets first move
			computer_turn(board, empty_slots, mode);

		tictactoe(board, empty_slots, mode);	// function plays one game of tictactoe per call
	}

	cout << "\nThank you for playing!" << endl;

	return 0;
}

void new_board(array<char, SIZE>& b) {
	for (int i = 0; i < SIZE; i++) {
		// index 0 - (SIZE - 1) will have values 1 - SIZE to start
		// SIZE is 9 so board will be 3 x 3 with index 0 - 8
		b.at(i) = i + 49;	// +48 to convert from int to char, +1 to account for array indexing, so + 49
	}
}

void new_emptySlots(unordered_set<int>& e_s) {
	for (int i = 1; i <= SIZE; i++) {
		// set will have values 1 - SIZE to start
		e_s.insert(i);	// SIZE is 9 so set will have 1 - 9
	}
}

void print_board(const array<char, SIZE>& b) {
	cout << " _  _  _" << endl;	// top "roof" of board
	for (int row = 0; row < sqrt(SIZE); row++) {	// SIZE / 3 = 3
		for (int col = 0; col < sqrt(SIZE); col++) {	//	SIZE / 3 = 3
			cout << "|" << b.at((row * sqrt(SIZE)) + col) << "|";	// digit if slot not taken, X or O if taken
		}
		cout << endl;	// newline for each new row
	}
	cout << endl;
}

int get_userInput(const unordered_set<int>& empty_slots, const array<char, SIZE>& board) {
	string str;	// read user input
	int inp = -1;	// want to return int value for simplicity
	do {		// board will show numbers for empty slots
		cout << "Enter a number for an empty slot to mark X: " << endl;
		cin >> str;	// have to read string to test isdigit on first char 

		if (isdigit(str[0]) && str.length() == 1)	// if user entered 1 char and 1st char == digit
			inp = str[0] - 48;	// convert ch to int by ASCII table trick
		else {	// if user entered 1 > char or 1st char != digit (DeMorgans Law)
			print_bad_input();
			print_board(board);
			continue;	// know input is bad so no need to execute to next part of loop
		}
		// if number exists in set of empty slots, break and return good input
		if (empty_slots.count(inp) > 0)		// count == 1 if inp is found in set
			break;
		else
			print_bad_input();
	} while (true);	// should always be true, loops breaks when good input entered 
	/* if user is stupid and cannot figure out how to input current digit, then they
	* probably have bigger problems than not being able to play TicTacToe.
	*/
	return inp;	// return choice
}

void print_bad_input() {
	cout << "Bad input!" << endl << endl;
}

void update_board(array<char, SIZE>& board, unordered_set<int>& empty_slots, int choice, char symbol) {
	if (symbol == 'O')
		cout << "Computer chose slot " << choice << "." << endl;

	board.at(choice - 1) = symbol;	// choice - 1 to account for index 0 - 8 but set has 1 - 9
	empty_slots.erase(choice);	// since slot is now filled with X or O, remove from set of empty slots
	print_board(board);
}

void tictactoe(array<char, SIZE>& board, unordered_set<int>& empty_slots, char difficulty) {
	if (empty_slots.size() == 9)
		print_board(board);	// special case when game starts, if new, empty board with 1 - 9 digits

	while (true) {	// breaks only when winner is found, or board is full --> tied / won on last open slot
		// user's turn, will choose a slot to fill with 'X'
		update_board(board, empty_slots, get_userInput(empty_slots, board), 'X');
		if (check_if_won(board, 'X')) {	// if 3 in a row found, check_if_won == true, user wins
			cout << "You got 3 in a row and won, good job!" << endl;
			return;	// exit function, game has ended
		}
		if (check_tie(empty_slots, board))	// check if board is full but no winner
			break;	// game tied, exit loop
		// computer's turn, randomly choose slot to fill with 'O'
		computer_turn(board, empty_slots, difficulty);	// update board is called in function
		if (check_if_won(board, 'O')) {	// if 3 in a row found, game_over == true, computer win
			cout << "The computer got 3 in a row, you lost lol." << endl;
			return;	// exit function, game has ended
		}
		if (check_tie(empty_slots, board))	// check if board is full but no winner
			break;	// game tied, exit loop
	}	// while loop
}

void computer_turn(array<char, SIZE>& board, unordered_set<int>& empty_slots, char difficulty) {
	/* easy mode: will check if computer can win (has two O's in a row, or a gap ie O 5 O) and will update board to win
	hard mode: will check if user can win (has two X's in a row, or a gap ie X 5 X) and will update board to block user from winning 
	insane mode: will 
	*/

	// will go over easy mode to see if computer can win, then hard mode to block user from winning, then insane to check 
	// any tricks the user may try, such as double attack or something like that.
	if (easy_mode(board, empty_slots))
		return;	// return to tictactoe function, we're done here

	if (difficulty == 'e')	// if on easy mode
		goto skip_difficulty;	// must skip hard and insane mode

	if (hard_mode(board, empty_slots))
		return;	// return to tictactoe function, we're done here

	if (difficulty == 'h')	// if on hard mode
		goto skip_difficulty;	// must skip insane mode

	if (insane_mode(board, empty_slots))
		return;	// return to tictactoe function, we're done here

	// if we get to this rand num generator part, then user is not one slot away from winning and no tricks, safe to use rng
	// also likely we are on easy mode, unless all the checks above weren't satisfied for hard or insane mode
	skip_difficulty:
	int randNum = 0;
	srand(time(0));	// seed
	while (empty_slots.count(randNum) == 0) {	// randNum not found in set of empty slots
		randNum = rand() % SIZE + 1;	// yields rng 1 - 9, will keep generating until num found in set
	}
	// update board to reflect computer's choice, with an O
	update_board(board, empty_slots, randNum, 'O');
}

bool easy_mode(array<char, SIZE>& board, unordered_set<int>& empty_slots) {
	int entry = check_one_away(board, 'O');	// check if computer has two 'O' in a row and can win
	if (entry >= 0) {	// if not -1, entry == slot that must be chosen by computer to win
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, entry, 'O');	// win game if found two 'O' in a row and open 3rd slot
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}

	entry = check_gap(board, 'O');	// another function to check if computer is one away from winning
	if (entry >= 0) {	// if not -1, entry == slot that must be chosen by computer to win
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, entry, 'O');	// block user if 2 'X' in a row and open slot in center
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}

	return false;	// lets computer_turn know this function didn't change anything, must continue
}

bool hard_mode(array<char, SIZE>& board, unordered_set<int>& empty_slots) {
	int entry = check_one_away(board, 'X');	// now check if user is one away from winning, if so, block user
	if (entry >= 0) {	// if not -1, entry == slot that must be chosen by computer to block user from winning
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, entry, 'O');	// block user if 2 'X' in a row and open slot
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}
	else {	// There are no 2 'X' in a row with a slot open for user to win
		entry = check_gap(board, 'X');	// another guard function to check if user is one away from winning
		if (entry >= 0) {
			// update board to reflect computer's choice, with an O
			update_board(board, empty_slots, entry, 'O');	// block user if 2 'X' in a row and open slot in center
			return true;	// lets computer_turn know to skip any further execution since we just updated board
		}
	}

	return false;	// lets computer_turn know this function didn't change anything, must continue
}
bool insane_mode(array<char, SIZE>& board, unordered_set<int>& empty_slots) {
	// user has taken center on first move
	if (defend_user_center_trick(board, empty_slots))	// check comments in function for explanation
		return true;	// lets computer_turn know to skip any further execution since we just updated board

	// there is X X O on a diagonal, must guard against taking non-corner or user can use double attack trick
	if (defend_user_diagonal_trick(board, empty_slots))	// check comments in function for explanation
		return true;	// lets computer_turn know to skip any further execution since we just updated board

	// user has taken a corner on first move, so computer will take center to stop any double attack tricks
	if (middle_is_open(board, empty_slots))	// check comments in function for explanation
		return true;	// lets computer_turn know to skip any further execution since we just updated board

	
	return false;	// lets computer_turn know this function didn't change anything, must continue
}

bool defend_user_center_trick(array<char, SIZE>& board, unordered_set<int>& empty_slots) {
	// user has gone first and taken center
	// must take corner, any corner. if take edge that's not corner, user can take another corner and double attack.
	/* EXAMPLE:
	* 1 2 3   1 2 3   X 2 3   X 2 3   X 2 X
	* 4 X 6   O X 6   O X 6   O X 6   O X 6
	* 7 8 9   7 8 9   7 8 9   7 8 O   7 8 O
	*
	* As you can see, if user takes center first and computer takes edge that's not corner, user can win
	* by double attack if they take another corner and computer is forced to defend. Then user
	* can make a triangle 2-corner center double attack that attacks two ways.
	*/

	// make computer take corner if user took center on first move
	if (board.at(4) == 'X' && empty_slots.size() == 8) {
		int randNum = 0;
		srand(time(0));	// seed
		randNum = rand() % 4;	// yields rng 0 - 3
		choose_corner(board, empty_slots, randNum);	// determines which of the 4 corners computer will choose
		return true;
	}
	else
		return false;	// lets insane_mode know this function didn't change anything, must continue
}

void choose_corner(array<char, SIZE>& board, unordered_set<int>& empty_slots, int choice) {
	if (choice == 0) {
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, 1, 'O');
		return;		// executed defense against user trick
	}
	if (choice == 1) {
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, 3, 'O');
		return;		// executed defense against user trick
	}
	if (choice == 2) {
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, 7, 'O');
		return;		// executed defense against user trick
	}
	if (choice == 3) {
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, 9, 'O');
		return;		// executed defense against user trick
	}
}

bool defend_user_diagonal_trick(array<char, SIZE>& board, unordered_set<int>& empty_slots) {
	// sequence 7 5 2 4 6 1 9 3 8
	// 1 2 3   1 2 3   1 X 3   1 X 3   1 X 3   O X 3  
	// 4 5 6   4 O 6   4 O 6   X O 6   X O X   X O X
	// X 8 9   X 8 9   X 8 9   X 8 9   X 8 9   X 8 9  now user takes 9 for double attack, this is a flaw
	// user has gone first and taken a corner
	// must take corner, any corner. if take edge that's not corner, user can take another corner and double attack.
	/* EXAMPLE:
	* 1 2 3   1 2 3   1 2 3   1 2 O   1 2 O
	* 4 5 6   O 5 6   O X 6   O X 6   O X 6
	* X 8 9   X 8 9   X 8 9   X 8 9   X 8 X
	*
	* Another thing to note is if user lets computer go first, then 6 9 4 5 1 7 is double attack
	* 
	* As you can see, if user takes corner first and computer takes edge that's not corner, user can win
	* by double attack if they take another corner and computer is forced to defend. Then user
	* can make a triangle 2-corner center double attack that attacks two ways.
	*/

	// special case of top left slot, will check diagonal tl to mm to see if br must be blocked
	if (check_cornerR(board, 0, sqrt(SIZE) + 1, 0, 'X')) {	// diagonal tl mm is X, br is O
		update_board(board, empty_slots, 7, 'O');
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}
	// special case of top right slot, will check diagonal tr to mm to see if bl must be blocked
	if (check_cornerR(board, sqrt(SIZE) - 1, sqrt(SIZE) - 1, 0, 'X')) {	// diagonal tr to mm, bl is O
		update_board(board, empty_slots, 1, 'O');
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}
	// special case of bottom left slot, will check diagonal bl to mm to see if tr must be blocked
	if (check_cornerR(board, SIZE - sqrt(SIZE), -(sqrt(SIZE) - 1), 0, 'X')) {	// diagonal bl to mm, tr is O
		update_board(board, empty_slots, 9, 'O');
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}
	// special case of bottom right slot, will check diagonal br to mm to see if tl must be blocked
	if (check_cornerR(board, SIZE - 1, -(sqrt(SIZE) + 1), 0, 'X')) {	// diagonal br to mm, tl is O
		update_board(board, empty_slots, 3, 'O');
		return true;	// lets computer_turn know to skip any further execution since we just updated board
	}

	return false;	// lets insane_mode know this function didn't change anything, must continue
}

bool check_cornerR(array<char, SIZE>& board, int slot, int next_slot, int count, char symbol) {
	// if count reaches this high and 3rd slot on diagonal == 'O', return true
	if (count == sqrt(SIZE) - 1 && board.at(slot) == 'O')	// if there is X X O on diagonal, return true
		return true;

	if (board.at(slot) == symbol)	// symbol found, go to next slot
		return check_cornerR(board, slot + next_slot, next_slot, count + 1, symbol);
	else	// if there is not X X O on diagonal, return false
		return false;
}

bool middle_is_open(array<char, SIZE>& board, unordered_set<int>& empty_slots) {
	// must always take center if user leaves it open on first move. if take center after user doesn't, 
	// doesn't matter if user takes corner or edge, no double attack possible for now if computer takes center.
		/* EXAMPLE:
		* X X O
		* O O X
		* X X O
		*
		* As you can see, if user doesn't take center first and computer takes center, no matter where user goes next,
		* it doesn't matter for now, assuming this is insane mode which it should be.
		*/

		// make computer take center if user didn't take center first move
	if (empty_slots.size() == 8 && isdigit(board.at(5))) {	// only 1 user input and center is open
		// update board to reflect computer's choice, with an O
		update_board(board, empty_slots, 5, 'O');	// will take center
		return true;	// this function was necessary
	}
	else
		return false;	// this function was unnecessary
}

bool check_if_won(const array<char, SIZE>& board, char player_symbol) {
	/* will return true if there are any 3 in a row X or O, false otherwise
			 recursively calls check_three_in_row to determine if starting index has 3 symbols in a row.
			 4th parameter of check_three_in_row function is the offset. Assume board is 3x3
			 Example: going left to right: from top left to top middle would be +1, then to top right +1.
			 Diagonal offset is 4 top left to bottom right, or -2 bottom left to top right
			 horizontal offset is 1, and vertical offset is 3

			KEY: l = left, r = right, m = middle, t = top, b = bottom	ie tr = top right
	*/

	// special case of top left slot, will check diagonal tl to br
	if (check_three_in_row(board, player_symbol, 0, sqrt(SIZE) + 1, 0))	// diagonal tl to br
		return true;
	// special case of top right slot, will check diagonal tr to bl
	if (check_three_in_row(board, player_symbol, sqrt(SIZE) - 1, sqrt(SIZE) - 1, 0))	// diagonal tl to br
		return true;

	// now check every vertical possibility for each top row slot, will check down from tl, tm, tr
	for (int i = 0; i < sqrt(SIZE); i++) {
		if (check_three_in_row(board, player_symbol, i, sqrt(SIZE), 0))	// 4th param is offset down one slot
			return true;
	}
	// now check every horizontal possibility for each left column slot, will check right from tl, ml, bl
	for (int i = 0; i < SIZE; i += sqrt(SIZE)) {	// i + sqrt(SIZE) each iteration to go down one slot
		if (check_three_in_row(board, player_symbol, i, 1, 0))	// 4th param is offset right one slot
			return true;
	}

	return false;	// if above code doesn't return true, there is no winning in a row combo
}

//bool check_if_wonI(const array<char, SIZE>& board, char symbol, int i1, int i2, int i3) {
//	 https://www.youtube.com/watch?v=0zGcUoRlhmw
//	 symbol will be X for user or O for computer
//	return (board.at(i1) == symbol && board.at(i2) == symbol && board.at(i3) == symbol);
//}

bool check_three_in_row(const array<char, SIZE>& board, char symbol, int slot, int next_slot, int count) {
	// if count has reached row/col/diagonal length without previous calls returning false. Means 3 in a row found.
	if (count == sqrt(SIZE))	// sqrt(SIZE) always == row/col/diagonal length
		return true;

	if (board.at(slot) == symbol)	// symbol found, go to next slot
		return check_three_in_row(board, symbol, slot + next_slot, next_slot, count + 1);
	else	// board.at(slot) != symbol, cannot be 3 in a row
		return false;
}

int check_one_away(const array<char, SIZE>& board, char symbol) {
	// if slot_number is ever >= 0, it means user is one away from winning if they enter that number
	int slot_number = -1;	

	// special case of top left slot, will check diagonal tl to mm to see if br must be blocked
	slot_number = check_one_awayR(board, 0, sqrt(SIZE) + 1, 0, symbol);	// diagonal tl mm
	if (slot_number >= 0) { return slot_number; }
	// special case of top right slot, will check diagonal tr to mm to see if bl must be blocked
	slot_number = check_one_awayR(board, sqrt(SIZE) - 1, sqrt(SIZE) - 1, 0, symbol);	// diagonal tr to mm
	if (slot_number >= 0) { return slot_number; }
	// special case of bottom left slot, will check diagonal bl to mm to see if tr must be blocked
	slot_number = check_one_awayR(board, SIZE - sqrt(SIZE), -(sqrt(SIZE) - 1), 0, symbol);	// diagonal bl to mm
	if (slot_number >= 0) { return slot_number; }
	// special case of bottom right slot, will check diagonal br to mm to see if tl must be blocked
	slot_number = check_one_awayR(board, SIZE - 1, -(sqrt(SIZE) + 1), 0, symbol);	// diagonal br to mm
	if (slot_number >= 0) { return slot_number; }

	// now check every vertical possibility for each top row slot, will check down from tl, tm, tr
	for (int i = 0; i < sqrt(SIZE); i++) {
		slot_number = check_one_awayR(board, i, sqrt(SIZE), 0, symbol);	// 3rd param is offset down one slot
		if (slot_number >= 0) { return slot_number; }
	}
	// now check every vertical possibility for each bottom row slot, will check up from bl, bm, br
	for (int i = SIZE - sqrt(SIZE); i < SIZE; i++) {
		slot_number = check_one_awayR(board, i, -(sqrt(SIZE)), 0, symbol);	// 3rd param is offset up one slot
		if (slot_number >= 0) { return slot_number; }
	}
	// now check every horizontal possibility for each left column slot, will check right from tl, ml, bl
	for (int i = 0; i < SIZE; i += sqrt(SIZE)) {	// i + sqrt(SIZE) each iteration to go down one slot
		slot_number = check_one_awayR(board, i, 1, 0, symbol);	// 3rd param is offset right one slot
		if (slot_number >= 0) { return slot_number; }
	}
	// now check every horizontal possibility for each right column slot, will check left from tr, mr, br
	for (int i = sqrt(SIZE) - 1; i < SIZE; i += sqrt(SIZE)) {	// i + sqrt(SIZE) each iteration to go down one slot
		slot_number = check_one_awayR(board, i, -1, 0, symbol);	// 3rd param is offset left one slot
		if (slot_number >= 0) { return slot_number; }
	}

	return -1;	// if above code doesn't return slot_number, user is not one entry away from winning
}

int check_one_awayR(const array<char, SIZE>& board, int slot, int next_slot, int count, char symbol) {
	// if count reaches this high, user / computer is one entry away from winning, return if slot is digit, meaning open slot
	if (count == sqrt(SIZE) - 1 && isdigit(board.at(slot)))	
		return slot + 1;	// return slot number to block user / computer

	if (board.at(slot) == symbol)	// symbol found, go to next slot
		return check_one_awayR(board, slot + next_slot, next_slot, count + 1, symbol);
	else	// board.at(slot) != symbol, user / computer cannot be one entry away from winning
		return -1;	// if user / computer is not one away from winning, return -1 
}

int check_gap(const array<char, SIZE>& board, char symbol) {
	// if slot_number is ever >= 0, it means user is one away from winning if they enter that number
	int slot_number = -1;

	/* if there is a gap that the check*_user_one_away function did not find, such as
	X
	4
	X
	then check for that and return 4 so computer will block user from winning */
	
	// top left slot, will check diagonal tl to br to see if mm must be blocked
	slot_number = check_gapR(board, 0, 4, symbol);	// diagonal tl br
	if (slot_number >= 0) { return slot_number - 48; }	// - 48 to account for char to int
	// top right slot, will check diagonal tr to bl to see if mm must be blocked
	slot_number = check_gapR(board, 2, 2, symbol);
	if (slot_number >= 0) { return slot_number - 48; }	// -48 to account for char to int
	// top left slot, will check horizontal tl to tr to see if tm must be blocked
	slot_number = check_gapR(board, 0, 1, symbol);	// horizontal tl to tr
	if (slot_number >= 0) { return slot_number - 48; }	// -48 to account for char to int
	// middle left slot, will check horizontal ml to mr to see if mm must be blocked
	slot_number = check_gapR(board, 3, 1, symbol);	// horizontal ml to mr
	if (slot_number >= 0) { return slot_number - 48; }	// - 48 to account for char to int
	// bottom left slot, will check horizontal bl to br to see if bm must be blocked
	slot_number = check_gapR(board, 6, 1, symbol);	// horizontal tl to tr
	if (slot_number >= 0) { return slot_number - 48; }	// - 48 to account for char to int
	// top left slot, will check vertical tl to bl to see if ml must be blocked
	slot_number = check_gapR(board, 0, 3, symbol);	// vertical tl to bl
	if (slot_number >= 0) { return slot_number - 48; }	// - 48 to account for char to int
	// top middle slot, will check vertical tm to bm to see if mm must be blocked
	slot_number = check_gapR(board, 1, 3, symbol);	// vertical tm to bm
	if (slot_number >= 0) { return slot_number - 48; }	// - 48 to account for char to int
	// top right slot, will check vertical tr to br to see if ml must be blocked
	slot_number = check_gapR(board, 2, 3, symbol);	// vertical tr to br
	if (slot_number >= 0) { return slot_number - 48; }	// - 48 to account for char to int

	return -1;	// if above code doesn't return slot_number, user is not one entry away from winning
}

int check_gapR(const array<char, SIZE>& board, int slot, int next_slot, char symbol) {
	// checks for a blank slot between two 'X' or 'O' to see if one away from winning
	if (board.at(slot) == symbol && isdigit(board.at(slot + next_slot)) && board.at(slot + 2 * next_slot) == symbol)
		return board.at(slot + next_slot);	// return slot number of board to block user / computer from winning
	else	// There was no case of symbol blank symbol horizontally, vertically, or diagonally
		return -1;
}

bool check_tie(const unordered_set<int>& empty_slots, const array<char, SIZE>& b) {
	if (empty_slots.size() == 0) {	// if set is empty, board is full with no winner, return true
		cout << "Game was a tie." << endl;
		return true;	// return true so calling function knows if tie
	}
	else
		return false;	// if not tie, return false
}

char choose_difficulty() {
	string str;
	cout << endl;

	do {
		cout << "Difficulty: choose e for easy, h for hard, i for insane, or q to quit:" << endl;	// new game difficulty?
		cin >> str;
		if (str.length() == 1) {	// only one char needed, length must be 1
			if (str[0] == 'e' || str[0] == 'h' || str[0] == 'i')
				return str[0];	// if e, h, i then return that char
			else if (str[0] == 'q')	// if q return q
				return str[0];
			else	// anything else is bad input by default
				print_bad_input();
		}
		else	// if string > 1 or < 1
			print_bad_input();
	} while (true);
	// this line should never be reached by the program, only return within loop can break out
}

bool first_move() {
	string str;
	cout << endl;

	do {
		cout << "Make first move? y or n:" << endl;	// new game?
		cin >> str;
		if (str.length() == 1) {	// y or n only one char needed, length must be 1
			if (str[0] == 'y' || str[0] == 'Y')	// if y or Y return true
				return true;
			else if (str[0] == 'n' || str[0] == 'N')	// if n or N return false
				return false;
			else	// anything else is bad input by default
				print_bad_input();
		}
		else	// if string > 1 or < 1
			print_bad_input();
	} while (true);
	// this line should never be reached by the program, only return within loop can break out
}

/* Left backdoors open for insane so it's not impossible. 
Computer goes first: If computer takes non-corner edge (8),
user takes middle. Then if computer takes parallel non-corner edge, in this case 2, user can take corner (9).
Computer must block (1). User must block (3). Computer must block both (7) and (6). User wins.
User goes first: User takes non-corner edge (6). Computer takes non-corner edge (8). User takes corner (7). 
Computer takes non-corner edge (4). User takes corner (3), double attack. Computer takes corner (9). User takes 5 and wins.
*/