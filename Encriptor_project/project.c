#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>


//remove '\n'

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}


// Function to return the next random number
int getNum(int v[],int n)
{

	//printf("In get Num  n este %d \n",n);
	//printf("In get Num  vectortul este ");
	//for(int i=0;i<n;i++)
		//printf("%d ",v[i]);
	//printf("\n");	
	// Generate a random number
	srand(time(NULL));

	// Make sure the number is within
	// the index range
	int index = rand() % n;

	// Get random number from the vector
	int num = v[index];

	// Remove the number from the vector
	int aux;
	aux=v[index];
	v[index]=v[n-1];
	
	//swap(v[index], v[n - 1]);
	

	// Return the removed number
	return num;
}

// Function to generate n non-repeating random numbers
int generateRandom(int n, char word[],FILE *fptr,FILE *fptr2)
{
    int aux=n;
    key_t SH_KEY=5678910;
    int shmid =shmget(SH_KEY, 10000 *sizeof(int),0666 | IPC_CREAT);

    if(shmid<0){
        perror("Memorie partajata");
    }
    int* v=(int*)shmat(shmid,NULL,0);

    // Fill the vector with the values
    // 0, 1, 2, 3, ..., n-1
    for (int i = 0; i < n; i++)
        v[i] = i ;

	
    
    // While vector has elements
    // get a random number from the vector and print it
    
    for( int i=0;i<= aux-1;i++)
    {
        pid_t pid = fork();
        if (pid<0) return errno;
        else if(pid==0){
	usleep(i*1000);/* wait 1ms for child 1, 2ms for child 2,... */	
	int nr=getNum(v,n-i);
        printf("%d %c\n" ,nr,word[nr]);
        //fputc(nr+'0', fptr2);
        //fputc(word[nr], fptr);
	fprintf(fptr2,"%c", nr+'0');
	fprintf(fptr,"%c", word[nr]);
        printf("Copilul %d a terminat  \n",getpid());

        exit(0);
        }

    }
   for(int i=0;i<aux;i++){
    	wait(NULL);
    	//printf("astept  i-ul %d  \n",i);
    }
	

	printf("---------AM TERMINAT UN CUVANT------------------\n");    
	
}

void decriptare(char word[],char permutare[],FILE *fptr)
{
	int n=strlen(permutare);
	//printf("%s %s ", word,permutare);
	for(int i=0;i<n;i++){
		int count=0;
		while(i!=(permutare[count]-'0'))
			count++;
		fputc(word[count], fptr);
	}
	fputc(' ', fptr);
}

int main(int argc,char* argv[]){
//criptarea un fiser cu cuvinte
   if(argc <3){
   	 char* fromFile = argv[1],* toFile = "criptat.txt", * toFile2 = "permutari.txt";
   	FILE *pFile;
        pFile = fopen (fromFile, "r");
   	if (pFile == NULL) perror ("Error opening file");
   	else {
   		FILE *fptr, *fptr2;
		fptr = fopen(toFile, "w");
		fptr2 = fopen(toFile2, "w");
		char mystring [1000000];
		char* pch;
     		while ( fgets (mystring , 1000000 , pFile) != NULL ) {
       			
       			pch = strtok (mystring," ");
       			while (pch != NULL )
       			{	
       				strstrip(pch);
       				printf("The contents of pch are:%s\n", pch);
       				int n=strlen(pch);
       				//printf("N-UL MEU ESTE %d \n",n);
         			generateRandom(n, pch, fptr, fptr2);
         			printf()
         			fputc('*', fptr2);
				fputc('*', fptr);
         			pch = strtok (NULL, " ");
       			}
     		}
     		fclose (pFile);
     		fclose(fptr);
     		fclose(fptr2);
   	}
   }
   else if(argc<4){
   //DECRIPTAREA
   	char* fromFile = argv[1],* fromFile2= argv[2], * toFile = "decriptare.txt";
   	FILE *pFile,*pFile2;
        pFile = fopen (fromFile, "r");
        pFile2 = fopen (fromFile2, "r");
        if (pFile == NULL) perror ("Error opening file1");
   	else 
   	{   		if (pFile2 == NULL) perror ("Error opening file2");
   		else 
   		{
   			FILE *fptr;
			fptr = fopen(toFile, "w");
			char mystring [1000000];
			char mystring2 [1000000];
			char *save_pch, *save_pch2;
			
			//
			int shm_id;
			key_t mem_key=5678;
			//int *shm_ptr;
			mem_key=ftok(".","a");
			shm_id=shmget(mem_key, 4*sizeof(int), IPC_CREAT | 0666);
			
			if(shm_id<0){
				perror("Memorie partajata decriptare");
				exit(1);
			}
			char* pch=(char *)shmat(shm_id,NULL,0);
			char* pch2=(char *)shmat(shm_id,NULL,0);
			while(fgets(mystring,1000000,pFile2) !=NULL)
			{
				if(fgets(mystring2,1000000,pFile2) !=NULL)
				{
					pch=strtok_r(mystring," ",&save_pch);
					pch2=strtok_r(mystring2," ",&save_pch2);
					for(int i=0;i<1;i++)
					{
						pid_t pid=fork();
						if (pid<0) return errno;
						else if(pid==0){
							decriptare(pch2,pch,fptr);
							pch=strtok_r(NULL," ",&save_pch);
							pch2=strtok_r(NULL," ",&save_pch2);
							exit(0);
						}
						else{
							//wait(NULL);
							printf("decriptare :Copilul %d a terminat \n", getpid());
						}
					}
				}
			}
		fclose(pFile);
		fclose(pFile2);
		fclose(fptr);
		}
	}
   }
   else printf("Prea multe arugumente");
   return 0;
 }
			
			
			
			
			
			
			
