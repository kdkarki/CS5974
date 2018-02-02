using System;
using System.Collections.Generic;

namespace TrustMgmtSimulation.Entities
{
    public class Customer : IParticipant
    {
        #region Current Provider
        
        /// <summary>
        /// This class represents visit by a Customer to a Service Provider
        /// </summary>
        public class CurrentProviderVisit : Visit
        {
            public CurrentProviderVisit()
            {
                this.SelectedProvider = null;
                this.ActualWaitTime = -1.0;
                this.AdvertisedWaitTime = -1.0;
                this.ProjectedWaitTime = 1.0;
                this.QueueStartTime = -1.0;
                this.ServiceStartTime = -1.0;
            }
            public void Reset()
            {
                this.SelectedProvider = null;
                this.ActualWaitTime = -1.0;
                this.AdvertisedWaitTime = -1.0;
                this.ProjectedWaitTime = 1.0;
                this.QueueStartTime = -1.0;
                this.ServiceStartTime = -1.0;
            }

            public void SetServiceStartTime(double serviceStartTime)
            {
                this.ServiceStartTime = serviceStartTime;
                this.ActualWaitTime = this.ServiceStartTime - this.QueueStartTime;
            }

            public Visit GetVisit()
            {
                return new Visit(SelectedProvider, AdvertisedWaitTime, ProjectedWaitTime, QueueStartTime, ServiceStartTime);
            }
            
        }

        /*
        /// <summary>
        /// The history of all the visits by a customer to different service providers
        /// </summary>
        public class VisitHistory
        {

        }
        */
            
        #endregion

        private CurrentProviderVisit currentVisit = new CurrentProviderVisit();

        public int Id { get; private set; }
        
        public bool IsMalicious { get; set; }

        public List<Visit> VisitHistory { get; set; }

        public Customer(int id, bool isMalicious)
        {
            this.Id = id;
            this.IsMalicious = isMalicious;
            this.VisitHistory = new List<Visit>();
        }

        public void InstantiateCurrentVisit(Provider selectedProvider, double advertisedWaitTime, double projectedWaitTime, double queueStartTime)
        {
            currentVisit.SelectedProvider = selectedProvider;
            currentVisit.AdvertisedWaitTime = advertisedWaitTime;
            currentVisit.ProjectedWaitTime = projectedWaitTime;
            currentVisit.QueueStartTime = queueStartTime;
        }

        public void SetCurrentVisitServiceStartTime(double serviceStartTime)
        {
            if(currentVisit.SelectedProvider == null)
                throw new InvalidOperationException("Current visit SelectedProvider is null. Cannot set Service Start Time.");
            
            this.currentVisit.SetServiceStartTime(serviceStartTime);
        }

        //TODO: 
        //      1. Provider Visit History
        //      2. Provider Feedback History
        
    }
}