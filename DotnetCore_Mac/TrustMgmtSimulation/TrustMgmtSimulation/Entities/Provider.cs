using System;

namespace TrustMgmtSimulation.Entities
{
    public class Provider : IParticipant
    {
        public int Id { get; private set; }
        
        public bool IsMalicious { get; set; }

        public double TrustScore { get; private set; }

        public int TotalVisitors { get; private set; }

        public double NextAvailTime { get; private set; }

        private double[] ServiceSlots { get; set; }

        public Provider(int id, bool isMalicious, int queueLength)
        {
            this.Id = id;
            this.IsMalicious = isMalicious;
            this.TrustScore = 0.5;//initial trust score should be 50%
            this.TotalVisitors = 0;
            this.NextAvailTime = 0.0;
            this.ServiceSlots = new double[queueLength];
            for(int aSlot = 0; aSlot < queueLength; aSlot++)
            {
                ServiceSlots[aSlot] = 0.0;
            }      
        }

        public double GetCurrentActualWaitTime(double currentTime)
        {
            return (NextAvailTime - currentTime);
        }

        public double GetCurrentAdvertisedWaitTime(double currentTime, double riskFactor)
        {
            double waitTimeMultiplier = 1.0, advertisedWaitTime = 0.0;
            if(this.IsMalicious)
                waitTimeMultiplier = (1.0 - riskFactor);
            
            advertisedWaitTime = waitTimeMultiplier * GetCurrentActualWaitTime(currentTime);

            if(advertisedWaitTime < 0.0167)
            {
                advertisedWaitTime = 0.0167;//minimum wait time is 1 minute
            }

            return advertisedWaitTime;
        }

        public void AddCustomerToServiceQueue(Customer customer, double currentTime)
        {
            if(customer == null)
                throw new System.ArgumentNullException("Null Customer cannot be added to service provider queue");
            if(customer.CurrentVisitServiceLength == 0.0)
                throw new System.ArgumentNullException("Customer sservice length cannot be 0.0");

            throw new System.NotImplementedException("When a customer selects a service provider then the customer needs to be added to the provider's service queue.");
        }

        public void RemoveAbandonedCustomerFromQueue(Customer customer, double currentTime)
        {
            if(customer == null)
                throw new System.ArgumentNullException("Null Customer cannot be added to service provider queue");
            throw new System.NotImplementedException();
        }
                
    }
}