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
int runtime_threshold = 5;

double t_ser;

class Cell {
public:
	int value;
	int row;
	int col;
	int markup[grid_size + 1];
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
	int rows[grid_size][grid_size]; //rows[rowID][markup] markup = {0,0,1,1} -> 3,4 are valid entries
	int cols[grid_size][grid_size]; //cols[colID][markup] 
	int boxes[grid_size][grid_size]; //boxes[boxID][markup]
	Board() {
		for (int i = 0; i < grid_size; i++) {
			for (int j = 0; j < grid_size; j++) {
				Cell newCell(0, i, j);
				newCell.value = 0;

				for (int k = 0; k < grid_size + 1; k++) {
					//this is a bool array with 1 for valid and 0 for invalid
					//newCell.markup = {0,0,1,1} -> 3,4 are valid entries
					newCell.markup[k] = 1;
				}
				cells[i][j] = newCell;
				rows[i][j] = 1;
				cols[i][j] = 1;
				boxes[i][j] = 1;

			}
		}
	}
	int get_box_id(int row, int col) {
		int box_size = sqrt(grid_size);
		int box_row = row / box_size;
		int box_col = col / box_size;
		return box_col + box_row * box_size;
	}
	Cell getCell(int id) {
		int col = id % grid_size;
		int row = id / grid_size;

		return cells[col][row];
	}
	/*int* get_row_markup(int row) {
		return rows[row];
	}
	int* get_col_markup(int col) {
		return cols[col];
	}
	int* get_box_markup(int row, int col) {
		int box_id = get_box_id(row, col);
		return boxes[box_id];
	}*/
	Cell get_markup(int row, int col) {
		Cell cell = cells[col][row];
		//int markup[grid_size + 1];
		int box_id = get_box_id(row, col);
		int markup_size = 0;
		for (int i = 1; i < grid_size + 1; i++) {
			//printf("%d\n", i);
			if (rows[row][i - 1] == 1 && cols[col][i - 1] == 1 && boxes[box_id][i - 1] == 1 && cells[col][row].value == 0)
			{
				//markup[i] = 1;

				//cell.markup[i] = 1;
				cells[col][row].markup[i] = 1;
				markup_size++;
			}
			else {
				//cell.markup[i] = 0;
				cells[col][row].markup[i] = 0;
				//markup[i] = 0;
			}
		}
		//markup[0] = markup_size;
		cells[col][row].markup[0] = markup_size;
		return cells[col][row];
	}
	int get_kth(int* markup, int k) {
		int count = 0;
		int index = 0;
		while (count < k) {

			if (markup[index + 1] == 1) {
				count++;
			}
			index++;
		}
		return index;
	}

	void add_value(int value, int row, int col) {
		int box_id = get_box_id(row, col);
		//printf("%d\n", value);
		cells[col][row].value = value;
		rows[row][value - 1] = 0;
		cols[col][value - 1] = 0;
		boxes[box_id][value - 1] = 0;
	}

	void remove_value(int value, int row, int col) {
		int box_id = get_box_id(row, col);
		cells[col][row].value = 0;
		rows[row][value - 1] = 1;
		cols[col][value - 1] = 1;
		boxes[box_id][value - 1] = 1;
	}

	void generate_markup() {
		for (int i = 0; i < grid_size; i++) {
			for (int j = 0; j < grid_size; j++) {
				if (cells[i][j].value != 0) add_value(cells[i][j].value, j, i);
			}
		}
	}

	void check_forced_cells() {
		//generate_markup();
		int forced_count = 0;
		for (int i = 0; i < grid_size; i++) {
			for (int j = 0; j < grid_size; j++) {
				if (cells[j][i].value == 0 && get_markup(i, j).markup[0] == 1) {
					int value = get_kth(cells[j][i].markup, 1);
					add_value(value, i, j);
					forced_count++;
				}
			}
		}
		//if (forced_count > 0) check_forced_cells();
	}

};

void print_markup(Board board, int row, int col) {
	Cell markup = board.get_markup(row, col);
	printf("markup (%d, %d) ", col, row);
	for (int i = 0; i < grid_size + 1; i++) {
		printf("%d ", markup.markup[i]);
		//printf(" %d ", board.rows[row][i]);
	}
	printf(" | ");

	for (int i = 0; i < grid_size; i++) {
		//printf("%d ", markup.markup[i]);
		printf(" %d ", board.rows[row][i]);
	}
	printf(" | ");
	for (int i = 0; i < grid_size; i++) {
		//printf("%d ", markup.markup[i]);
		printf(" %d ", board.cols[col][i]);
	}
	printf(" | ");

	int box = board.get_box_id(row, col);
	printf(" %d - ", box);
	for (int i = 0; i < grid_size; i++) {
		//printf("%d ", markup.markup[i]);

		printf(" %d ", board.boxes[box][i]);
	}
	printf("\n");
}
void print_all_markup(Board board) {
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {
			print_markup(board, i, j);
		}
	}
	printf("\n");
}

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






//
//Cell getCell(Board &board, int id) {
//	int col = id % grid_size;
//	int row = id / grid_size;
//
//	return board.cells[col][row];
//}
//
//Cell getCell(Board &board, int row, int col) {
//	return board.cells[col][row];
//}

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

Board solved_board;
int solved = 0;

