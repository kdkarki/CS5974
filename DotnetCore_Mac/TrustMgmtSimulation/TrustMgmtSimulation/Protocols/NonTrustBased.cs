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
        
        public override (Provider selectedProvider, List<Witness> witnesses, double selectedProviderActualWaitTime, double selectedProviderAdvertisedWaitTime, double selectedProviderProjectedWaitTime) DetermineMostEligibleProvider(List<Provider> providerList, double currentTime)
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
            if(leastBusyProviders.Count <= 1)
            {
                //if there are no providers then this will crash which should never happen :) 

                //Since there is no trust, the advertised and projected wait time will be the same
                return (leastBusyProviders[0], null, leastBusyProviders[0].GetCurrentActualWaitTime(currentTime),  selectedProviderAdvWaitTime, selectedProviderAdvWaitTime);
            }
            else
            {
                var randomlySelectedLeastBusyProvider = leastBusyProviders[SMPLWrapper.random(0, leastBusyProviders.Count - 1)];
                //Since there is no trust, the advertised and projected wait time will be the same
                return(randomlySelectedLeastBusyProvider, null, randomlySelectedLeastBusyProvider.GetCurrentActualWaitTime(currentTime), selectedProviderAdvWaitTime, selectedProviderAdvWaitTime);
            }
        }
    }
}