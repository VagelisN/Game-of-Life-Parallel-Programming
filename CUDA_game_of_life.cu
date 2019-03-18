#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <cuda.h>
#include <math.h>


__global__ void kernel_func(int *ccurrent, int *cnext,int arraysize)
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;// get the row number
	int col = blockIdx.x * blockDim.x + threadIdx.x;// get the col number
	if(row >= arraysize || col >= arraysize)return;// check if the dimensions are over the dims of the array
	/*Because the ccurent array is a flattened 2d array we access each element ccurent[(row * arraysize) + column]*/
	int neighbours = 0;//start checking the neighbours
	if((col-1) < 0){if(ccurrent[row * arraysize + (arraysize -1)] == 1 )neighbours++;}//West neighbour
    else if(ccurrent[row * arraysize +(col-1)] == 1 )neighbours++;//west

    if((row+1) > (arraysize-1))//South-West neighbour
    {
     	if((col-1) < 0){if(ccurrent[(arraysize-1)] == 1)neighbours++;}
      	else if(ccurrent[(col-1)] == 1)neighbours++;
    }
    else if((col-1) < 0){if(ccurrent[(row+1) * arraysize + (arraysize-1)] == 1)neighbours++;}//sw
    else if(ccurrent[(row+1) * arraysize +(col-1)] == 1 )neighbours++;//sw
                
    if((row+1) > arraysize-1){if(ccurrent[col] == 1)neighbours++;}//South neighbour
    else if(ccurrent[(row+1) * arraysize + col] == 1 )neighbours++;//s
                
    if((row+1) > (arraysize-1))//South- East neighbour
    {
        if((col+1) > (arraysize-1)){if(ccurrent[0] == 1)neighbours++;}
    	else if(ccurrent[col+1] == 1)neighbours++;
    }
    else if((col+1) > (arraysize-1)){if(ccurrent[(row+1) * arraysize] == 1)neighbours++;}//se
    else if(ccurrent[(row+1) * arraysize + col+1] == 1 )neighbours++;//se
                
    if((col+1) > (arraysize-1)){if(ccurrent[row * arraysize] == 1)neighbours++;}//East neighbour
    else if(ccurrent[row * arraysize + col+1] == 1 )neighbours++;//e

    if((row-1) < 0)//North- East neighbour
    {
    	if((col+1) > (arraysize-1) ){if(ccurrent[(arraysize-1) * arraysize] == 1)neighbours++;}
        else if(ccurrent[(arraysize-1)* arraysize + col+1] == 1)neighbours++;
    }
    else if((col+1) > arraysize-1){if(ccurrent[(row-1) * arraysize] == 1)neighbours++;}//ne
    else if(ccurrent[(row-1) * arraysize + col+1] == 1 )neighbours++;//ne
                
    if((row-1) < 0){if(ccurrent[(arraysize-1)* arraysize + col])neighbours++;}//North neighbour
    else if(ccurrent[(row-1) * arraysize + col] == 1) neighbours++;//n
                
    if((row-1) < 0)//North- West neighbour
    {
    	if((col-1) < 0 ){if(ccurrent[(arraysize-1) * arraysize + arraysize-1] == 1)neighbours++;}
        else if(ccurrent[(arraysize-1) * arraysize + col-1] == 1)neighbours++;
    }
    else if((col-1) < 0){if(ccurrent[(row-1) * arraysize + arraysize-1] == 1)neighbours++;}//nw
    else if(ccurrent[(row-1) * arraysize + col-1] == 1 )neighbours++;//nw
                   
    if(ccurrent[row * arraysize +col] == 0)
    {
    	if(neighbours == 3)cnext[row * arraysize + col]=1;//If the array[row][col] = 1 check the number of neighbours and set the next value 
        else cnext[row * arraysize + col] = 0;
    }
    if(ccurrent[row * arraysize + col] == 1)
    {
    	if(neighbours>1 && neighbours<4)cnext[row * arraysize + col] = 1;
    	else cnext[row * arraysize + col] = 0;
    }
}


__global__ void kernel_checksimilarity(int *ccurrent, int *cnext, int arraysize, int *dsame, int *ddead)
{
	int row = blockIdx.y * blockDim.y + threadIdx.y;// get the row number
	int col = blockIdx.x * blockDim.x + threadIdx.x;// get the col number
	if(row >= arraysize || col >= arraysize)return;// check if the dimensions are over the dims of the array
	if(ccurrent[row * arraysize + col] != cnext[row * arraysize + col]) *dsame = 0;
	if(cnext[row * arraysize + col] == 1)*ddead = 0;
}



