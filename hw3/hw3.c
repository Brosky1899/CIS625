#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

#define maxwords 50000
#define maxlines 100000
#define wordlen 10
#define linelen 2001

double myclock();

int main(int argc, char **argv)
{

   int nwords;
   int nlines;
   int i, k, err;
   double nchars = 0;
   double tstart, ttotal;
   FILE *fd;
   static char word[maxwords][wordlen];
   static char line[maxlines][linelen];
   memset(word, 0, sizeof(char) * maxwords * wordlen);
   memset(line, 0, sizeof(char) * maxlines * linelen);


// MPI

   int comm_size;
   int comm_rank;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

   if(comm_rank==0){


// Read in the dictionary words

   fd = fopen( "/homes/dan/625/keywords.txt", "r" );
   nwords = -1;
   do {
      err = fscanf( fd, "%[^\n]\n", word[++nwords] );
   } while( err != EOF && nwords < maxwords );
   fclose( fd );

   printf( "Read in %d words from dictionary\n", nwords);


// Read in the lines from the data file

   fd = fopen( "/homes/dan/625/wiki_dump.txt", "r" );
   nlines = -1;
   do {
      err = fscanf( fd, "%2000s\n", line[++nlines]);
      if( line[nlines] != NULL ) nchars += (double) strlen( line[nlines] );
   } while( err != EOF && nlines < maxlines);
   fclose( fd );

   printf( "Read in %d lines from data file averaging %.0lf chars/line\n", nlines, nchars / nlines);


// Loop over the word list

   tstart = myclock();  // Set the zero time
   tstart = myclock();  // Start the clock

   for(i=1; i<comm_size; ++i){
      MPI_Send(word, maxwords * wordlen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
      MPI_Send(line, maxlines * linelen, MPI_CHAR, i, 0, MPI_COMM_WORLD);
   }

   fd = fopen( "wiki.out", "w" );
   for(i=0; i<maxwords; ++i){
      int from=i%(comm_size-1)+1;
      MPI_Recv(&k, 1, MPI_INT, from, 0, MPI_COMM_WORLD, &status);
      if(k==-1)
         continue;
      fprintf(fd, "%s: %d", word[i], k);
      while(1){
         MPI_Recv(&k, 1, MPI_INT, from, 0, MPI_COMM_WORLD, &status);
	 if(k==-1){
	    fprintf(fd, "\n");
	    break;
	 }
	 fprintf(fd, ", %d", k);
      }
   }
   fclose(fd);
   ttotal = myclock() - tstart;
   printf("%d,%f\n", comm_size, ttotal);

   }
   else {

   MPI_Recv(word, maxwords * wordlen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
   MPI_Recv(line, maxlines * linelen, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

   for( i = 0; i < maxwords; i+=(comm_size-1) ) {
      for( k = 0; k < maxlines; k++ ) {
         if( strstr( line[k], word[i] ) != NULL )
	    MPI_Send(&k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
      k=-1;
      MPI_Send(&k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
   }
   MPI_Send(&k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

   }


// MPI

   MPI_Finalize();

}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
