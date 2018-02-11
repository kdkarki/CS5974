using System.Collections.Generic;
using System.Linq;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Protocols
{
    public class NonTrustBased : ITrustProtocol
    {
        public (Provider selectedProvider, double selectedProviderActualWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime, double riskFactor)
        {
            if(providerList == null || providerList.Count < 1)
            {
                throw new System.InvalidOperationException("Provider list cannot be null or empty when determining most eligible provider.");
            }
            List<Provider> leastBusyProviders = new List<Provider>();
            
            var selectedProviderAdvWaitTime = providerList[0].GetCurrentAdvertisedWaitTime(currentTime, riskFactor);
            leastBusyProviders.Add(providerList[0]);
            for(int i = 1; i < providerList.Count; i++)
            {
                var advWaitTime = providerList[i].GetCurrentAdvertisedWaitTime(currentTime, riskFactor);
                if(advWaitTime < selectedProviderAdvWaitTime)
                {
                    leastBusyProviders.Clear();
                    selectedProviderAdvWaitTime = advWaitTime;                    
                    leastBusyProviders.Add(providerList[i]);
                }
                else if(advWaitTime == selectedProviderAdvWaitTime)
                {
                    leastBusyProviders.Add(providerList[i]);
                }
            }
            if(leastBusyProviders.Count == 0)
            {
                return (leastBusyProviders[0], selectedProviderAdvWaitTime);
            }
            else
            {
                return(leastBusyProviders[SMPLWrapper.random(0, leastBusyProviders.Count - 1)], selectedProviderAdvWaitTime);
            }
        }
    }
}