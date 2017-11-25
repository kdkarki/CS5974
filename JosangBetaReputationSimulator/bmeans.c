/**********************************************************************/
/*                                                                    */
/*             File bmeans.c:  Batch Means Analysis Module            */
/*                                                                    */
/*  This is intended to be separately compiled and linked with the    */
/*  object files of the "smpl" and "rand" modules and the object file */
/*  of the simulation program.  Alternatively, the "smpl", "rand",    */
/*  and "bmeans" object files can be used to construct a "smpl" lib-  */
/*  rary file, and the simulation program object file then linked     */
/*  with that library module.  Note that this file "#include"s the    */
/*  "stat.c" file.                                                    */
/*                                                                    */
/**********************************************************************/

#include "smpl.h"

real T(real,int);

static int d,k,m,n;
static real smy,smY,smY2,Y,h;

void init_bm(m0,mb)
        int m0,mb;
{ /* set deletion amount & batch size */
    d=m0; m=mb; smy=smY=smY2=0.0; k=n=0;
}

int obs(y)
        real y;
{
    int r=0; real var;
    if (d) then {d--; return(r);}
    smy+=y; n++;
    if (n==m) then
    { /* batch complete:  update sums & counts */
        smy/=n; smY+=smy; smY2+=smy*smy; k++;
        printf("batch %2d mean = %.3f",k,smy);
        smy=0.0; n=0;
        if (k>=10) then
        { /* compute grand mean & half width */
            Y=smY/k; var=(smY2-k*Y*Y)/(k-1);
            h=T(0.025,k-1)*sqrt(var/k);
            printf(", rel. HW = %.3f",h/Y);
            if (h/Y<=0.1) then r=1;
        }
        printf("\n");
    }
    return(r);
}

void civals(mean,hw,nb)
        real *mean,*hw; int *nb;
{ /* return batch means analysis results */
    *mean=Y; *hw=h; *nb=k;
}