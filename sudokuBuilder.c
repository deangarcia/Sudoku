// SudokuBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "omp.h"
#include <time.h>


int const grid_size = 9;

class Cell {
public:
	int value;
	int row;
	int col;
	int markup[grid_size];
	Cell() {

	}
	Cell(int Value, int Row, int Col) {
		value = Value;
		row = Row;
		col = Col;
	}
};

class Board {
public:
	Cell cells[grid_size][grid_size];
	Board() {
		for (int i = 0; i < grid_size; i++) {
			for (int j = 0; j < grid_size; j++) {
				Cell newCell(0, i, j);
				newCell.value = 0;

				for (int k = 0; k < grid_size; k++) {
					// Why are we assigning to one shouldn't it be 0 the markups 
					// thinking: The markup array contains all numbers that cant go in the associated cell right?
					//           so why is 1 right off the back being eliminated from all new cells.
					newCell.markup[k] = 1;
				}
				cells[i][j] = newCell;
			}
		}
	}
};

void printBoard(Board &board) {
	printf("\n");
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			int val = board.cells[i][j].value;
			if (val == 0) {
				printf(" _");
			}
			else {
				printf(" %d", val);
			}

		}
		printf("\n");
	}
}

// Function for validating a cells value content. For example 
// Can not have more than 1 occurence {1,2,3,4,5,6,7,8,9} in a box
// the set of numbers that can be repeated is determined by the board grid size
// or sqrt(number of cells)
// row = 0 col = 0     row = 0 col = 3     row = 0 col = 6
// (0, 0)(0, 1)(0, 2)  (0, 3)(0, 4)(0, 5)  (0, 6)(0, 7)(0, 8)                                                                  
// (1, 0)(1, 1)(1, 2)  (1, 3)(1, 4)(1, 5)  (1, 6)(1, 7)(1, 8)                                                                  
// (2, 0)(2, 1)(2, 2)  (2, 3)(2, 4)(2, 5)  (2, 6)(2, 7)(2, 8)   
// row = 3 col = 0	   row = 3 col = 3     row = 3 col = 6
// (3, 0)(3, 1)(3, 2)  (3, 3)(3, 4)(3, 5)  (3, 6)(3, 7)(3, 8)                                                                  
// (4, 0)(4, 1)(4, 2)  (4, 3)(4, 4)(4, 5)  (4, 6)(4, 7)(4, 8)                                                                  
// (5, 0)(5, 1)(5, 2)  (5, 3)(5, 4)(5, 5)  (5, 6)(5, 7)(5, 8)
// row = 6 col = 0     row = 6 col = 3     row = 6 col = 6                                                                
// (6, 0)(6, 1)(6, 2)  (6, 3)(6, 4)(6, 5)  (6, 6)(6, 7)(6, 8)                                                                  
// (7, 0)(7, 1)(7, 2)  (7, 3)(7, 4)(7, 5)  (7, 6)(7, 7)(7, 8)                                                                  
// (8, 0)(8, 1)(8, 2)  (8, 3)(8, 4)(8, 5)  (8, 6)(8, 7)(8, 8)
int valid_cell_box(Board board, int row, int col, int n)
{
	int i, j;
	int box_grid_size = sqrt(grid_size);
	// Force row and col values to be one of the row col values 
	// shown in the comments that way we beggining looping at the first
	// cell of the square.
	row = row - row % box_grid_size;
	col = col - col % box_grid_size;

	for (i = 0; i < box_grid_size; i++)
	{
		for (j = 0; j < box_grid_size; j++)
		{
			if (board.cells[i + row][j + col].value == n)
			{
				return 0; // Return 0 on fail
			}
		}
	}
	return 1; // Return 1 on success
}

int valid_cell_row(Board board, int row, int n)
{
	int j;
	for (j = 0; j < grid_size; j++)
	{
		if (board.cells[row][j].value == n)
		{
			return 0;
		}
	}
	return 1;
}

int valid_cell_col(Board board, int col, int n)
{
	int i;
	for (i = 0; i < grid_size; i++)
	{
		if (board.cells[i][col].value == n)
		{
			return 0;
		}
	}
	return 1;
}

int valid_cell_placement(Board board, int row, int col, int n)
{
	int valid = 0;
	if (valid_cell_col(board, col, n) && valid_cell_row(board, row, n) && valid_cell_box(board, row, col, n))
	{
		valid = 1;
	}
	printf("valid?%d %d %d %d\n", valid, col, row, n);
	return valid;
}

