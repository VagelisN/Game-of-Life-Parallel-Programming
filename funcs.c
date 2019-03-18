#include <stdio.h>
#include <unistd.h>
#include "funcs.h"

int calculate_neighbours_inside(int** current, int j, int k, int arraysize)
{
	int neighbours=0;
	if(current[j-1][k-1]==1)neighbours++;
	if(current[j-1][k]==1)neighbours++;
	if(current[j-1][k+1]==1)neighbours++;
	if(current[j][k-1]==1)neighbours++;
	if(current[j][k+1]==1)neighbours++;
	if(current[j+1][k]==1)neighbours++;
	if(current[j+1][k+1]==1)neighbours++;
	if(current[j+1][k-1]==1)neighbours++;
	return neighbours;
}

int calculate_neighbours_nsew(char nsew, int* row_col , int **current,int arraysize,int j)
{
	int neighbours=0;
	if(nsew == 'e')
	{
			neighbours=0;
			if(current[j-1][arraysize-1]==1)neighbours++;//n
			if(current[j-1][arraysize-2]==1)neighbours++;//nw
			if(current[j][arraysize-2]==1)neighbours++;//w
			if(current[j+1][arraysize-2]==1)neighbours++;//sw
			if(current[j+1][arraysize-1]==1)neighbours++;//s
			if(row_col[j+1]==1)neighbours++;//se
			if(row_col[j]==1)neighbours++;//e
			if(row_col[j-1]==1)neighbours++;//ne
	}
	else if(nsew == 'w')
	{
			if(current[j-1][0]==1)neighbours++;
			if(current[j-1][1]==1)neighbours++;
			if(current[j][1]==1)neighbours++;
			if(current[j+1][1]==1)neighbours++;
			if(current[j+1][0]==1)neighbours++;
			if(row_col[j+1]==1)neighbours++;
			if(row_col[j]==1)neighbours++;
			if(row_col[j-1]==1)neighbours++;
	}
	else if(nsew == 'n')
	{
			if(current[0][j-1]==1)neighbours++;//west
			if(current[1][j-1]==1)neighbours++;//sw
			if(current[1][j]==1)neighbours++;//south
			if(current[1][j+1]==1)neighbours++;//se
			if(current[0][j+1]==1)neighbours++;//east
			if(row_col[j+1]==1)neighbours++;//ne
			if(row_col[j]==1)neighbours++;//n
			if(row_col[j-1]==1)neighbours++;//nw
	}
	else if(nsew == 's')
	{
			if(current[arraysize-1][j-1]==1)neighbours++;//west
			if(current[arraysize-2][j-1]==1)neighbours++;//nw
			if(current[arraysize-2][j]==1)neighbours++;//n
			if(current[arraysize-2][j+1]==1)neighbours++;//ne
			if(current[arraysize-1][j+1]==1)neighbours++;//east
			if(row_col[j+1]==1)neighbours++;//se
			if(row_col[j]==1)neighbours++;//s
			if(row_col[j-1]==1)neighbours++;//sw
	}
	return neighbours;
}

int calculate_neighbours_corners(int corner,int *arr1 ,int* arr2,int dead_or_alive ,int **current,int arraysize,int j)
{

	int neighbours=0;
	if(corner == 0)
	{   //calculate the top right element
		if(current[0][arraysize-2]==1)neighbours++;//west
		if(current[1][arraysize-2]==1)neighbours++;//sw
		if(current[1][arraysize-1]==1)neighbours++;//s
		if(arr1[1] == 1)neighbours++;//se
		if(arr1[0] == 1)neighbours++;//east
		if(dead_or_alive == 1)neighbours++;//ne
		if(arr2[arraysize-1] == 1)neighbours++;//n
		if(arr2[arraysize-2]==1)neighbours++;//nw
	}
	else if(corner == 1)
	{//calculate the top left element
		if(current[0][1]==1)neighbours++;//east
		if(current[1][1]==1)neighbours++;//se
		if(current[1][0]==1)neighbours++;//s
		if(arr1[1] == 1)neighbours++;//sw
		if(arr1[0] == 1)neighbours++;//west
		if(dead_or_alive == 1)neighbours++;//nw
		if(arr2[0] == 1)neighbours++;//n
		if(arr2[1]==1)neighbours++;//ne
	}
	else if(corner == 2)
	{//calculate the bottom right element
		if(current[arraysize-2][arraysize-1]==1)neighbours++;//north
		if(current[arraysize-2][arraysize-2]==1)neighbours++;//nw
		if(current[arraysize-1][arraysize-2]==1)neighbours++;//w
		if(arr1[arraysize-2] == 1)neighbours++;//sw
		if(arr1[arraysize-1] == 1)neighbours++;//south
		if(dead_or_alive == 1)neighbours++;//se
		if(arr2[arraysize-1] == 1)neighbours++;//e
		if(arr2[arraysize-2]==1)neighbours++;//ne
	}
	else if(corner == 3)
	{
		if(current[arraysize-2][0]==1)neighbours++;//north
		if(current[arraysize-2][1]==1)neighbours++;//ne
		if(current[arraysize-1][1]==1)neighbours++;//e
		if(arr1[1] == 1)neighbours++;//se
		if(arr1[0] == 1)neighbours++;//south
		if(dead_or_alive == 1)neighbours++;//sw
		if(arr2[arraysize-1] == 1)neighbours++;//w
		if(arr2[arraysize-2]==1)neighbours++;//nw
	}
	return neighbours;
}

int next_state(int neighbours,int dead_or_alive)
{
	if(dead_or_alive==0){if(neighbours==3)return 1;else return 0;}
	else
	{
		if(neighbours < 2)return 0;
		else if(neighbours >= 4)return 0;
		else return 1;
	}
}

void print_state(int** current ,int rank,int arraysize, int generation)
{
		int k,j; 
		sleep(rank+1);
		printf("Next generation %d is:My rank: %d\n",generation,rank);
		for (k = 0; k < arraysize; ++k)
		{
			for (j = 0; j < arraysize; ++j)
			{
				printf("%d ",current[k][j]);
			}
			printf("\n");
		}
}