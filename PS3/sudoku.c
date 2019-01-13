/*
 *Alissa Greaney & Johnathan Soto
 *Problem Set 3
 *Determines whether a sudoku matrix is a valid sudoku using threads to check each row, column, and subgrid
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
void * checkRows(void *);
void * checkCols(void *);
void * checkGrid(void *);

//given to each thread to be able to access the grid and specific row/column they are checking
typedef struct Data{
	int row;
	int col;
	int (*grid)[9];
} Data;

int main()
{
	//initialize and read in matrix
	int grid[9][9] = {0};
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			scanf("%d",&grid[row][col]);
		}
	}
	pthread_t *thread_handles = (pthread_t*) malloc(27 * sizeof(pthread_t));
	long thread_id = 0;
	//initialize struct data for threads 
	Data * dataR = (Data*) malloc (sizeof(Data));
	Data * dataC = (Data *) malloc (sizeof(Data));
	Data * dataG = (Data *) malloc (sizeof(Data));
	//create threads for each row, column, and subgrid 
	for (int i = 0; i < 9; i++)
	{

		//Rows
		dataR->row = i;
		dataR->col = 0;
		dataR->grid = grid;
		pthread_create(&thread_handles[thread_id++],NULL,checkRows, dataR);
	
		//Columns
		dataC->row = 0;
		dataC->col = i;
		dataC->grid = grid;
		pthread_create(&thread_handles[thread_id++],NULL,checkCols, dataC);
		
		//Subgrids
		//starting row and column at top left of subgrid 
		dataG->row = (int)(i/3) *3;
		dataG->col = (i%3) * 3;
		dataG->grid = grid;
		pthread_create(&thread_handles[thread_id++],NULL,checkGrid, dataG);
		
	}
	//join threads
	int *y; //threads return value
	int rerr = 0; //used to only output first errors found
	int cerr = 0;
	int gerr = 0;
    	for (thread_id = 0; thread_id < 27; thread_id++) {
        	pthread_join(thread_handles[thread_id], (void*)&y);
		if(rerr != 1 || cerr != 1 || gerr !=1)
		{
			//if the thread returned an error
			if(y == (void*) 0)
			{
				int g = (int) thread_id/3;
				if(thread_id % 3 == 0 && rerr == 0)
				{
					printf("Row %d has an error\n",g+1);
					rerr++;
				}
				if(thread_id % 3 == 1 && cerr == 0)
				{
					printf("Column %d has an error\n", g+1);
					cerr++;
				}
				if(thread_id % 3 == 2 && gerr == 0)
				{
					if(g == 0)
						printf("Top left subgrid has an error\n");
					if(g == 1)
						printf("Top middle subgrid has an error\n");
					if(g == 2)
						printf("Top right subgrid has an error\n");
					if(g == 3)
						printf("Middle left subgrid has an error\n");
					if(g == 4)
						printf("Center subgrid has an error\n");
					if(g == 5)
						printf("Middle right subgrid has an error\n");
					if(g == 6)
						printf("Bottom left subgrid has an error\n");
					if(g == 7)
						printf("Bottom middle subgrid has an error\n");
					if(g == 8)
						printf("Bottom right subgrid has an error\n");
					gerr++;
				}
			}
		}
    	}
	if(cerr == 0 && rerr == 0 && gerr == 0)
		printf("The sudoku input is valid\n");
	else
		printf("The sudoku input is not valid\n");
	free(thread_handles);
	free(dataC);
	free(dataR);
	free(dataG);
	return 0;
}

/*
 * Check if the row is valid based on given row in Data struct 
 * Mark in an array if the number appears - returns 0 if the number has already appeared)
 */
void* checkRows (void * data)
{
	int good[10] = {0};
	Data * d = (Data *)data;
	for(int i = 0; i < 9; i++)
	{
		if(good[d->grid[d->row][i]] == 1)
		{
			return (void *)0;
		}
		else
		{
			good[d->grid[d->row][i]] = 1;
		}
	}
	return (void *)1;
}

/*
 * Check if the column is valid based on given column in Data struct 
 * Mark in an array if the number appears - returns 0 if the number has already appeared)
 */
void* checkCols (void * data)
{
	int good[10] = {0};

	Data * d = (Data *)data;
	for(int i = 0; i < 9; i++)
	{
		if(good[d->grid[i][d->col]] == 1)
		{
			return (void *)0;
		}
		else
		{
			good[d->grid[i][d->col]] = 1;
		}
	}
	return (void *) 1;
}

/*
 * Check if the subgrid is valid based on given starting row and column in Data struct 
 * Mark in an array if the number appears - returns 0 if the number has already appeared)
 */
void* checkGrid (void * data)
{
	//row and column are upper left of grid to look in
	int good[10] = {0};
	Data * d = (Data *)data;
	int rowMax = d->row + 3;
	int colMax = d->col + 3;
	for(int i = d->row; i < rowMax; i++)
	{
		for(int j = d->col; j < colMax; j++)
		{
			if(good[d->grid[i][j]] == 1)
			{
				return (void *)0;
			}
			else
			{
				good[d->grid[i][j]] = 1;
			}
		}
	}
	return (void*) 1;
}

