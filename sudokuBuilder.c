// SudokuBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"
#include <time.h>


int const grid_size = 4;

struct Cells
{
	int value;
	int row;
	int column;
	int id;
	int * box;
	int * markup;
}
**sudoku_board;
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
					newCell.markup[k] = 1;
				}
				cells[i][j] = newCell;
			}
		}
	}
};

void printBoard(int grid_size) {
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			//printf("(%d, %d)", sudoku_board[i][j].row, sudoku_board[i][j].column);
			int val = sudoku_board[i][j].value;
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
void printBoard(Board &board) {
	printf("\n");
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			//printf("(%d, %d)", sudoku_board[i][j].row, sudoku_board[i][j].column);
			int val = board.cells[j][i].value;
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
void allocate_board(int grid_size)
{
	sudoku_board = (struct Cells **) malloc(grid_size * sizeof(struct Cells *));

	int i;
	for (i = 0; i < grid_size; i++)
	{
		sudoku_board[i] = (struct Cells *)malloc(grid_size * sizeof(struct Cells));
	}
}

void deallocate_board(int grid_size)
{
	int i;
	for (i = 0; i < grid_size; i++)
	{
		free(sudoku_board[i]);
	}
	free(sudoku_board);
}

void init_array(int grid_size)
{
	int i, j;
	int cell_id = 0;
	for (i = 0; i < grid_size; i++)
	{
		for (j = 0; j < grid_size; j++)
		{
			sudoku_board[i][j].value = 0;
			sudoku_board[i][j].row = i;
			sudoku_board[i][j].column = j;
			sudoku_board[i][j].id = cell_id++;
			sudoku_board[i][j].markup = (int *)malloc(grid_size * sizeof(int));

			for (int k = 0; k < grid_size; k++)
			{
				sudoku_board[i][j].markup[k] = 0;
			}
		}
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
//int valid_cell_box(int row, int col, int n, int box_grid_size)
//{
//	int i, j;
//
//	// Force row and col values to be one of the row col values 
//	// shown in the comments that way we beggining looping at the first
//	// cell of the square.
//	row = row - row % box_grid_size;
//	col = col - col % box_grid_size;
//
//	for (i = 0; i < box_grid_size; i++)
//	{
//		for (j = 0; j < box_grid_size; j++)
//		{
//			if (sudoku_board[i + row][j + col].value == n)
//			{
//				return 0; // Return 0 on fail
//			}
//		}
//	}
//	return 1; // Return 1 on success
//}

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
			if (board.cells[i + col][j + row].value == n)
			{
				return 0; // Return 0 on fail
			}
		}
	}
	return 1; // Return 1 on success
}

//int valid_cell_row(int row, int n, int grid_size)
//{
//	int j;
//	for (j = 0; j < grid_size; j++)
//	{
//		if (sudoku_board[row][j].value == n)
//		{
//			return 0;
//		}
//	}
//	return 1;
//}
int valid_cell_row(Board board, int row, int n)
{
	int j;
	for (j = 0; j < grid_size; j++)
	{
		if (board.cells[j][row].value == n)
		{
			return 0;
		}
	}
	return 1;
}

//int valid_cell_col(int col, int n, int grid_size)
//{
//	int i;
//	for (i = 0; i < grid_size; i++)
//	{
//		if (sudoku_board[i][col].value == n)
//		{
//			return 0;
//		}
//	}
//	return 1;
//}

int valid_cell_col(Board board, int col, int n)
{
	int i;
	for (i = 0; i < grid_size; i++)
	{
		if (board.cells[col][i].value == n)
		{
			return 0;
		}
	}
	return 1;
}

//int valid_cell_placement(int row, int col, int n, int grid_size, int box_grid_size)
//{
//	if (valid_cell_col(col, n, grid_size) && valid_cell_row(row, n, grid_size) && valid_cell_box(row, col, n, box_grid_size))
//	{
//		return 1;
//	}
//
//	return 0;
//}

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

// Function for inputting cell values in the diagnol boxes
// of a Sudoku board. This step is unique because it can all be done without
// having to validate cell input row and column wise. 
// TODO: Add OpenMP code to run this in parallel
//void fill_diagonal_boxes(int grid_size, int box_grid_size)
//{
//	int i, j, k;
//	for (i = 0; i < grid_size; i = i + box_grid_size)
//	{
//		int n;
//		for (j = 0; j < box_grid_size; j++)
//		{
//			for (k = 0; k < box_grid_size; k++)
//			{
//				do
//				{
//					n = rand() % grid_size + 1;
//				} while (!valid_cell_box(i, i, n, box_grid_size));
//
//				sudoku_board[j + i][k + i].value = n;
//			}
//		}
//	}
//}

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

Board solve_board_bruteforce2(Board board, int row, int col)
{
	Board bCopy = board;
	for (int j = row; j < grid_size; j++) {
		for (int i = col; i < grid_size; i++) {
			if (board.cells[i][j].value == 0) {
				int value = 1;
				while (value <= grid_size) {
					int valid = valid_cell_placement(board, j, i, value);
					if (valid == 1) {
						bCopy.cells[i][j].value = value;
						printBoard(bCopy);
						if (i == grid_size - 1 && j == grid_size - 1) {
							//check valid board
							if (check_valid_board(bCopy) == -1) {
								bCopy.cells[0][0].value = -1;
							}
							return bCopy;
						}
						else {
							int nextCol = i;
							int nextRow = j;
							if (i < grid_size - 1) {
								nextCol++;
							}
							else if (j < grid_size - 1) {
								nextCol = 0;
								nextRow++;
							}
							printf("%d %d\n", nextCol, nextRow);
							Board temp = solve_board_bruteforce2(bCopy, nextRow, nextCol);
							if (temp.cells[0][0].value != -1) return temp;
						}
					}
					value++;
					if (value > grid_size && valid != 1) {
						bCopy.cells[0][0].value = -1;
						return bCopy;
					}
				}
			}
		}
	}
	//check valid board
	int valid = check_valid_board(bCopy);
	if (valid == -1) {
		bCopy.cells[0][0].value = valid;
	}
	return bCopy;
}
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

Board brute_force(Board board) {
	//(struct Cells *)malloc(grid_size * sizeof(struct Cells));
	int *values = (int*)malloc(grid_size*grid_size * sizeof(int));
	for (int i = 0; i < grid_size*grid_size; i++) {
		int col = i % grid_size;
		int row = i / grid_size;
		if (board.cells[col][row].value != 0) {
			values[i] = -1;
		}
		else {
			values[i] = 1;
		}
	}

	int done = 0;
	while (done != 1) {


		done = 1;
		for (int i = 0; i < grid_size*grid_size; i++) {
			if (values[i] < grid_size && values[i] > 0) done = 0;
		}
	}
}


Cell getCell(Board &board, int id) {
	int col = id % grid_size;
	int row = id / grid_size;

	return board.cells[col][row];
}

int getRandomValue(int min, int max) {

	return rand() % max + min;
}


void generate_random_board(Board &board, int clue_count) {
	//srand(time(NULL));
	while (clue_count > 0) {
		int cellId = getRandomValue(0, grid_size * grid_size);
		Cell cell = getCell(board, cellId);

		if (cell.value == 0) {
			int randVal = getRandomValue(1, grid_size);
			if (valid_cell_placement(board, cell.row, cell.col, randVal) == 1) {
				cell.value = randVal;
				board.cells[cell.row][cell.col].value = randVal;
				//printf("%d %d \n",cell.value, sudoku_board[cell.row][cell.column]);
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
	//int i, j, k;
	//int grid_size = 9;
	int box_grid_size = sqrt(grid_size);
	int clue_count = 4;

	//allocate_board(grid_size);
	//init_array(grid_size);

	//fill_values(grid_size, box_grid_size, clue_count);

	//printBoard(grid_size);

	//deallocate_board(grid_size);

	Board board1;
	Board board2 = board1;
	//board1.cells[5][7].value = 3;
	generate_random_board(board2, clue_count);
	srand(time(NULL));

	generate_random_board(board1, clue_count);
	printBoard(board1);
	printf("\n");
	printBoard(board2);

	board2 = solve_board_bruteforce2(board2, 0, 0);
	printf("\n");
	printBoard(board2);

	omp_set_num_threads(8);
#pragma omp parallel
	{
		int tid = omp_get_thread_num();
		printf("%d\n", tid);
	}

	return 0;
}
