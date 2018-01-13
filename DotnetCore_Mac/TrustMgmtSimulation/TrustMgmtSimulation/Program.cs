using System;
using System.Collections.Generic;
using System.Linq;

namespace TrustMgmtSimulation
{
    public class Program
    {

        static void Main(string[] args)
        {
            List<ISimulation> objSimulationArray = new List<ISimulation>(){new MMC1(), new MM1_Bmeans()};
            objSimulationArray.ForEach(s => {s.Run();});
        }
    }
}
