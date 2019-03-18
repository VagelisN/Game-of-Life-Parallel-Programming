#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc,char *argv[])
{
    srand(time(NULL));
	char c;
	int arraysize = 1000,i,j,k;
	int generations = 1000 ,all_dead,all_same,neighbours = 0,check = 31;	
	int **tempboard = NULL,**curboard=NULL,**nextboard= NULL;
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
	curboard = malloc(arraysize*sizeof(int*));
	nextboard = malloc(arraysize*sizeof(int*));
	for(i = 0; i < arraysize; i++)
	{
	    curboard[i] = malloc(arraysize * sizeof(int)); 
	    nextboard[i] = malloc(arraysize * sizeof(int)); 
	}
    for (i = 0; i < arraysize; ++i)
    {
        for(j = 0; j < arraysize; j++)
        {
            curboard[i][j] = rand()%2;
            nextboard[i][j] = 0;
            //printf("%d ",curboard[i][j] );
        }
        //printf("\n");
    }
    clock_t begin = clock();
	for(i = 0; i < generations; i++)
	{
	    for(j = 0; j < arraysize; j++)
	    {
	        for(k = 0; k < arraysize; k++)
	        {
	            neighbours = 0;
	            if((k-1) < 0){if(curboard[j][(arraysize -1)] == 1 )neighbours++;}//w
                else if(curboard[j][(k-1)] == 1 )neighbours++;//w

                if((j+1) > (arraysize-1))//sw
                {
                    if((k-1) < 0){if(curboard[0][(arraysize-1)] == 1)neighbours++;}
                    else if(curboard[0][(k-1)] == 1)neighbours++;
                }
                else if((k-1) < 0){if(curboard[j+1][arraysize-1] == 1)neighbours++;}//sw
                else if(curboard[j+1][k-1] == 1 )neighbours++;//sw
                
                if((j+1) > arraysize-1){if(curboard[0][k] == 1)neighbours++;}//s
                else if(curboard[j+1][k] == 1 )neighbours++;//s
                
                if((j+1) > (arraysize-1))//se
                {
                    if((k+1) > (arraysize-1)){if(curboard[0][0] == 1)neighbours++;}
                    else if(curboard[0][k+1] == 1)neighbours++;
                }
                else if((k+1) > (arraysize-1)){if(curboard[j+1][0] == 1)neighbours++;}//se
                else if(curboard[j+1][k+1] == 1 )neighbours++;//se
                
                if((k+1) > (arraysize-1)){if(curboard[j][0] == 1)neighbours++;}//e
                else if(curboard[j][k+1] == 1 )neighbours++;//e
                
                
                if((j-1) < 0)//ne
                {
                    if((k+1) > (arraysize-1) ){if(curboard[arraysize-1][0] == 1)neighbours++;}
                    else if(curboard[arraysize-1][k+1] == 1)neighbours++;
                }
                else if((k+1) > arraysize-1){if(curboard[j-1][0] == 1)neighbours++;}//ne
                else if(curboard[j-1][k+1] == 1 )neighbours++;//ne
                
                if((j-1) < 0){if(curboard[arraysize-1][k])neighbours++;}//n
                else if(curboard[j-1][k] == 1) neighbours++;//n
                
                if((j-1) < 0)//nw
                {
                    if((k-1) < 0 ){if(curboard[arraysize-1][arraysize-1] == 1)neighbours++;}
                    else if(curboard[arraysize-1][k-1] == 1)neighbours++;
                }
                else if((k-1) < 0){if(curboard[j-1][arraysize-1] == 1)neighbours++;}//nw
                else if(curboard[j-1][k-1] == 1 )neighbours++;//nw
                
                
                    
                if(curboard[j][k] == 0)
                {
                    if(neighbours == 3)nextboard[j][k]=1;
                    else nextboard[j][k] = 0;
                }
                if(curboard[j][k] == 1)
                {
                    if(neighbours>1 && neighbours<4)nextboard[j][k] = 1;
                    else nextboard[j][k] = 0;
                }
	        }
	    }
        tempboard = nextboard;
        nextboard = curboard;
        curboard = tempboard;
        if(i%check == 0)
        {
            all_dead = 1;
            all_same = 1;
            for (j = 0; j < arraysize; ++j)
            {
                for (k = 0; k < arraysize; ++k)
                {
                    if(curboard[j][k] == 1)
                    {
                        all_dead= 0;
                        break;
                    }
                    if(nextboard[j][k] != curboard[j][k])
                    {
                        all_same = 0;
                    }
                }
            }
            if(all_dead == 1)break;
            if(all_same == 1)break;
        }
	}
    clock_t end = clock();
    double time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
    //printf("Last generation is %d :\n",i);
    /*for (j = 0; j < arraysize; ++j)
    {
        for (k = 0; k < arraysize; ++k)
            printf("%d ",curboard[j][k] );
        printf("\n");
    }*/
    printf("\nProcess terminated after %.1f seconds. Last generation: %d. arraysize: %dx%d \n\n",time_spent,i,arraysize,arraysize);
	for (i = 0; i < arraysize; ++i)
    {
        free(curboard[i]);
        free(nextboard[i]);
    }
    free(curboard);
    free(nextboard);
}