int check_valid_board(Board board) {
	int valid = 1;
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
			int temp = valid_cell_placement(board, j, i, board.cells[i][j].value);
			if (temp == 0) valid = -1;
		}
	}
	return valid;
}

// Board solve_board_bruteforce2(Board board, int row, int col)
// {
// 	Board bCopy = board;
// 	for (int j = row; j < grid_size; j++) {
// 		for (int i = col; i < grid_size; i++) {
// 			if (board.cells[i][j].value == 0) {
// 				int value = 1;
// 				while (value <= grid_size) {
// 					int valid = valid_cell_placement(board, j, i, value);
// 					if (valid == 1) {
// 						bCopy.cells[i][j].value = value;
// 						printBoard(bCopy);
// 						if (i == grid_size - 1 && j == grid_size - 1) {
// 							//check valid board
// 							if (check_valid_board(bCopy) == -1) {
// 								bCopy.cells[0][0].value = -1;
// 							}
// 							return bCopy;
// 						}
// 						else {
// 							int nextCol = i;
// 							int nextRow = j;
// 							if (i < grid_size - 1) {
// 								nextCol++;
// 							}
// 							else if (j < grid_size - 1) {
// 								nextCol = 0;
// 								nextRow++;
// 							}
// 							printf("%d %d\n", nextCol, nextRow);
// 							Board temp = solve_board_bruteforce2(bCopy, nextRow, nextCol);
// 							if (temp.cells[0][0].value != -1) return temp;
// 						}
// 					}
// 					value++;
// 					if (value > grid_size && valid != 1) {
// 						bCopy.cells[0][0].value = -1;
// 						return bCopy;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	//check valid board
// 	int valid = check_valid_board(bCopy);
// 	if (valid == -1) {
// 		bCopy.cells[0][0].value = valid;
// 	}
// 	return bCopy;
// }

int* add_to_values(int values[]) {
	int done = 0;
	int index = grid_size * grid_size - 1;
	while (done != 1) {

		if (values[index] == -1) {
			index++;
		}
		else if (values[index] < grid_size - 1) {
			values[index]++;
			return values;
		}
		else {
			values[index] = 1;
			index++;
		}

	}
	return values;
}

// Board brute_force(Board board) {

// 	int *values = (int*)malloc(grid_size*grid_size * sizeof(int));
// 	for (int i = 0; i < grid_size*grid_size; i++) {
// 		int col = i % grid_size;
// 		int row = i / grid_size;
// 		if (board.cells[col][row].value != 0) {
// 			values[i] = -1;
// 		}
// 		else {
// 			values[i] = 1;
// 		}
// 	}

// 	int done = 0;
// 	while (done != 1) {


// 		done = 1;
// 		for (int i = 0; i < grid_size*grid_size; i++) {
// 			if (values[i] < grid_size && values[i] > 0) done = 0;
// 		}
// 	}
// }


Cell getCell(Board &board, int id) {
	int col = id % grid_size;
	int row = id / grid_size;

	return board.cells[row][col];
}

int getRandomValue(int min, int max) {

	return rand() % max + min;
}

void generate_random_board(Board &board, int clue_count) {
	while (clue_count > 0) {
		int cellId = getRandomValue(0, grid_size * grid_size);
		Cell cell = getCell(board, cellId);

		if (cell.value == 0) {
			int randVal = getRandomValue(1, grid_size);
			if (valid_cell_placement(board, cell.row, cell.col, randVal) == 1) {
				cell.value = randVal;
				board.cells[cell.row][cell.col].value = randVal;
				clue_count -= 1;
			}

		}
	}
}

void fill_values(Board &board, int clue_count)
{
	generate_random_board(board, clue_count);
}

int main()
{
	int box_grid_size = sqrt(grid_size);
	int clue_count = 52;

	srand(time(NULL));

	Board board1;
	Board board2 = board1;
	generate_random_board(board2, clue_count);
	srand(time(NULL));

	generate_random_board(board1, clue_count);
	printBoard(board1);
	printf("\n");
	printBoard(board2);

//	board2 = solve_board_bruteforce2(board2, 0, 0);
	printf("\n");
	printBoard(board2);

// 	omp_set_num_threads(8);
// #pragma omp parallel
// 	{
// 		int tid = omp_get_thread_num();
// 		printf("%d\n", tid);
// 	}

	return 0;
}
