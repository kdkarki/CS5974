using System.Collections.Generic;
using System.Linq;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Protocols
{
    public class NonTrustBased : TrustProtocol
    {
        public NonTrustBased(double riskFactor) : base(riskFactor)
        {

        }
        
        public override (Provider selectedProvider, double selectedProviderActualWaitTime, double selectedProviderProjectedWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime)
        {
            if(providerList == null || providerList.Count < 1)
            {
                throw new System.InvalidOperationException("Provider list cannot be null or empty when determining most eligible provider.");
            }
            List<Provider> leastBusyProviders = new List<Provider>();
            
            var selectedProviderAdvWaitTime = providerList[0].GetCurrentAdvertisedWaitTime(currentTime, RiskFactor);
            leastBusyProviders.Add(providerList[0]);
            for(int i = 1; i < providerList.Count; i++)
            {
                var advWaitTime = providerList[i].GetCurrentAdvertisedWaitTime(currentTime, RiskFactor);
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
                return (leastBusyProviders[0], selectedProviderAdvWaitTime, selectedProviderAdvWaitTime);
            }
            else
            {
                return(leastBusyProviders[SMPLWrapper.random(0, leastBusyProviders.Count - 1)], selectedProviderAdvWaitTime, selectedProviderAdvWaitTime);
            }
        }
    }
}