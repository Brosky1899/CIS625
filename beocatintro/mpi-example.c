#include <omp.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  int numprocs, rank, namelen, id;
  char processor_name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name,&namelen);

  printf("MPI: Process %d on %s out of %d\n",rank,processor_name,numprocs);

  #pragma omp parallel private(id)
  {
    id = omp_get_thread_num();
    printf("OMP: (%s) Thread %d\n",processor_name,id);
  }

  MPI_Finalize();
  return(0);
}
