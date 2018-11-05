#include < stdlib.h > 
#include < stdio.h > 
#include < string.h > 
#include < time.h > 
#include "mpi.h"

#define maxwords 50000
#define maxlines 100000
#define wordlen 10
#define linelen 2001

double myclock();

int main(int argc, char ** argv) {

  int nwords;
  int nlines;
  int i, k, err;
  double nchars = 0;
  double tstart, ttotal;
  FILE * fileVar;
  static char wordList[maxwords][wordlen];
  static char lineList[maxlines][linelen];
  memset(wordList, 0, sizeof(char) * maxwords * wordlen);
  memset(lineList, 0, sizeof(char) * maxlines * linelen);

  // MPI

  int comm_size;
  int comm_rank;
  MPI_Status status;
  MPI_Init( & argc, & argv);
  MPI_Comm_size(MPI_COMM_WORLD, & comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, & comm_rank);

  if (comm_rank == 0) {
	
    fileVar = fopen("/homes/dan/625/keywords.txt", "r");
    nwords = -1;
    do {
      err = fscanf(fileVar, "%[^\n]\n", wordList[++nwords]);	//read in dictionary 
    } while (err != EOF && nwords < maxwords);
    fclose(fileVar);

    printf("Read in %d words from dictionary\n", nwords);

    fileVar = fopen("/homes/dan/625/wiki_dump.txt", "r");
    nlines = -1;
    do {
      err = fscanf(fileVar, "%2000s\n", lineList[++nlines]);	//read in wiki dump
      if (lineList[nlines] != NULL) nchars += (double) strlen(lineList[nlines]);
    } while (err != EOF && nlines < maxlines);
    fclose(fileVar);

    printf("Read in %d lines from data file averaging %.0lf chars/line\n", nlines, nchars / nlines);

    tstart = myclock(); // Set the zero time
    tstart = myclock(); // Start the clock

    for (i = 1; i < comm_size; ++i) {
      MPI_Send(wordList, maxwords * wordlen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
      MPI_Send(line, maxlines * linelen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }

    fileVar = fopen("wiki.out", "w");
    for (i = 0; i < maxwords; ++i) {
      int from = i % (comm_size - 1) + 1;
      MPI_Recv( & k, 1, MPI_INT, from, 0, MPI_COMM_WORLD, & status);
      if (k == -1)
        continue;
      fprintf(fileVar, "%s: %d", wordList[i], k);
      while (1) {
        MPI_Recv( & k, 1, MPI_INT, from, 0, MPI_COMM_WORLD, & status);
        if (k == -1) {
          fprintf(fileVar, "\n");
          break;
        }
        fprintf(fileVar, ", %d", k);
      }
    }
    fclose(fileVar);
    ttotal = myclock() - tstart;
    printf("%d,%f\n", comm_size, ttotal);

  } else {

    MPI_Recv(wordList, maxwords * wordlen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, & status);
    MPI_Recv(line, maxlines * linelen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, & status);

    for (i = 0; i < maxwords; i += (comm_size - 1)) {
      for (k = 0; k < maxlines; k++) {
        if (strstr(line[k], wordList[i]) != NULL)
          MPI_Send( & k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
      k = -1;
      MPI_Send( & k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Send( & k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  }
  MPI_Finalize();

}

double myclock() {
  static time_t t_start = 0; // Save and subtract off each time

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, & ts);
  if (t_start == 0) t_start = ts.tv_sec;

  return (double)(ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}