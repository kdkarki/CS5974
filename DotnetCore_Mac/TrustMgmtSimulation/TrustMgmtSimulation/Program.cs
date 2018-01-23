using System;
using System.Collections.Generic;
using System.Linq;

namespace TrustMgmtSimulation
{
    public enum DecisionPolicyType
    {
        LeastBusySP,
        MostTrustworthySP,
        LeastBusyAmongMostTrustworthy
    }
    public class Program
    {
        //set the variables with default values
        static double totalExecutionTime = 1015.00, //total time the simulation runs
                      riskFactor = 1.0;             //the wait time offset reported by a malicious SP
        static int reportNumber = 1,                //the report to be generated
                   maliciouPercent = 30;            //the malicious SRs and SPs. Parameter P_m

        static DecisionPolicyType decisionPolicy = DecisionPolicyType.LeastBusyAmongMostTrustworthy;

        static void Main(string[] args)
        {
            /*
            (new List<ISimulation>(){new MMC1(), new MM1_Bmeans()})
            .ForEach(s => {s.Run(); SMPLWrapper.reset(); });
            */

            Console.WriteLine("******************************************************************");
            Console.WriteLine("*\tStarting the simulation for CS5974");
            Console.WriteLine("*\tTrust-based Service Community Management: A Case Study");
            Console.WriteLine("******************************************************************");

            
            SetParameters(args);
            

        }

