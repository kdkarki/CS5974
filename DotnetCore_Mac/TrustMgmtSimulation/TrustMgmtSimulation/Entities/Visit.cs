
namespace TrustMgmtSimulation.Entities
{
    public class Visit
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
            public double ActualWaitTime { get; internal set; }

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
            public double ServiceStartTime { get; internal set; } 

            public Visit()
            {
                
            }

            public Visit(Provider provider, double advWaitTime, double prjWaitTime, double qStarTime, double servStartTime)
            {
                this.SelectedProvider = provider;
                this.AdvertisedWaitTime = advWaitTime;
                this.ProjectedWaitTime = prjWaitTime;
                this.QueueStartTime = qStarTime;
                this.ServiceStartTime = servStartTime;
                if(qStarTime > -1.0) //this means the customer attempted to recieve service
                    this.ActualWaitTime = servStartTime - qStarTime;
            }
    }
}