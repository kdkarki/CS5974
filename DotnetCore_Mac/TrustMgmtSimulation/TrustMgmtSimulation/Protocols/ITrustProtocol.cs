using System.Collections.Generic;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Protocols
{
    public interface ITrustProtocol
    {
        double RiskFactor { get; }
        (Provider selectedProvider, List<Witness> witnesses, double selectedProviderActualWaitTime, double selectedProviderAdvertisedWaitTime, double selectedProviderProjectedWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime);
    }
}