// Shamelessly stolen and adapted from http://www.thegeekstuff.com/2012/05/c-fork-function/

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

int var_glb; /* A global variable*/
static int * var_glb2; /* A pointer that we use as a global variable*/

int main(void)
    {
    pid_t childPID;
    int var_lcl = 0;
    var_glb = 0;
    var_glb2 = mmap(NULL, sizeof *var_glb2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *var_glb2=0;

    childPID = fork();
    if(childPID >= 0) // fork was successful
        {
        if(childPID == 0) // child process
            {
            var_lcl++;
            var_glb++;
            *var_glb2 += 1;
            printf("\n Child Process :: var_lcl = [%d], var_glb[%d], *var_glb2[%d]\n", var_lcl, var_glb, *var_glb2);
            }
        else //Parent process
            {
            var_lcl = 10;
            var_glb += 2;
            *var_glb2 += 2;
            printf("\n Parent process :: var_lcl = [%d], var_glb[%d], *var_glb2[%d]\n", var_lcl, var_glb, *var_glb2);
            }
        }
    else // fork failed
        {
        printf("\n Fork failed, quitting!!!!!!\n");
        return 1;
        }
    return 0;
    }
