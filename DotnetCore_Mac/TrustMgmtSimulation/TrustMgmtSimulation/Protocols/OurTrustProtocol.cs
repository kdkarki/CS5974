using TrustMgmtSimulation.Entities;
using System.Collections.Generic;

namespace TrustMgmtSimulation.Protocols
{
    public class OurTrustProtocol : TrustProtocol
    {
        public OurTrustProtocol(double riskFactor) : base(riskFactor)
        {

        }
        
        public override (Provider selectedProvider, double selectedProviderActualWaitTime, double selectedProviderProjectedWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime)
        {
            return (null, 0.0, 0.0);
        }
    }
}