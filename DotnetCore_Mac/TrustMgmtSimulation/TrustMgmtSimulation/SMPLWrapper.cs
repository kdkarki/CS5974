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

        /* ------------------------------------------------------------------ */

        /*
         * Function from smpl.c
         */


        /*---------------  INITIALIZE SIMULATION SUBSYSTEM  ------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "smpl")]
        public static extern void smpl(int id, string moduleName);

        /*---------------------------  SAVE NAME  ----------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "reset")]
        public static extern void reset();

        /*---------------------------  SAVE NAME  ----------------------------*/
        //TODO: This function is defined as static int save_name(char *s, int m)
        //      So not sure if this is the right way to p/invoke this.
        [DllImport("./libsmpl.dylib", EntryPoint = "save_name")]
        public static extern int save_name(string s, int m);

        /*-------------------------  GET MODEL NAME  -------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "mname")]
        public static extern string mname();

        /*------------------------  GET FACILITY NAME  -----------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "fname")]
        public static extern string fname(int f);

        //TODO: Following function not wrapped because they are defined static
        //      get_blk : GET BlOCK
        //      get_elm : GET ELEMENT
        //      put_elm : RETURN ELEMENT

        /*-------------------------  SCHEDULE EVENT  -------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "schedule")]
        public static extern void schedule(int ev, double te, int tkn);

        /*---------------------------  CAUSE EVENT  --------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "cause")]
        public static extern void cause(ref int ev, ref int tkn);

        /*--------------------------  RETURN TIME  ---------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "time")]
        public static extern double time();

        /*--------------------------  CANCEL EVENT  --------------------------*/
        /// <summary>
        /// Cancel the specified event
        /// </summary>
        /// <returns>The id of the token (customer)</returns>
        /// <param name="ev">Event id.</param>
        [DllImport("./libsmpl.dylib", EntryPoint = "cancel")]
        public static extern int cancel(int ev);

        /*-------------------------  SUSPEND EVENT  --------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "suspend")]
        public static extern int suspend(int tkn);

        //TODO: static void enlist(int *head, int elm) not wrapped

        /*-------------------------  DEFINE FACILITY  ------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "facility")]
        public static extern int facility(string s, int n);

        //TODO: static void resetf() not wrapped

        /*------------------------  REQUEST FACILITY  ------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "request")]
        public static extern int request(int f, int tkn, int pri);

        //TODO: static void enque(int f, int j, int pri, int env, real te) nw/

        /*------------------------  PREEMPT FACILITY  ------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "preempt")]
        public static extern int preempt(int f, int tkn, int pri);

        /*------------------------  RELEASE FACILITY  ------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "release")]
        public static extern void release(int f, int tkn);

        /*-----------------------  GET FACILITY STATUS  ----------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "status")]
        public static extern int status(int f);

        /*--------------------  GET CURRENT QUEUE LENGTH  --------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "inq")]
        public static extern int inq(int f);

        /*--------------------  GET FACILITY UTILIZATION  --------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "U")]
        public static extern double U(int f);

        /*----------------------  GET MEAN BUSY PERIOD  ----------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "B")]
        public static extern double B(int f);

        /*--------------------  GET AVERAGE QUEUE LENGTH  --------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "Lq")]
        public static extern double Lq(int f);

        /*-----------------------  TURN TRACE ON/OFF  ------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "trace")]
        public static extern void trace(int n);

        /*-----------------------------  PAUSE  ------------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "smpl_pause")]
        public static extern void smpl_pause();

        /*------------------  DISPLAY ERROR MESSAGE & EXIT  ------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "error")]
        public static extern void error(int n, string s);

        /*------------------------  GENERATE REPORT  -------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "report")]
        public static extern void report();

        /*--------------------  GENERATE FACILITY REPORT  --------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "reportf")]
        public static extern void reportf();

        /*---------------------------  COUNT LINES  --------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "lns")]
        public static extern int lns(int i);

        /*----------------------------  END PAGE  ----------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "endpage")]
        public static extern void endpage();

        /*----------------------------  NEW PAGE  ----------------------------*/
        [DllImport("./libsmpl.dylib", EntryPoint = "newpage")]
        public static extern void newpage();

        //TODO: File *sendto(FILE *dest) -> need to be wrapped
    }
}
