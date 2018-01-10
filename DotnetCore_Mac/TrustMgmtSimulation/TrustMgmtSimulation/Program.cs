using System;

namespace TrustMgmtSimulation
{
    public class Program
    {

        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            SMPLWrapper.smpl(0, "Test");

            Console.WriteLine(SMPLWrapper.time());
        }
    }
}
