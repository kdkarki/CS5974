using System.Collections.Generic;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Protocols
{
    public interface ITrustProtocol
    {
        double RiskFactor { get; }
        (Provider selectedProvider, double selectedProviderActualWaitTime, double selectedProviderProjectedWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime);
    }
}