        private static void SetParameters(string[] args)
        {
            bool isReportSet = false, 
            isTotalExecutionTimeSet = false, 
            isParamRSet = false, 
            isMaliciousnessSet = false, 
            isDecisionPolicySet = false;

            int reportIndex = Array.IndexOf(args, "-r") == -1 ? Array.IndexOf(args, "-R") : -1;
            int exeTimeIndex = Array.IndexOf(args, "-t") == -1 ? Array.IndexOf(args, "-T") : -1;
            int paramRIndex = Array.IndexOf(args, "-f") == -1 ? Array.IndexOf(args, "-F") : -1;
            int maliciousIndex = Array.IndexOf(args, "-m") == -1 ? Array.IndexOf(args, "-M") : -1;
            int decisionPolicyIndex = Array.IndexOf(args, "-d") == -1 ? Array.IndexOf(args, "-D") : -1;
            int acceptDefaultIndex = Array.IndexOf(args, "-y") == -1 ? Array.IndexOf(args, "-Y") : -1;


            if(reportIndex != -1)
            {
                if(Int32.TryParse(args[reportIndex + 1], out reportNumber))
                {
                    isReportSet = true;
                }
            }
            if(exeTimeIndex != -1)
            {
                if(Double.TryParse(args[exeTimeIndex + 1], out totalExecutionTime))
                {
                    isTotalExecutionTimeSet = true;
                }
            }
            if(paramRIndex != -1)
            {
                if(Double.TryParse(args[paramRIndex + 1], out riskFactor))
                {
                    isParamRSet = true;
                }
            }
            if(maliciousIndex != -1)
            {
                if(Int32.TryParse(args[maliciousIndex + 1], out maliciouPercent))
                {
                    isMaliciousnessSet = true;
                }
            }
            if(decisionPolicyIndex != -1)
            {
                isDecisionPolicySet = true;
                switch(args[decisionPolicyIndex + 1])
                {
                    case "LBSP":
                    decisionPolicy = DecisionPolicyType.LeastBusySP;
                    break;
                    case "MTSP":
                    decisionPolicy = DecisionPolicyType.MostTrustworthySP;
                    break;
                    case "LBMT":
                    decisionPolicy = DecisionPolicyType.LeastBusyAmongMostTrustworthy;
                    break;
                    default:
                    isDecisionPolicySet = false;
                    break;

                }
            }

            SummarizeSimulationConfig();

            if(acceptDefaultIndex > -1)
            {
                isDecisionPolicySet = true;
                isParamRSet = true;
                isMaliciousnessSet = true;
                isReportSet = true;
                isTotalExecutionTimeSet = true;
                return;
            }

            while(!isReportSet || !isDecisionPolicySet || !isMaliciousnessSet || !isParamRSet || !isTotalExecutionTimeSet)
            {
                Console.WriteLine("Select an option to set a parameter value:");
                if(!isReportSet)
                {
                    Console.WriteLine(String.Format("\tr: Report Number. Default value: {0}", reportNumber));
                }
                if(!isDecisionPolicySet)
                {
                    Console.WriteLine(String.Format("\td: Decision Policy. Default value: {0}", decisionPolicy.ToString()));
                }if(!isMaliciousnessSet)
                {
                    Console.WriteLine(String.Format("\tm: Maliciousness. Default value: {0}", maliciouPercent));
                }if(!isParamRSet)
                {
                    Console.WriteLine(String.Format("\tf: Risk Factor. Default value: {0}", riskFactor));
                }if(!isTotalExecutionTimeSet)
                {
                    Console.WriteLine(String.Format("\tt: Total Execution Time (hrs). Default value: {0}", totalExecutionTime));
                }

                Console.WriteLine("\ty: Accept default values");

                char userSelection = Console.ReadKey().KeyChar;
                Console.WriteLine();
                var possibleValues = new []{'d', 'D', 'f', 'F', 'm', 'M', 'r', 'R', 't', 'T', 'y', 'Y'};
                if(!possibleValues.Contains(userSelection))
                {
                    Console.WriteLine("Select a correct option!");
                    continue;
                }

                switch(userSelection)
                {
                    case 'd': case 'D':
                    while(true)
                    {
                        Console.WriteLine("Enter decision policy. \nOptions:");
                        Console.WriteLine("\tLBSP (Least Busy Service Provider)");
                        Console.WriteLine("\tMTSP (Most Trustworthy Service Provider)");
                        Console.WriteLine("\tLBMT (Least Busy among  Most Trustworthy)");
                        string dpolUserInput = Console.ReadLine();
                        if(!(new []{"lbsp", "mtsp", "lbmt"}).Contains(dpolUserInput.ToLower()))
                        {
                            Console.WriteLine("Invalid option entered. Enter a valid option from the list.");
                            continue;
                        }
                        isDecisionPolicySet = true;
                        switch(dpolUserInput)
                        {
                            case "LBSP":
                            decisionPolicy = DecisionPolicyType.LeastBusySP;
                            break;
                            case "MTSP":
                            decisionPolicy = DecisionPolicyType.MostTrustworthySP;
                            break;
                            case "LBMT":
                            decisionPolicy = DecisionPolicyType.LeastBusyAmongMostTrustworthy;
                            break;
                            default:
                            break;
                        }
                        break;
                    }
                    break;
                    case 'f': case 'F':
                    while(true)
                    {
                        Console.WriteLine("Enter Risk Factor value. Accepted range is 0.0 - 1.0.");
                        string rfUserInput = Console.ReadLine();
                        double rFactor = -1.0;
                        if(!Double.TryParse(rfUserInput, out rFactor) || rFactor < 0.0 || rFactor > 1.0)
                        {
                            Console.WriteLine("Enter a correct risk factor value between 0.0 and 1.0.");
                            continue;
                        }
                        riskFactor = rFactor;
                        isParamRSet = true;
                        break;
                    }
                    break;
                    case 'm': case 'M':
                    while(true)
                    {
                        Console.WriteLine("Enter Maliciousness value. Accepted range is 0 - 100.");
                        string malUserInput = Console.ReadLine();
                        int mal = -1;
                        if(!Int32.TryParse(malUserInput, out mal) || mal < 0 || mal > 100)
                        {
                            Console.WriteLine("Enter a correct maliciouness value between 0 and 100.");
                            continue;
                        }
                        maliciouPercent = mal;
                        isMaliciousnessSet = true;
                        break;
                    }
                    break;
                    case 'r': case 'R':
                    while(true)
                    {
                        Console.WriteLine("Enter Report number. Accepted range is 1 - 8.");
                        string repUserInput = Console.ReadLine();
                        int rep = -1;
                        if(!Int32.TryParse(repUserInput, out rep) || rep < 0 || rep > 8)
                        {
                            Console.WriteLine("Enter a correct report number between 0 and 8.");
                            continue;
                        }
                        reportNumber = rep;
                        isReportSet = true;
                        break;
                    }
                    break;
                    case 't': case 'T':
                    while(true)
                    {
                        Console.WriteLine("Enter Total Simulation run time. Accepted value is >= 1.0");
                        string teUserInput = Console.ReadLine();
                        double execTime = -1.0;
                        if(!Double.TryParse(teUserInput, out execTime) || execTime < 1.0)
                        {
                            Console.WriteLine("Enter a correct simlation run time value >= 1.0.");
                            continue;
                        }
                        totalExecutionTime = execTime;
                        isTotalExecutionTimeSet = true;
                        break;
                    }
                    break;
                    case 'y': case 'Y':
                    isDecisionPolicySet = true;
                    isParamRSet = true;
                    isMaliciousnessSet = true;
                    isReportSet = true;
                    isTotalExecutionTimeSet = true;
                    break;
                }
            }

            SummarizeSimulationConfig();
        }

        private static void SummarizeSimulationConfig()
        {
            Console.WriteLine("Simulation configuration:");
            Console.WriteLine(String.Format("\tReport (figure) #: {0}\n\tDecision Policy: {1}\n\tMaliciousness % (P_m): {2}\n\tTotal Simulation Time (hrs): {3:0.00}\n\tRisk Factor (R_f): {4:0.00}\n\n",
                                            reportNumber, decisionPolicy.ToString(), maliciouPercent, totalExecutionTime, riskFactor));
        } 
    }
}
