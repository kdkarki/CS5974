using System.Collections.Generic;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Protocols
{
    public abstract class TrustProtocol : ITrustProtocol
    {
        public double RiskFactor { get; private set; }

        public abstract (Provider selectedProvider, double selectedProviderActualWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime);

        public TrustProtocol(double riskFactor)
        {
            this.RiskFactor = riskFactor;
        }
    }
}