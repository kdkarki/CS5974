/**********************************************************************/
/*                           File "smpl.h"                            */
/*  Includes, Defines, & Extern Declarations for Simulation Programs  */
/**********************************************************************/
#ifndef __SMPL_H__
#define __SMPL_H__	1

#include <stdio.h>
#include <math.h>

#define real double
#define then

extern real Lq(), U(), B(), time();
extern char *fname(), *mname();
extern FILE *sendto();

extern real uniform(), expntl(), erlang(), hyperx(), normal();
extern long seed();

void smpl(int,char*);
int facility(char*,int);
void schedule(int,real,int);
void cause(int*,int*);
int request(int,int,int);
void release(int,int);
void report();
void error(int,char*);

void init_bm(int,int);
int obs(real);
void civals(real*,real*,int*);

#endif