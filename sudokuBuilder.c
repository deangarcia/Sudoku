// SudokuBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"
#include <time.h>
#include <fstream>
//#include <iostream>

using namespace std;


int const grid_size = 9;
int clue_count = 17;
int runtime_threshold = 1000;

double t_ser;

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
					//this is a bool array with 1 for valid and 0 for invalid
					//newCell.markup = {0,0,1,1} -> 3,4 are valid entries
					newCell.markup[k] = 1;
				}
				cells[i][j] = newCell;
			}
		}
	}
};

void printBoard(Board &board) {
	printf("\n");
	int digits = 0;
	int mag = 1;
	while (mag < grid_size) {
		digits++;
		mag *= 10;
	}

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
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

void printBoard(Board &board, ofstream &output) {
	//output << ",";
	int digits = 0;
	int mag = 1;
	while (mag < grid_size) {
		digits++;
		mag *= 10;
	}

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			int val = board.cells[j][i].value;
			if (val == 0) {
				output << " _";
			}
			else {
				output << " " << val;
			}

		}
		output << " |";
	}
	output << ",";
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
	int box_row = row - row % box_grid_size;
	int box_col = col - col % box_grid_size;

	for (i = 0; i < box_grid_size; i++)
	{
		for (j = 0; j < box_grid_size; j++)
		{
			int cell_row = i + box_row;
			int cell_col = j + box_col;
			if (board.cells[cell_col][cell_row].value == n && !(cell_row == row && cell_col == col))
			{
				return 0; // Return 0 on fail
			}
		}
	}
	return 1; // Return 1 on success
}

int valid_cell_row(Board board, int col, int row, int n)
{
	int j;
	for (j = 0; j < grid_size; j++)
	{
		if (board.cells[j][row].value == n && col != j)
		{
			return 0;
		}
	}
	return 1;
}

int valid_cell_col(Board board, int col, int row, int n)
{
	int i;
	for (i = 0; i < grid_size; i++)
	{
		if (board.cells[col][i].value == n && row != i)
		{
			return 0;
		}
	}
	return 1;
}

int valid_cell_placement(Board board, int row, int col, int n)
{
	int valid = 0;
	int box_valid = valid_cell_box(board, row, col, n);
	int row_valid = valid_cell_row(board, col, row, n);
	int col_valid = valid_cell_col(board, col, row, n);
	if (box_valid && row_valid && col_valid)
	{
		valid = 1;
	}
	//printf("valid? %d (%d, %d) %d %d %d\n", n, col, row, box_valid, col_valid, row_valid);
	return valid;
}

int check_valid_board(Board board) {
	int valid = 1;
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
			//int temp = valid_cell_placement(board, j, i, board.cells[i][j].value);
			if (!valid_cell_placement(board, j, i, board.cells[i][j].value)) valid = -1;
		}
	}
	return valid;
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




Cell getCell(Board &board, int id) {
	int col = id % grid_size;
	int row = id / grid_size;

	return board.cells[col][row];
}

Cell getCell(Board &board, int row, int col) {
	return board.cells[col][row];
}

int getRandomValue(int min, int max) {

	return rand() % max + min;
}

// Need this so we can safely put random numbers in the diagnol boxes
// without needing to worry about row and column. Random numbers are important because 
// we put random numbers in the diagnal when we call the fill remaining cells function which 
// runs iteratively starting from cell 0,0 and ending on cell 8,8 it will adjust to these random placements
// sort of like seeding with the rand function.
// if we dont have this random placement then our first row will always be 1 2 3 4 5 6 7 8 9
// we cant make the fill remaining cells function place random numbers in cells either because if we did
// we could be repeating value placement attempts and it would significantly slow down our algorithm. 
void fill_diagonal_boxes(Board &board)
{
	int box_grid_size = sqrt(grid_size);
	int i, j, k;
	for (i = 0; i < grid_size; i = i + box_grid_size)
	{
		int n;
		for (j = 0; j < box_grid_size; j++)
		{
			for (k = 0; k < box_grid_size; k++)
			{
				do
				{
					n = rand() % grid_size + 1;
				} while (!valid_cell_box(board, k + i, i + j, n));

				board.cells[i + j][k + i].value = n;
			}
		}
	}
}

