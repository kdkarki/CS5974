using System.Collections.Generic;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Protocols
{
    public interface ITrustProtocol
    {
        (Provider selectedProvider, double selectedProviderActualWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime, double riskFactor);
    }
}