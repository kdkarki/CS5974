using System;
using System.Collections.Generic;
using System.Linq;

namespace TrustMgmtSimulation
{
    public class Program
    {

        static void Main(string[] args)
        {
            (new List<ISimulation>(){new MMC1(), new MM1_Bmeans()})
            .ForEach(s => {s.Run(); SMPLWrapper.reset(); });
        }
    }
}
