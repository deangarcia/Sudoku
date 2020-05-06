#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Cells
{
	int value;
	int row;
	int column;
	int id;
	int * box;
	int * markup;
} **sudoku_board;

void allocate_board(int grid_size)
{
    sudoku_board = (struct Cells **) malloc(grid_size * sizeof(struct Cells *)); 
    
    int i;
    for(i=0; i < grid_size; i++) 
    {
         sudoku_board[i] = (struct Cells *)malloc(grid_size * sizeof(struct Cells)); 
    }
}

void deallocate_board(int grid_size)
{
    int i;
    for(i=0; i < grid_size; i++) 
    {
        free(sudoku_board[i]); 
    }
    free(sudoku_board);
}

void init_array(int grid_size)
{
	int i, j;
	int cell_id = 0;
	for(i = 0; i < grid_size; i++) 
	{
		for(j = 0; j < grid_size; j++) 
		{
			sudoku_board[i][j].value = 0;
			sudoku_board[i][j].row = i;
			sudoku_board[i][j].column = j;
			sudoku_board[i][j].id =cell_id++; 
			sudoku_board[i][j].markup = (int *)malloc(grid_size * sizeof(int));

			for(int k = 0; k < grid_size; k++)
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
int valid_cell_box(int row, int col, int n, int box_grid_size) 
{ 
	int i, j;

	// Force row and col values to be one of the row col values 
	// shown in the comments that way we beggining looping at the first
	// cell of the square.
	row = row - row % box_grid_size;
	col = col - col % box_grid_size;

	for(i = 0; i < box_grid_size; i++) 
	{
		for(j = 0; j < box_grid_size; j++) 
		{ 
			if(sudoku_board[i + row][j + col].value == n) 
			{
				return 0; // Return 0 on fail
			}
		}
	}
	return 1; // Return 1 on success
}

int valid_cell_row(int row, int n, int grid_size)
{
	int j;
	for (j = 0; j < grid_size; j++)
	{
		if (sudoku_board[row][j].value == n)
		{
			return 0;
		}  
    } 
	return 1;
}

int valid_cell_col(int col, int n, int grid_size)
{
	int i;
	for (i = 0; i < grid_size; i++)
	{
		if (sudoku_board[i][col].value == n)
		{
			return 0;
		}  
    } 
	return 1;
}

int valid_cell_placement(int row, int col, int n, int grid_size, int box_grid_size)
{
	if(valid_cell_col(col, n, grid_size) && valid_cell_row(row, n, grid_size) && valid_cell_box(row, col, n, box_grid_size))
	{
		return 1;
	}

	return 0;
}

// Function for inputting cell values in the diagnol boxes
// of a Sudoku board. This step is unique because it can all be done without
// having to validate cell input row and column wise. 
// TODO: Add OpenMP code to run this in parallel
void fill_diagonal_boxes(int grid_size, int box_grid_size)
{
	int i, j, k;
	for(i = 0; i < grid_size; i = i + box_grid_size)
	{
		int n; 
		for(j = 0; j < box_grid_size; j++) 
		{ 
			for(k = 0; k < box_grid_size; k++) 
			{ 
				do
				{ 
					n = rand() % grid_size + 1; 
				} 
				while(!valid_cell_box(i, i, n, box_grid_size)); 

				sudoku_board[j+i][k+i].value = n; 
			} 
		} 
	} 
}

// TODO: This implementation only works for a 9x9 grid need to redo with 
// logic that do 4x4 and 16x16
// row = 0 col = 0	 row = 0 col = 2
// (0, 0)(0, 1)      (0, 2)(0, 3)
// (1, 0)(1, 1)      (1, 2)(1, 3)
// row = 2 col = 0   row = 2 col = 2
// (2, 0)(2, 1)      (2, 2)(2, 3)
// (3, 0)(3, 1)      (3, 2)(3, 3)
// Also might need to do some recursion and backtracking for this because 
// creating it is basically like solving it currently taking to long with 
// random assignments while validating for row, column and box
int fill_remain_boxes(int grid_size, int box_grid_size) 
{ 

    // For Left Most
    int i, j, k;
	for(i = 0; i<grid_size; i = i + box_grid_size)
	{
		if(i == 0)
		{
			// Do nothing this box is full
		}
		else
		{
			int n; 
			for(j = 0; j < box_grid_size; j++) 
			{ 
				for(k = 0; k < box_grid_size; k++) 
				{ 
					do
					{ 
						n = rand() % grid_size + 1; 
					} 
					while(!valid_cell_placement(i, 0, n, grid_size, box_grid_size)); 
					sudoku_board[j+i][k].value = n; 
				} 
			} 
		}
	} 

	// For Middle Boxes
	for(i = 0; i < grid_size; i = i + box_grid_size)
	{
		if(i == 3)
		{
			// Do nothing this box is full
		}
		else
		{
			int n; 
			for(j = 0; j < box_grid_size; j++) 
			{ 
				for(k = 0; k < box_grid_size; k++) 
				{ 
					do
					{ 
						n = rand() % grid_size + 1; 
					} 
					while(!valid_cell_placement(i, 3, n, grid_size, box_grid_size)); 
					sudoku_board[j+i][k+3].value = n; 
				} 
			} 
		}
	} 

	// For Right Most Boxes
	for(i = 0; i < grid_size; i = i + box_grid_size)
	{
		if(i == 6)
		{
			// Do nothing this box is full
		}
		else
		{
			int n; 
			for(j = 0; j < box_grid_size; j++) 
			{ 
				for(k = 0; k < box_grid_size; k++) 
				{ 
					do
					{ 
						n = rand() % grid_size + 1; 
					} 
					while(!valid_cell_placement(i, 6, n, grid_size, box_grid_size));

					sudoku_board[j+i][k+6].value = n; 
				} 
			} 
		}
	} 
} 

void fill_values(int grid_size, int box_grid_size, int empty_cells)
{
    fill_diagonal_boxes(grid_size, box_grid_size);
    //fill_remain_boxes(grid_size, box_grid_size);
    //remove_cells();
}

int main()
{
    int i, j, k;
    int grid_size = 4;
    int box_grid_size = sqrt(grid_size);
    int empty_cells = 52;
    
    allocate_board(grid_size);
    init_array(grid_size);

    fill_values(grid_size, box_grid_size, empty_cells);
   
	for(i = 0; i < grid_size; i++) 
	{
		for(j = 0; j < grid_size; j++) 
		{
			printf("(%d, %d)", sudoku_board[i][j].row, sudoku_board[i][j].column);
		 	//printf(" %d", sudoku_board[i][j].value);
		}
		printf("\n");
	}
    
    deallocate_board(grid_size);

    return 0;
}