void fill_diagonal_box(Board &board)
{
	int box_grid_size = sqrt(grid_size);
	int i, j, k;
	i = 0;

	int n;
	for (j = 0; j < box_grid_size; j++)
	{
		for (k = 0; k < box_grid_size; k++)
		{
			do
			{
				n = rand() % grid_size + 1;
			} while (!valid_cell_box(board, k + i, i + j, n));

			board.cells[i + j][k + i].value = n;
		}
	}


}

// Brute force/final step to generating a valid board function
int fill_remain_cells(Board &board)
{
	int i, j, k;
	for (j = 0; j < grid_size; j++)
	{
		for (i = 0; i < grid_size; i++)
		{
			if (board.cells[i][j].value == 0)
			{
				for (k = 1; k <= grid_size; k++)
				{
					if (valid_cell_placement(board, j, i, k))
					{
						board.cells[i][j].value = k;
						//printBoard(board);
						if (fill_remain_cells(board))
						{
							return 1;
						}
						else
						{
							board.cells[i][j].value = 0;
							if (omp_get_wtime() - t_ser > runtime_threshold) return 0;
						}
					}
				}
				return 0;
			}
		}
	}
	return 1;
}



// Was thinking about just stopping mid way through the puzzle generation
// but you have to completely fill in the board before you can validate
// it, this is why instead I go back and randomly remove cell values.
void fill_values(Board &board)
{
	//fill_diagonal_boxes(board);
	fill_diagonal_box(board);
	fill_remain_cells(board);
}

Board generate_random_board(int clue_count) {
	Board board;
	t_ser = omp_get_wtime();
	int cell_count = grid_size * grid_size;
	fill_values(board);
	printf("valid? %d\n", check_valid_board(board));
	int count = 0;
	while (cell_count > clue_count) {
		int cellId = getRandomValue(0, grid_size * grid_size);
		Cell cell = getCell(board, cellId);

		if (board.cells[cell.col][cell.row].value != 0) {
			board.cells[cell.col][cell.row].value = 0;
			//printBoard(board);
			count++;
			//printf("count: %d\n", count);
			cell_count--;
		}
	}

	return board;
}


void run_experiment() {
	ofstream output;
	output.open("data.csv");
	output << "runID, size, clue_count, board, alg1Time, alg1Solution, alg1Valid \n";
	srand(omp_get_wtime());
	for (int i = 0; i < 100; i++) {
		printf("run# %d", i + 1);
		output << i + 1 << "," << grid_size << "," << clue_count << ",";
		//srand(time(NULL));

		Board board = generate_random_board(clue_count);
		printBoard(board, output);
		t_ser = omp_get_wtime();
		fill_remain_cells(board);
		double elapsedTime = omp_get_wtime() - t_ser;
		output << elapsedTime << ",";


		printBoard(board, output);

		int valid_solution = check_valid_board(board);
		output << valid_solution << ",";
		output << "\n";
	}
	output.close();
}

int main()
{
	printf("-----------------------------------------test begins----------------------------------------------\n");

	run_experiment();

	//int box_grid_size = sqrt(grid_size);
	//int clue_count = 52;

	//srand(time(NULL));

	//Board board1 = generate_random_board(clue_count);
	//Board board2 = board1;
	//fill_remain_cells(board2);
	//
	//
	//
	//
	//printBoard(board1);
	////printf("valid? %d", check_valid_board(board1));
	//printf("\n");
	//printBoard(board2);
	//printf("valid? %d", check_valid_board(board2));

	// 	omp_set_num_threads(8);
	// #pragma omp parallel
	// 	{
	// 		int tid = omp_get_thread_num();
	// 		printf("%d\n", tid);
	// 	}

	return 0;
}