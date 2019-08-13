/***************************************************************************
 * This program utilizes the cell and maze classes to create, solve, draw, *
 * or seed a pseudo-random number generator. User options are read through *
 * command line arguements. The following is a list of user arguements:    *
 *                                                                         *
 * -v             gives author's name; the program exits immediately after *
 *                the name is printed.                                     *
 * -s III OOO     solve the maze in file III placing solution in file OOO. *
 * -c RRR CCC MMM create a maze with RRR rows and CCC columns, placing the *
 *                maze in file MMM.                                        *
 * -r NNN         seed a pseudo-random number generator with NNN; if the   *
 *                -r option is not given, use a random seed of 1.          *
 *-d III          draw the created maze ( -c) or draw the solved maze      *
 *                ( -s) found in file III.                                 *
 *                                                                         *
 * Author: Alan Hencey                                                     *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cell.h"
#include "maze.h"

// Necissary for random() and srandom() to work.
long int random(void);
void srandom(unsigned int seed);

/* options */
int Special = 0;    /* option -s      */
int Number = 0;     /* option -n XXXX */
char* Name1 = 0;     /* option -N YYYY ZZZZ */
char* Name2 = 0;

int ProcessOptions(int,char **);
void Fatal(char *,...);
void printInvalidArgs();
void printAuthor();

int
main(int argc,char **argv)
    {
    int argIndex;

    if (argc == 1) Fatal("%d arguments.\n",argc-1);

    argIndex = ProcessOptions(argc,argv);

    return 0;
    }

void
Fatal(char *fmt, ...)
    {
    va_list ap;

    fprintf(stderr,"An error occured: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(-1);
    }

/* only -oXXX  or -o XXX options */

int
ProcessOptions(int argc, char **argv)
    {
    int CREATE = 0;
    int createRows = 0;
    int createCols = 0;
    int createFileArg = 0;

    int SOLVE = 0;
    int solveFileArg = 0;
    int solutionFileArg = 0;

    int SEED = 0;
    int randomSeed = 1;

    int DRAW = 0;
    int drawFileArg = 0;

    int start,argIndex;
    int argsUsed;
    char *arg;

    argIndex = 1;

    while (argIndex < argc && *argv[argIndex] == '-')
        {
        /* check if stdin, represented by "-" is an argument */
        /* if so, the end of options has been reached */
        if (argv[argIndex][1] == '\0') return argIndex;

        argsUsed = 0;
        start = argIndex;


        /* advance argIndex to point to the first argument to the option */
        if (argv[start][2] == '\0')
            {
            arg = argv[start+1];
            ++argIndex;
            }
        else
            {
            /* first arg is connected to option, so don't advance */
            arg = argv[start]+2;
            }

        switch (argv[start][1])
            {
            /*
             * when option has an argument, do this
             *
             *     examples are -m4096 or -m 4096
             *
             *     case 'm':
             *         MemorySize = atol(arg);
             *         argsUsed = 1;
             *         break;
             *
             * when option has multiple arguments, do this
             *
             *     examples are -r4096 1280 or -r 4096 1280
             *
             *     case 'r':
             *         Rows = atoi(arg);
             *         Cols = atoi(argv[argIndex+1]);
             *         argsUsed = 2;
             *         break;
             *
             * when option does not have an argument, do this
             *
             *     example is -a
             *
             *     case 'a':
             *         PrintActions = 1;
             *         break;
             */

            case 'v':
                printAuthor();
                return 0;
            case 'r':
                SEED = 1;
                randomSeed = atoi(arg);
                argsUsed = 1;
                break;
            case 'c':
                CREATE = 1;
                createRows = atoi(arg);
                createCols = atoi(argv[argIndex+1]);
                createFileArg = argIndex + 2;
                argsUsed = 3;
                break;
            case 's':
                SOLVE = 1;
                solveFileArg = argIndex;
                solutionFileArg = argIndex + 1;
                argsUsed = 2;
                break;
            case 'd':
                DRAW = 1;
                drawFileArg = argIndex;
                argsUsed = 1;
                break;
            default:
                printInvalidArgs();
                exit(-1);
            }

        argIndex += argsUsed;
        }

        if (CREATE)
        {
            FILE *createFile = fopen(argv[createFileArg],"w");
            createMAZE(createRows,createCols,randomSeed,createFile);
            fclose(createFile);
        }
        if (SOLVE)
        {
            FILE *solveFile = fopen(argv[solveFileArg],"r");
            FILE *solutionFile = fopen(argv[solutionFileArg],"w");
            solveMAZE(solveFile,solutionFile);
            fclose(solveFile);
            fclose(solutionFile);
        }
        if (DRAW)
        {
            FILE *drawFile = fopen(argv[drawFileArg],"r");
            drawMAZE(drawFile);
            fclose(drawFile);
        }

    return argIndex;
    }

void printInvalidArgs()
{
    printf("Invalid command line arguements. Please use one or more of \n");
    printf("the following:\n\n");
    printf("option             action\n");
    printf("-v                 gives author's name; the program exits\n");
    printf("                   immediately after the name is printed.\n\n");
    printf("-s III OOO         solves the maze in file III placing\n");
    printf("                   the solution in file OOO.\n\n");
    printf("-c RRR CCC MMM     creates a maze with RRR rows and CCC\n");
    printf("                   columns, placing the maze in file MMM.\n\n");
    printf("-r NNN             seeds a pseudo-random number generator\n");
    printf("                   with NNN; if the -r option is not given,\n");
    printf("                   a random seed of 1 is used.\n\n");
    printf("-d III             draws the created maze ( -c) or draws the\n");
    printf("                   solved maze ( -s) found in file III.\n\n");
}

void printAuthor() {printf("Written by Alan Hencey\n");}
