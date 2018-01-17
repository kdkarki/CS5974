using System;

namespace TrustMgmtSimulation.Entities
{
    public class Customer : IParticipant
    {
        #region Current Provider
        
        /// <summary>
        /// This class represents visit by a Customer to a Service Provider
        /// </summary>
        public class CustomerCurrentVisit
        {
            /// <summary>
            /// The service provider selected in event 2
            /// </summary>
            /// <returns></returns>
            public Provider SelectedProvider { get; set; }

            /// <summary>
            /// The wait time advertised by the service provider 
            /// at the time of selection by the customer (event 2)
            /// </summary>
            /// <returns></returns>
            public double AdvertisedWaitTime { get; set; }

            /// <summary>
            /// The projected wait time of the service provider 
            /// at the time of selection by the customer (event 2)
            /// </summary>
            /// <returns></returns>
            public double ProjectedWaitTime { get; set; }

            /// <summary>
            /// The actual wait time of the service provider
            /// experienced by the customer when the customer
            /// is able to get successful service 'request'.
            /// A value of -1.0 means the customer has not 
            /// received the service and is waiting in queue.
            /// </summary>
            /// <returns></returns>
            public double ActualWaitTime { get; set; }

            /// <summary>
            /// The time at which the customer gets into the queue
            /// to receive to receive service from the provider.
            /// There may be no queue which means the customer will
            /// be able to request service immediately
            /// </summary>
            /// <returns></returns>
            public double QueueStartTime { get; set; }
            
            /// <summary>
            /// The time at which the customer was able to successfully
            /// request the service from the provider. The difference of
            /// ServiceStartTime and QueueStartTime is the actual wait time
            /// </summary>
            /// <returns></returns>
            public double ServiceStartTime { get; set; }
            
            public CustomerCurrentVisit()
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
            
        }
            
        #endregion

        private CustomerCurrentVisit currentVisit = new CustomerCurrentVisit();

        public int Id { get; set; }
        
        public bool IsMalicious { get; set; }

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
            
            currentVisit.ServiceStartTime = serviceStartTime;
            currentVisit.ActualWaitTime = currentVisit.ServiceStartTime - currentVisit.QueueStartTime;
        }

        //TODO: 
        //      1. Provider Visit History
        //      2. Provider Feedback History
        
    }
}