int fill_remain_cells(Board &board, int newk)
{



	int i, j;

	for (j = 0; j < grid_size; j++)
	{
		for (i = 0; i < grid_size; i++)
		{
			if (board.cells[i][j].value == 0)
			{

				for (int k = newk; k <= grid_size; k++)
				{
					if (solved) return 0;
					if (valid_cell_placement(board, j, i, k))
					{
						board.cells[i][j].value = k;
						//printBoard(board);
						if (fill_remain_cells(board, 1))
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
	if (!solved) {
		solved = 1;
		solved_board = board;
	}


	return 1;
}



int fill_cells_markup(Board &board)
{
	//board.check_forced_cells();
	int i, j, k;
	for (j = 0; j < grid_size; j++)
	{
		for (i = 0; i < grid_size; i++)
		{
			if (board.cells[i][j].value == 0)
			{
				Cell cell = board.get_markup(j, i);
				int markup_count = cell.markup[0];
				for (k = 1; k <= markup_count; k++)
				{
					int value = board.get_kth(cell.markup, k);

					//if (valid_cell_placement(board, j, i, value))
					//{
					board.add_value(value, j, i);
					//printBoard(board);
					//print_all_markup(board);


					if (fill_cells_markup(board))
					{
						return 1;
					}
					else
					{
						board.remove_value(board.cells[i][j].value, j, i);

						if (omp_get_wtime() - t_ser > runtime_threshold) return 0;
					}
					//}
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
	int valid_board = check_valid_board(board);
	printf("valid? %d\n", valid_board);
	if (valid_board == 1) {
		int count = 0;
		while (cell_count > clue_count) {
			int cellId = getRandomValue(0, grid_size * grid_size);
			Cell cell = board.getCell(cellId);

			if (board.cells[cell.col][cell.row].value != 0) {
				board.cells[cell.col][cell.row].value = 0;
				//board.remove_value(row, col);
				//printBoard(board);
				count++;
				//printf("count: %d\n", count);
				cell_count--;
			}
		}
	}
	else {
		printBoard(board);
		return generate_random_board(clue_count);
	}


	return board;
}

void brute_force_parallel(Board board) {
	omp_set_num_threads(grid_size);
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < grid_size; i++) {
			Board board1 = board;
			fill_remain_cells(board1, i + 1);
		}
	}
}


void run_experiment() {
	ofstream output;
	output.open("data.csv");
	output << "runID, size, clue_count, board, alg1Time, alg1Solution, alg1Valid, alg2Time, alg2Solution, alg2Valid, alg3Time, alg3Solution, alg3Valid \n";
	srand(omp_get_wtime());
	Board board1, board2, board3, board4;
	double elapsedTime;
	int valid_solution;
	for (int i = 0; i < 100; i++) {
		solved = 0;

		printf("run# %d", i + 1);
		output << i + 1 << "," << grid_size << "," << clue_count << ",";
		//srand(time(NULL));

		Board board = generate_random_board(clue_count);
		solved_board = board;
		printBoard(board, output);


		//brute force serial------------------------------------------------------------
		board1 = board;

		t_ser = omp_get_wtime();
		fill_remain_cells(board1);
		elapsedTime = omp_get_wtime() - t_ser;
		output << elapsedTime << ",";


		printBoard(board1, output);

		valid_solution = check_valid_board(board1);
		output << valid_solution << ",";


		//brute force parallel-------------------------------------------------------
		board2 = board;
		//printBoard(board2, output);

		t_ser = omp_get_wtime();

		brute_force_parallel(board2);
		elapsedTime = omp_get_wtime() - t_ser;
		output << elapsedTime << ",";


		printBoard(solved_board, output);

		valid_solution = check_valid_board(solved_board);
		if (!solved) valid_solution = -1;
		output << valid_solution << ",";

		//markup serial------------------------------------------------------------
		board3 = board;

		t_ser = omp_get_wtime();
		board3.generate_markup();
		fill_cells_markup(board3);
		elapsedTime = omp_get_wtime() - t_ser;
		output << elapsedTime << ",";


		printBoard(board3, output);

		valid_solution = check_valid_board(board3);
		output << valid_solution << ",";

		output << "\n";
	}
	output.close();
}





void testing() {

	t_ser = omp_get_wtime();
	srand(time(NULL));

	Board board1 = generate_random_board(clue_count);


	/*Board board1;
	fill_values(board1);*/
	printBoard(board1);
	board1.generate_markup();
	board1.check_forced_cells();
	print_all_markup(board1);
	fill_cells_markup(board1);
	//int valid_board = ;
	printf("valid? %d\n", check_valid_board(board1));
	printBoard(board1);
	print_all_markup(board1);
}

int* res() {
	//#pragma omp parallel
	//	{
	//		res();
	//	}
	int test[3]; // = { 8,9,3 };;
	for (int i = 0; i < 3; i++) {
		test[i] = i;
	}
	return test;
}

int main()
{
	printf("-----------------------------------------test begins----------------------------------------------\n");

	run_experiment();
	//testing();

	/*int* test = res();
	for (int i = 0; i < 3; i++) {

		printf("%d ", test[0 ]);
	}*/


	// 	omp_set_num_threads(8);
	// #pragma omp parallel
	// 	{
	// 		int tid = omp_get_thread_num();
	// 		printf("%d\n", tid);
	// 	}

	return 0;
}