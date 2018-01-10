using System;
using System.Runtime.InteropServices;

namespace TrustMgmtSimulation
{
    public class SMPLWrapper
    {
        public SMPLWrapper()
        {
            
        }

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
