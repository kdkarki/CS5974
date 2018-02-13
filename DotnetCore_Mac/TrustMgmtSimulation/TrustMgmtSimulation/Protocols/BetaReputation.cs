using TrustMgmtSimulation.Entities;
using System.Collections.Generic;

namespace TrustMgmtSimulation.Protocols
{
    public class BetaReputation : TrustProtocol
    {
        public BetaReputation(double riskFactor) : base(riskFactor)
        {

        }
        
        public override (Provider selectedProvider, List<Witness> witnesses, double selectedProviderActualWaitTime, double selectedProviderProjectedWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime)
        {            
            return (null, null, 0.0, 0.0);
        }
    }
}