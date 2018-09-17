#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define STRING_SIZE 16

unsigned int array_size;
char **char_array;
int char_counts[26];

char getRandomChar()
{
	int randNum = 0;
	char randChar = ' ';

	randNum = 26 * (rand() / (RAND_MAX + 1.0)); 	// pick number 0 < # < 25
	randNum = randNum + 97;				// scale to 'a'
	randChar = (char) randNum;

	// printf("%c", randChar);
	return randChar;
}

void init_arrays()
{
  int i, j, randNum; 
  char randChar;

  for ( i = 0; i < array_size; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
		 char_array[i][j] = getRandomChar();
	}
  }

  for ( i = 0; i < 26; i++ ) {
  	char_counts[i] = 0;
  }
}

void count_array()
{
  char theChar;
  int i, j, charLoc;
  
  for ( i = 0; i < array_size; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
	         theChar = char_array[i][j];
		 charLoc = ((int) theChar) - 97;
		 char_counts[charLoc]++;
	}
  }
}

void print_results()
{
  int i, total = 0;

  for ( i = 0; i < 26; i++ ) {
     total += char_counts[i];
     printf(" %c %d\n", (char) (i + 97), char_counts[i]);
  }
  printf("\nTotal characters:  %d\n", total);
}

void mallocCharArray() {
  int i;
  char_array = (char **)malloc(array_size * STRING_SIZE * sizeof(char *));
	for(i=0; i < array_size; i++) {
		char_array[i] = (char *)malloc(STRING_SIZE * sizeof(char));
	}
}
void freeCharArray() {
  int i;
  for(i = 0; i < array_size; i++) {
    free(char_array[i]);
  }
  free(char_array);
}

int main(int argc, char** argv) {
	struct timeval t1, t2;
	double elapsedTime;
  array_size = strtoul(argv[1], NULL, 0) * 500;
  mallocCharArray();
	init_arrays();
	gettimeofday(&t1, NULL);
	count_array();
	gettimeofday(&t2, NULL);
	print_results();
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("DATA, %u, %s, %s, %f\n", array_size, getenv("SLURM_NTASKS"), getenv("SLURMD_NODENAME"),  elapsedTime);
  freeCharArray();
  return 0;
}

