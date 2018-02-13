using TrustMgmtSimulation.Entities;
using System.Collections.Generic;

namespace TrustMgmtSimulation.Protocols
{
    public class BetaReputation : TrustProtocol
    {
        public BetaReputation(double riskFactor) : base(riskFactor)
        {

        }
        
        public override (Provider selectedProvider, double selectedProviderActualWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime)
        {            
            return (null, 0.0);
        }
    }
}