#include<pthread.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#define N 4

pthread_mutex_t mtx;
pthread_barrier_t barrier;

int arr[N][N];
const double PHASES = log2(N*N)+1;

void printArray(){

	for(int i=0; i<N; i++){ 
		for(int j=0; j<N; j++){
			printf("%d ",arr[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
}

void swap(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

void *shearShort(void *threadid){
		
	//rowcol determines the row or column the thread will use
	long rowcol = (long)threadid;
	
	for(int phase=1; phase<=PHASES; phase++){
	
		pthread_mutex_lock(&mtx);
	
		
		if(phase%2!=0){ //sort Rows
			
			if(rowcol%2==0){ //increasing order
				int switched;
				
				for(int i=0; i<N-1; i++){
				
					switched = 0;
					
					for(int j=0; j<N-i-1; j++){
					
						if(arr[rowcol][j]>arr[rowcol][j+1]){
							swap(&arr[rowcol][j],&arr[rowcol][j+1]);	
							switched = 1;
						}
					}
					
					if(switched == 0){
						break;
					}
				}
			
			} else{ //decreasing order
				int switched;
			
				for(int i=0; i<N-1; i++){
				
					switched = 0;
					
					for(int j=0; j<N-i-1; j++){
					
						if(arr[rowcol][j]<arr[rowcol][j+1]){
							swap(&arr[rowcol][j],&arr[rowcol][j+1]);	
							switched = 1;
						}
					}
					
					if(switched == 0){
						break;
					}
				}	
			} 
			
		} else{ // column sorting
			int switched;
			
				for(int i=0; i<N-1; i++){
				
					switched = 0;
					
					for(int j=0; j<N-i-1; j++){
					
						if(arr[j][rowcol]>arr[j+1][rowcol]){
							swap(&arr[j][rowcol],&arr[j+1][rowcol]);	
							switched = 1;
						}
					}
					
					if(switched == 0){
						break;
					}
				}
		}
			
		pthread_mutex_unlock(&mtx);	
		int wait = pthread_barrier_wait(&barrier);
		
		if(wait == PTHREAD_BARRIER_SERIAL_THREAD){
		printf("Phase %d\n",phase);
		printArray();
		}
	
		//wait again
		pthread_barrier_wait(&barrier);
	}
	
	
}//end of shearShort

int main(){
	
	//1. Read from file
	FILE *file = fopen("input.txt","r");
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++){
			fscanf(file,"%d,", &arr[i][j]);
		}
	}
	fclose(file);
	
	//2. Print array
	printf("Array read from file:\n");
	printArray(arr);
	printf("\n\n");
	
	//3. Create and initalize condition variables
	pthread_mutex_init(&mtx, NULL);
	pthread_barrier_init(&barrier,NULL,N);
	
	//4. Create threads
	pthread_t threads[N];
	
	int thread;
	
	for(long t=0; t<N; t++){
		//printf("In main: creating thread %d\n", (int)t);
		thread = pthread_create(&threads[t], NULL, shearShort, (void *)t);
	
		if(thread){
		 	printf("ERROR; return code from pthread_create() is %d\n", thread);
       	exit(-1);
       }
	}
	

	//Wait on other threads
	for(int i=0; i<N; i++){
		pthread_join(threads[i], NULL);
		
	}
	
	pthread_mutex_destroy(&mtx);
	pthread_barrier_destroy(&barrier);
	//pthread_exit(NULL);
	
	return 0;
}
