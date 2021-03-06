﻿using System;
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
                      riskFactor = 1.0,             //the wait time offset reported by a malicious SP
                      customerArrivalTime = 10.00,  //T_a: customer arrival time
                      providerServiceTime = 1.0,    //T_s: service provider service time
                      waitTimeThreshold = 1.0;      //A customer will abandon a SP if wait time is over this threshold
        static int reportNumber = 1,                //the report to be generated
                   maliciousPercent = 30;           //the malicious SRs and SPs. Parameter P_m

        static DecisionPolicyType decisionPolicy = DecisionPolicyType.LeastBusyAmongMostTrustworthy;
        static Protocols.ITrustProtocol trustProtocol = new Protocols.OurTrustProtocol(riskFactor);

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

            SimulationManager simMgr = new SimulationManager(10, 20, 2200, maliciousPercent, customerArrivalTime, providerServiceTime, waitTimeThreshold, trustProtocol);
            
            simMgr.RunSimulation(totalExecutionTime);

        }

        private static void SetParameters(string[] args)
        {
            bool isReportSet = false, 
            isTotalExecutionTimeSet = false, 
            isParamRSet = false, 
            isMaliciousnessSet = false, 
            isDecisionPolicySet = false,
            isTrustProtocolSet = false;

            int reportIndex = Array.IndexOf(args, "-r") == -1 ? Array.IndexOf(args, "-R") : Array.IndexOf(args, "-r");
            int exeTimeIndex = Array.IndexOf(args, "-t") == -1 ? Array.IndexOf(args, "-T") : Array.IndexOf(args, "-t");
            int paramRIndex = Array.IndexOf(args, "-f") == -1 ? Array.IndexOf(args, "-F") : Array.IndexOf(args, "-f");
            int maliciousIndex = Array.IndexOf(args, "-m") == -1 ? Array.IndexOf(args, "-M") : Array.IndexOf(args, "-m");
            int decisionPolicyIndex = Array.IndexOf(args, "-d") == -1 ? Array.IndexOf(args, "-D") : Array.IndexOf(args, "-d");
            int acceptDefaultIndex = Array.IndexOf(args, "-y") == -1 ? Array.IndexOf(args, "-Y") : Array.IndexOf(args, "-y");
            int trustProtocolIndex = Array.IndexOf(args, "-p") == -1 ? Array.IndexOf(args, "-P") : Array.IndexOf(args, "-p");


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
                if(Int32.TryParse(args[maliciousIndex + 1], out maliciousPercent))
                {
                    isMaliciousnessSet = true;
                }
            }
            if(decisionPolicyIndex != -1)
            {
                isDecisionPolicySet = true;
                switch(args[decisionPolicyIndex + 1].ToLower())
                {
                    case "lbsp": 
                    decisionPolicy = DecisionPolicyType.LeastBusySP;
                    break;
                    case "mtsp":
                    decisionPolicy = DecisionPolicyType.MostTrustworthySP;
                    break;
                    case "lbmt":
                    decisionPolicy = DecisionPolicyType.LeastBusyAmongMostTrustworthy;
                    break;
                    default:
                    isDecisionPolicySet = false;
                    break;

                }
            }
            if(trustProtocolIndex != -1)
            {
                isTrustProtocolSet = true;
                switch(args[trustProtocolIndex + 1].ToLower())
                {
                    case "o": case "our":
                    trustProtocol = new Protocols.OurTrustProtocol(riskFactor);
                    break;
                    case "b": case "beta":
                    trustProtocol = new Protocols.BetaReputation(riskFactor);
                    break;
                    case "n": case "notrust":
                    trustProtocol = new Protocols.NonTrustBased(riskFactor);
                    break;
                    default:
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
                isTrustProtocolSet = true;
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
                }
                if(!isMaliciousnessSet)
                {
                    Console.WriteLine(String.Format("\tm: Maliciousness. Default value: {0}", maliciousPercent));
                }
                if(!isParamRSet)
                {
                    Console.WriteLine(String.Format("\tf: Risk Factor. Default value: {0}", riskFactor));
                }
                if(!isTotalExecutionTimeSet)
                {
                    Console.WriteLine(String.Format("\tt: Total Execution Time (hrs). Default value: {0}", totalExecutionTime));
                }
                if(!isTrustProtocolSet)
                {
                    Console.WriteLine(String.Format("\tp: Trust Protocol. Default value: {0}", trustProtocol.ToString()));
                }
                

                Console.WriteLine("\ty: Accept default values");

                char userSelection = 'y';
                if(!System.Diagnostics.Debugger.IsAttached)
                    userSelection = Console.ReadKey().KeyChar;
                Console.WriteLine();
                var possibleValues = new []{'d', 'D', 'f', 'F', 'm', 'M', 'p', 'P', 'r', 'R', 't', 'T', 'y', 'Y'};
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
                        Console.WriteLine("Enter Maliciousness value. Accepted values are 0, 10, 30, and 50.");
                        string malUserInput = Console.ReadLine();
                        int mal = -1;
                        if(!Int32.TryParse(malUserInput, out mal) || (!new [] {0, 10, 30, 50}.Any(mal.Equals)))
                        {
                            Console.WriteLine("Enter a correct maliciouness value.");
                            continue;
                        }
                        maliciousPercent = mal;
                        isMaliciousnessSet = true;
                        break;
                    }
                    break;
                    case 'p': case 'P':
                    while(true)
                    {
                        Console.WriteLine("Enter Trust Protocol.");
                        Console.WriteLine("\tn or notrust: Non-Trust Based Protocol");
                        Console.WriteLine("\tb or beta: Josang's Beta Reputation System");
                        Console.WriteLine("\to or our: Our Proposed Trust Protocol");
                        string protocolUserInput = Console.ReadLine();
                        isTrustProtocolSet = true;
                        switch(protocolUserInput.ToLower())
                        {
                            case "o": case "our":
                            trustProtocol = new Protocols.OurTrustProtocol(riskFactor);
                            break;
                            case "b": case "beta":
                            trustProtocol = new Protocols.BetaReputation(riskFactor);
                            break;
                            case "n": case "notrust":
                            trustProtocol = new Protocols.NonTrustBased(riskFactor);
                            break;
                            default:
                            isTrustProtocolSet = false;
                            Console.WriteLine("Enter corrent trust protocol");
                            break;
                        }
                        if(isTrustProtocolSet)
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
            Console.WriteLine(String.Format("\tReport (figure) #: {0}\n\tTrust Protocol: {1}\n\tDecision Policy: {2}\n\tMaliciousness % (P_m): {3}\n\tTotal Simulation Time (hrs): {4:0.00}\n\tRisk Factor (R_f): {5:0.00}\n\n",
                                            reportNumber, trustProtocol.ToString(), decisionPolicy.ToString(), maliciousPercent, totalExecutionTime, riskFactor));
        } 
    }
}