int main(int argc, char *argv[])
{
	srand(time(NULL));
	unsigned int i, j, arraysize = 50,check = 31;
	int *same , *dead;
	int *dsame = NULL, *ddead = NULL;
	same = (int*)malloc(sizeof(int));
	dead = (int*)malloc(sizeof(int));
	cudaMalloc(&dsame,sizeof(int));
	cudaMalloc(&ddead,sizeof(int));
	unsigned long generations = 50;
	char c;
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
	//Host mallocs and initialization of the arrays
	int *currentdata = (int*)malloc(arraysize * arraysize * sizeof(int));
	int **current = (int**)malloc(arraysize * sizeof(int*));
	int *nextdata = (int*)malloc(arraysize *arraysize * sizeof(int));
	int **next = (int**)malloc(arraysize * sizeof(int*));
	for (i = 0; i < arraysize; ++i)
	{
		current[i] = &(currentdata[i*arraysize]);
		next[i] = &(nextdata[i*arraysize]);
	}
	for (i = 0; i < arraysize; ++i)
	{
		for (j= 0; j < arraysize; ++j)
		{
			current[i][j] = rand()%2;
			next[i][j] = 0;
			//printf("%d ", current[i][j]);
		}
		//printf("\n");
	}
	//Device mallocs and transfer HostToDevice
	int *ccurrent = NULL;
	cudaMalloc(&ccurrent,(arraysize * arraysize * sizeof(int)));
	int *cnext = NULL;
	cudaMalloc(&cnext,(arraysize * arraysize * sizeof(int)));
	cudaMemcpy(ccurrent, currentdata, (arraysize * arraysize * sizeof(int)), cudaMemcpyHostToDevice);
	cudaMemcpy(cnext, nextdata, (arraysize * arraysize * sizeof(int)), cudaMemcpyHostToDevice);
	int *ctemp;
	//dim3 dimBlock(block_size, block_size);
	//dim3 dimGrid((arraysize + dimBlock.x - 1) / dimBlock.x,(arraysize + dimBlock.y - 1) / dimBlock.y);
	
	cudaDeviceProp myCUDA;
	if (cudaGetDeviceProperties(&myCUDA, 0) == cudaSuccess)
	{
	    printf("Using device %d: ", 0);
	    printf("%s; global mem: %dΒ; compute v%d.%d; clock: %d kHz\n",myCUDA.name, (int)myCUDA.totalGlobalMem, (int)myCUDA.major,(int)myCUDA.minor, (int)myCUDA.clockRate);
	}
	int threadsPerBlock = myCUDA.maxThreadsPerBlock;
	int temp = (int)sqrt(threadsPerBlock);
	printf("Maximum threads per block dimension = %d\n",temp);
	dim3 dimBlock(temp, temp);
	dim3 dimGrid((arraysize + dimBlock.x - 1) / dimBlock.x,(arraysize + dimBlock.y - 1) / dimBlock.y);
	float ttime;
	cudaEvent_t start, stop;//Start the clock

	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start, 0);
	for (i = 0; i < generations; ++i)
	{
		kernel_func<<<dimGrid, dimBlock>>>(ccurrent,cnext,arraysize);//call the Device function
		cudaDeviceSynchronize();
		if((i%check) == 0)
		{
			*same = 1;
			*dead = 1;
			cudaMemcpy(dsame, same, sizeof(int), cudaMemcpyHostToDevice);
			cudaMemcpy(ddead, dead, sizeof(int), cudaMemcpyHostToDevice);
			kernel_checksimilarity<<<dimGrid,dimBlock>>>(ccurrent,cnext,arraysize,dsame,ddead);
			cudaDeviceSynchronize();
			cudaMemcpy(same, dsame, sizeof(int), cudaMemcpyDeviceToHost);
			cudaMemcpy(dead, ddead, sizeof(int), cudaMemcpyDeviceToHost);
			if((*same == 1) || (*dead == 1))break;
		}
		ctemp = ccurrent;//Change the arrays for the next generation
		ccurrent = cnext;
		cnext = ctemp;
		/*cudaMemcpy(currentdata, ccurrent, (arraysize * arraysize * sizeof(int)), cudaMemcpyDeviceToHost);//DevicetoHost transfer
		printf("In generation %d\n",i);
		for ( k = 0; k < arraysize; ++k)//print the array
		{
			for (j = 0; j < arraysize; ++j)
			{
				printf("%d ",current[k][j]);
			}
			printf("\n");
		}*/
	}
	cudaEventRecord(stop, 0);//stop the clock
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&ttime, start, stop);
	printf("\nProcess exited at generation: %d with arraysize: %d x %d, after:  %3.1f ms \n", i, arraysize, arraysize,ttime);
	if(*dead == 1)printf("Exited earlier because all cells are dead.\n");
	if(*same == 1)printf("Exited earlier because current and next generations are the same.\n");
	/*printf("Last version of the array\n");
	cudaMemcpy(currentdata, ccurrent, (arraysize * arraysize * sizeof(int)), cudaMemcpyDeviceToHost);
	for ( i = 0; i < arraysize; ++i)
	{
		for (j = 0; j < arraysize; ++j)
		{
			printf("%d ",current[i][j]);
		}
		printf("\n");
	}*/
	//Free the allocated memory

	cudaFree(ccurrent);
	cudaFree(cnext);
	cudaFree(dsame);
	cudaFree(ddead);
	free(same);
	free(dead);
	free(nextdata);
	free(next);
	free(currentdata);
	free(current);
	return 0;
}