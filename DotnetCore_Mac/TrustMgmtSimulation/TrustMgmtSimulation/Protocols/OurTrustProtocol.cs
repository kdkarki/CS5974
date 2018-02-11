using TrustMgmtSimulation.Entities;
using System.Collections.Generic;

namespace TrustMgmtSimulation.Protocols
{
    public class OurTrustProtocol : ITrustProtocol
    {
        public (Provider selectedProvider, double selectedProviderActualWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime, double riskFactor)
        {
            return (null, 0.0);
        }
    }
}