using System;
using System.Runtime.InteropServices;

namespace TrustMgmtSimulation
{
    /// <summary>
    /// This is a wrapper class for smpl library.
    /// The smpl shared object library is compiled with following files:
    /// smpl.h, smpl.c, bmeans.c, rand.c, stat.c
    /// 
    /// The shared object library is 'libsmpl.dylib'. 
    /// It was compiled in following 2 steps:
    /// 
    /// gcc -c -fpic smpl.c bmeans.c rand.c stat.c
    /// gcc -shared -o libsmpl.dylib smpl.o stat.o bmeans.o rand.o
    /// 
    /// The first step compiles all the individual files and generates
    /// respective object files (.o). The second step combines these object 
    /// file into a single shared object file with defined by -o argument
    /// </summary>
    public class SMPLWrapper
    {
        public SMPLWrapper()
        {
            
        }

        /*
         * Functions from stat.c
         */

        /*--------  COMPUTE pth QUANTILE OF THE NORMAL DISTRIBUTION  ---------*
         * This function computes the pth upper quantile of the stand-        *
         * ard normal distribution (i.e., the value of z for which the        *
         * are under the curve from z to +infinity is equal to p).  'Z'       *
         * is a transliteration of the 'STDZ' function in Appendix C of       *
         * "Principles of Discrete Event Simulation", G. S. Fishman,          *
         * Wiley, 1978.   The  approximation used initially appeared in       *
         * in  "Approximations for Digital Computers", C. Hastings, Jr.,      *
         * Princeton U. Press, 1955. 
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "Z")]
        public static extern double Z(double p);

        /*-----------  COMPUTE pth QUANTILE OF THE t DISTRIBUTION  -----------*
         * This function computes the upper pth quantile of the t dis-        *
         * tribution (the value of t for which the area under the curve       *
         * from t to +infinity is equal to p).  It is a transliteration       *
         * of the 'STUDTP' function given in Appendix C of  "Principles       *
         * of Discrete Event Simulation", G. S. Fishman, Wiley, 1978.   
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "T")]
        public static extern double T(double p, int ndf);

        /* ------------------------------------------------------------------ */

        /*
         * Function from rand.c
         */

        /*--------------------  SELECT GENERATOR STREAM  ---------------------*
         * set stream for 1<=n<=15, return stream for n=0 
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "stream")]
        public static extern int stream(int n);

        /*--------------------------  SET/GET SEED  --------------------------*
         * set seed of stream n for Ik>0, return current seed for Ik=0        
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "seed")]
        public static extern long seed(long Ik, int n);

        /*------------  UNIFORM [a, b] RANDOM VARIATE GENERATOR  -------------* 
         * 'uniform' returns a psuedo-random variate from a uniform           * 
         * distribution with lower bound a and upper bound b.                 
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "uniform")]
        public static extern double uniform(double a, double b);

        /*--------------------  RANDOM INTEGER GENERATOR  --------------------*
         * 'random' returns an integer equiprobably selected from the         *
         * set of integers i, i+1, i+2, . . , n.                              
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "random")]
        public static extern int random(int i, int n);

        /*--------------  EXPONENTIAL RANDOM VARIATE GENERATOR  --------------*
         * 'expntl' returns a psuedo-random variate from a negative           *
         * exponential distribution with mean x.                              
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "expntl")]
        public static extern double expntl(double x);

        /*----------------  ERLANG RANDOM VARIATE GENERATOR  -----------------*
         * 'hyperx' returns a psuedo-random variate from Morse's two-stage    * 
         * hyperexponential distribution with mean x and 
         * standard deviation s, s>x.
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "hyperx")]
        public static extern double hyperx(double x, double s);

        /*-----------------  NORMAL RANDOM VARIATE GENERATOR  ----------------*
         * 'normal' returns a psuedo-random variate from a normal distribution*
         * with mean x and standard deviation s.                              *
         */
        [DllImport("./libsmpl.dylib", EntryPoint = "normal")]
        public static extern double normal(double x, double s);

        /* ------------------------------------------------------------------ */

        /*
         * Function from bmeans.c
         */

        /*------------------------  INITIALIZE BEMANS  -----------------------*
         * set deletion amount & batch size                                   *
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "init_bm")]
        public static extern void init_bm(int m0, int mb);

        /*-------------------------------  OBS  ------------------------------*
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "obs")]
        public static extern int obs(double y);

        /*------------------------------  CIVALS  ----------------------------*
        */
        [DllImport("./libsmpl.dylib", EntryPoint = "civals")]
        public static extern void civals(ref double mean, ref double hw, ref int nb);

        /*
         [DllImport("./libsmpl.dylib", EntryPoint = "")]
         public static extern 
         */

        [DllImport("./libsmpl.dylib", EntryPoint = "smpl")]
        public static extern void smpl(int id, string moduleName);

        [DllImport("./libsmpl.dylib", EntryPoint = "time")]
        public static extern double time();

        /*
         * Following function needs to be imported
         * 
            extern real Lq(), U(), B(), time();
            extern char* fname(), *mname();
            extern FILE* sendto();

            extern real uniform(), expntl(), erlang(), hyperx(), normal();
            extern long seed();

            void smpl(int, char*);
            int facility(char*, int);
            void schedule(int, real, int);
            void cause(int*, int*);
            int request(int, int, int);
            void release(int, int);
            void report();
            void error(int, char*);

            void init_bm(int, int);
            int obs(real);
            void civals(real*, real*, int*);
        *
        */
    }
}
