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
    for (i=0; i < grid_size; i++) 
    {
         sudoku_board[i] = (struct Cells *)malloc(grid_size * sizeof(struct Cells)); 
    }
}

void deallocate_board(int grid_size)
{
    int i;
    for (i=0; i < grid_size; i++) 
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
        sudoku_board[i][j].id =cell_id++; 
        sudoku_board[i][j].box = (int *)malloc(grid_size * sizeof(int));
        sudoku_board[i][j].markup = (int *)malloc(grid_size * sizeof(int));
        for(int k = 0; k < grid_size; k++)
        {
            sudoku_board[i][j].markup[k] = 0;
            sudoku_board[i][j].box[k] = 0;
        }
      }
    }
}

int valid_num_box(int row, int col, int n, int box_grid_size) 
{ 
  int i, j;
  for (int i = 0; i < box_grid_size; i++) 
  {
    for (int j = 0; j < box_grid_size; j++) 
    {
      if (sudoku_board[i + row][j + col].value == n) 
      {
        return 0; 
      }
    }
  }

  return 1; 
} 

void fill_diagonal_boxes(int grid_size, int box_grid_size)
{
  int i, j, k;
  for(int i = 0; i<grid_size; i=i+box_grid_size)
  {
    int n; 
    for (int j=0; j<box_grid_size; j++) 
    { 
      for (int k=0; k<box_grid_size; k++) 
      { 
          do
          { 
              n = rand() % 9 + 1; 
          } 
          while (!valid_num_box(i, i, n, box_grid_size)); 

          sudoku_board[j+i][k+i].value = n; 
      } 
    } 
  } 
}

void fill_values(int grid_size, int box_grid_size, int empty_cells)
{
    fill_diagonal_boxes(grid_size, box_grid_size);
    //fill_remain_boxes();
    //remove_cells();
}

int main()
{
    int i, j, k;
    int grid_size = 9;
    int box_grid_size = sqrt(grid_size);
    int empty_cells = 52;
    
    allocate_board(grid_size);
    init_array(grid_size);

    fill_values(grid_size, box_grid_size, empty_cells);
   
    for (i = 0; i < grid_size; i++) 
    {
      for (j = 0; j < grid_size; j++) 
      {
         printf(" %d", sudoku_board[i][j].value);
      }
      printf("\n");
    }
    
    deallocate_board(grid_size);

    return 0;
}