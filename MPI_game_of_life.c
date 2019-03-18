#include "mpi.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "funcs.h"



int main(int argc, char  *argv[])
{
	char c;
	double local_start,local_finish,local_elapsed,elapsed;
	int dead_count=0,no_change_count=0,termination_flag=1, check = 51,all_dead_no_change[2];
	int generations = 50, arraysize = 50, cart_size_x = -1,cart_size_y= -1, neighbours=0;
	int rank, temprank, numprocs, j, k, i;
	MPI_Datatype column;
	MPI_Comm  old_comm, new_comm;
	MPI_Status status,statusr;
 	MPI_Request sendN, sendS, sendE, sendW, sendNE, sendNW, sendSE, sendSW;
 	MPI_Request recvN, recvS, recvE, recvW, recvNE, recvNW, recvSE, recvSW;
	MPI_Init(&argc,&argv);//MPI_initializations
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (i = 0; i < numprocs ; ++i){if(i*i == numprocs){cart_size_x = i;cart_size_y=i; break;}}
	if(cart_size_x==-1)//processes cant be square rooted
	{
		for (i = 5; i > 1; --i)//divide processes in a N*M array
		{
			if(numprocs%i==0 && numprocs/i !=1){cart_size_x=i;cart_size_y=numprocs/i;break;}
		}
	}
	if(cart_size_x==-1){fprintf(stderr, "Number of processes given is not acceptable!\n");exit(1);}
	while((c = getopt (argc, argv, "g:s:")) != -1)
	{
		switch(c)
		{
			case 'g':
				generations = atoi(optarg);
				break;
			case 's':
				arraysize = atoi(optarg);
				break;
			default:
				break;
		}		
	}
	int coords[2], tempcoords[2];
	int ndims, reorder, ierr;
	int dim_size[2], periods[2];
	old_comm = MPI_COMM_WORLD;
	ndims = 2;
	dim_size[0] = cart_size_x;
	dim_size[1] = cart_size_y;
	periods[0] = 1;
	periods[1] = 1;
	reorder = 1;
 	MPI_Cart_create(old_comm, ndims, dim_size, periods, reorder, &new_comm);//topology for the given number of processes 
	MPI_Type_vector(arraysize, 1, arraysize, MPI_INT, &column);//vector to send columns 
	MPI_Type_commit(&column);
	srand(time(NULL) + rank);

	//Initialization of arrays
	int *temp=malloc(2*numprocs*sizeof(int));
	int **temparray = NULL;
	int *curdata =malloc(arraysize * arraysize * sizeof(int));
	int **current = calloc(arraysize , sizeof(int*));
	int *nextdata =malloc(arraysize * arraysize * sizeof(int));
	int **next = calloc(arraysize , sizeof(int*));
	int *north = malloc(arraysize * sizeof(int));
	int *south = malloc(arraysize * sizeof(int));
	int *east = malloc(arraysize * sizeof(int));
	int *west = malloc(arraysize * sizeof(int));
	int ne=-1,nw=-1,se=-1,sw=-1;
	for (j = 0; j < arraysize; ++j)
	{
		current[j]= &(curdata[j*arraysize]);
		next[j] = &(nextdata[j*arraysize]);
	}
	for (i = 0; i < arraysize; ++i)
	{
		for (j = 0; j < arraysize; ++j)
		{
			current[i][j] = rand()%2;
			next[i][j] = 0;
		}
	}

	//Find my coords in new_comm
	MPI_Cart_coords(new_comm, rank,2,coords);

	//Initialize requests (send-recv inits)
	tempcoords[0] = coords[0]-1;// North-West
	tempcoords[1] = coords[1]-1;
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[0][0], 1, MPI_INT, temprank, 24, new_comm, &sendNW);
	MPI_Recv_init(&nw, 1, MPI_INT, temprank, 25, new_comm, &recvNW);

	tempcoords[0] = coords[0]-1;// North-East
	tempcoords[1] = coords[1]+1;
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[0][arraysize-1], 1, MPI_INT, temprank, 26, new_comm, &sendNE);
	MPI_Recv_init(&ne, 1, MPI_INT, temprank, 27, new_comm, &recvNE);

	tempcoords[0] = coords[0]+1;// South-West
	tempcoords[1] = coords[1]-1;
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[arraysize-1][0], 1, MPI_INT, temprank, 27, new_comm, &sendSW);
	MPI_Recv_init(&sw, 1, MPI_INT, temprank, 26, new_comm, &recvSW);

	tempcoords[0] = coords[0]+1;// South-East
	tempcoords[1] = coords[1]+1;
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[arraysize-1][arraysize-1], 1, MPI_INT, temprank, 25, new_comm, &sendSE);
	MPI_Recv_init(&se, 1, MPI_INT, temprank, 24, new_comm, &recvSE);

	tempcoords[0] = coords[0]-1;// North
	tempcoords[1] = coords[1];
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[0][0], arraysize, MPI_INT, temprank, 21, new_comm, &sendN);
	MPI_Recv_init(&north[0], arraysize, MPI_INT, temprank, 20, new_comm, &recvN);

	tempcoords[0] = coords[0]+1;// South
	tempcoords[1] = coords[1];
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[arraysize-1][0], arraysize, MPI_INT, temprank, 20, new_comm, &sendS);
	MPI_Recv_init(&south[0], arraysize, MPI_INT, temprank, 21, new_comm, &recvS);
	

	tempcoords[0] = coords[0];// West
	tempcoords[1] = coords[1]-1;
	MPI_Cart_rank( new_comm, tempcoords, &temprank);
	MPI_Send_init(&current[0][0], 1, column, temprank, 22, new_comm, &sendW);
	MPI_Recv_init(&west[0], arraysize, MPI_INT, temprank, 23, new_comm, &recvW);

	tempcoords[0] = coords[0];// East
	tempcoords[1] = coords[1]+1;
	MPI_Cart_rank( new_comm, tempcoords, &temprank);

	MPI_Send_init(&current[0][arraysize-1], 1, column, temprank, 23, new_comm, &sendE);
	MPI_Recv_init(&east[0], arraysize, MPI_INT, temprank, 22, new_comm, &recvE);
	//Initialize requests (send-recv inits)

	//print_state(current,rank,arraysize,i);//remove comment to print each processes board

	MPI_Barrier(new_comm);//barrier to start timing 
	local_start=MPI_Wtime();
	for ( i = 0; i < generations; ++i)
	{
		//start the start and recieve requests 
		MPI_Start(&sendE);
		MPI_Start(&sendW);
		MPI_Start(&sendN);
		MPI_Start(&sendS);
		MPI_Start(&sendNE);
		MPI_Start(&sendNW);
		MPI_Start(&sendSE);
		MPI_Start(&sendSW);

		MPI_Start(&recvE);
		MPI_Start(&recvW);
		MPI_Start(&recvN);
		MPI_Start(&recvS);
		MPI_Start(&recvNE);
		MPI_Start(&recvNW);
		MPI_Start(&recvSE);
		MPI_Start(&recvSW);


		//Calculate the next generation of the spots that dont need information from other processes
		for (j = 1; j < arraysize -1; ++j)
		{
			for (k = 1; k < arraysize-1; ++k)
			{
				neighbours = calculate_neighbours_inside(current, j, k, arraysize);
				next[j][k] = next_state(neighbours, current[j][k]);
			}
		}
		//Wait until the requests have been resolved 
		MPI_Wait(&sendE,&status);
		MPI_Wait(&sendW,&status);
		MPI_Wait(&sendN,&status);
		MPI_Wait(&sendS,&status);
		MPI_Wait(&sendNE,&status);
		MPI_Wait(&sendNW,&status);
		MPI_Wait(&sendSE,&status);
		MPI_Wait(&sendSW,&status);
		MPI_Wait(&recvE,&statusr);
		for (j = 1; j < arraysize-1; ++j)//calculate the right column
		{
			neighbours = calculate_neighbours_nsew('e', east, current, arraysize, j);
			next[j][arraysize-1] = next_state(neighbours, current[j][arraysize-1]);
		}
		MPI_Wait(&recvW,&status);
		for (j = 1; j < arraysize-1; ++j)//calculate the left column
		{
			neighbours = calculate_neighbours_nsew('w', west, current, arraysize, j);
			next[j][0] = next_state(neighbours, current[j][0]);
		}
		MPI_Wait(&recvN,&status);
		for (j = 1; j < arraysize-1; ++j)//calculate the top column
		{
			neighbours = calculate_neighbours_nsew('n', north, current, arraysize, j);
			next[0][j] = next_state(neighbours, current[0][j]);
		}
		MPI_Wait(&recvS,&status);
		//section
		for (j = 1; j < arraysize-1; ++j)//calculate the bottom column
		{
			neighbours=calculate_neighbours_nsew('s', south, current, arraysize, j);
			next[arraysize-1][j] = next_state(neighbours, current[arraysize-1][j]);
		}
		//section
		MPI_Wait(&recvNE,&status);
		neighbours = calculate_neighbours_corners(0, east, west, ne, current, arraysize, j);//calculate the top right element
		next[0][arraysize-1] = next_state(neighbours, current[0][arraysize-1]);

		MPI_Wait(&recvNW,&status);
		neighbours = calculate_neighbours_corners(1, west, north, nw, current, arraysize, j);//calculate the top left element
		next[0][0] = next_state(neighbours, current[0][0]);

		MPI_Wait(&recvSE,&status);
		neighbours = calculate_neighbours_corners(2, south, east, se, current, arraysize, j);//calculate the bottom right element
		next[arraysize-1][arraysize-1] = next_state(neighbours, current[arraysize-1][arraysize-1]);

		MPI_Wait(&recvSW,&status);
		neighbours = calculate_neighbours_corners(3, south, west, sw, current, arraysize, j);//calculate the bottom left element
		next[arraysize-1][0] =  next_state(neighbours, current[arraysize-1][0]==0);

		if(i%check == 0)//checks every 51 generations if there have been no changes or all the elements are dead
		{
			all_dead_no_change[0]=0;
			all_dead_no_change[1]=0;
			for (k = 0; k < arraysize; ++k)
			{
				for (j = 0; j < arraysize; ++j)
				{
					if(next[k][j]==1)all_dead_no_change[0]=1;//one alive organism is found
					if(current[k][j]!=next[k][j])all_dead_no_change[1]=1;//there is one different element between two gens 
				}
			}
			if(rank==0)
			{
				//for(int k = 0; k < numprocs; ++k)temp[k]=-1;
				MPI_Gather(all_dead_no_change,2,MPI_INT,temp,2,MPI_INT,0,new_comm);//gather from all the processes the all_dead nochange matrix 

			
			}
			else if (rank!=0)MPI_Gather(all_dead_no_change,2,MPI_INT,temp,2,MPI_INT,0,new_comm);//all the processes except for process 0 send their all_dead_no_change matrix
			if(rank==0)
			{
				dead_count=0;
				no_change_count=0;
				//check the gathered array if all dead or no change 
				for (int k = 0; k < 2*numprocs; k+=2)
				{
					if(temp[k]==0)dead_count++;
					if(temp[k+1]==0)no_change_count++;
				}
				if(dead_count==numprocs||no_change_count==numprocs)termination_flag=0;
			}
			MPI_Bcast(&termination_flag,1,MPI_INT,0,new_comm);//broadcast to all the processes if a terminated condition is found
			if(termination_flag==0)break;
		}
		//Change the arrays for the next generation
		temparray = current;
		current = next;
		next = temparray;
		//print_state(current,rank,arraysize,i);//remove comment to print each processes board
	}
	local_finish = MPI_Wtime();
	local_elapsed = local_finish - local_start;//each process calculates its elapsed time
	MPI_Reduce(&local_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,new_comm);//process 0 gets the maximum elapsed time of all the processes 
	if(rank==0)printf("\nProcess terminated after %.1f seconds. Last generation: %d. arraysize: %dx%d \n\n",elapsed,i,cart_size_x*arraysize,cart_size_y*arraysize);
	//free allocated memory
	free(north);
	free(south);
	free(east);
	free(west);
	free(temp);
	free(curdata);
	free(current);
	free(nextdata);
	free(next);
	MPI_Request_free(&sendN);
	MPI_Request_free(&sendS);
	MPI_Request_free(&sendE);
	MPI_Request_free(&sendW);
	MPI_Request_free(&sendNE);
	MPI_Request_free(&sendNW);
	MPI_Request_free(&sendSE);
	MPI_Request_free(&sendSW);
	MPI_Request_free(&recvN);
	MPI_Request_free(&recvS);
	MPI_Request_free(&recvE);
	MPI_Request_free(&recvW);
	MPI_Request_free(&recvNE);
	MPI_Request_free(&recvNW);
	MPI_Request_free(&recvSE);
	MPI_Request_free(&recvSW);
	MPI_Finalize();
	return 0;
}