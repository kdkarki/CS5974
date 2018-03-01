using System;
using System.Collections.Generic;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation.Entities
{
    public class Visit
    {
        /// <summary>
            /// The service provider selected in event 2
            /// </summary>
            /// <returns></returns>
            public Provider SelectedProvider { get; private set; }

            /// <summary>
            /// The wait time advertised by the service provider 
            /// at the time of selection by the customer (event 2)
            /// </summary>
            /// <returns></returns>
            public double AdvertisedWaitTime { get; private set; }

            /// <summary>
            /// The projected wait time of the service provider 
            /// at the time of selection by the customer (event 2)
            /// </summary>
            /// <returns></returns>
            public double ProjectedWaitTime { get; private set; }

            /// <summary>
            /// The actual wait time of the service provider
            /// experienced by the customer when the customer
            /// is able to get successful service 'request'.
            /// A value of -1.0 means the customer has not 
            /// received the service and is waiting in queue.
            /// </summary>
            /// <returns></returns>
            public double ActualWaitTime { get; private set; }

            /// <summary>
            /// The time at which the customer gets into the queue
            /// to receive to receive service from the provider.
            /// There may be no queue which means the customer will
            /// be able to request service immediately
            /// </summary>
            /// <returns></returns>
            public double QueueStartTime { get; private set; }
            
            /// <summary>
            /// The time at which the customer was able to successfully
            /// request the service from the provider. The difference of
            /// ServiceStartTime and QueueStartTime is the actual wait time
            /// </summary>
            /// <returns></returns>
            public double ServiceStartTime { get; private set; } 

            /// <summary>
            /// The amount of time taken by the customer receiving the service
            /// </summary>
            /// <returns></returns>
            public double ServiceLength { get; set; }

            public List<Witness> Witnesses { get; private set; }
            

            public Visit()
            {
                this.SelectedProvider = null;
                this.ActualWaitTime = -1.0;
                this.AdvertisedWaitTime = -1.0;
                this.ProjectedWaitTime = 1.0;
                this.QueueStartTime = -1.0;
                this.ServiceStartTime = -1.0;
                this.ServiceLength = 0.0;
            }

            public Visit(Provider provider, double advWaitTime, double prjWaitTime, double qStarTime, double servStartTime, double serviceLength, List<Witness> witnesses = null)
            {
                this.SelectedProvider = provider;
                this.AdvertisedWaitTime = advWaitTime;
                this.ProjectedWaitTime = prjWaitTime;
                this.QueueStartTime = qStarTime;
                this.ServiceStartTime = servStartTime;
                if(servStartTime > -1.0) //this means the customer attempted to recieve service
                    this.ActualWaitTime = servStartTime - qStarTime;

                this.ServiceLength = serviceLength;
                
                if(witnesses != null)
                {
                    Witnesses = witnesses;
                }
            }

            /// <summary>
            /// A customer may get in queue but leave before receiving service from the provider
            /// because the customer waited more than the wait time threshold.
            /// In that case the customer should still record the wait time experienced 
            /// which is the wait time threshold.
            /// </summary>
            /// <param name="actualWaitTime">The actual wait time experienced by customer</param>
            public void SetActualWaitTime(double actualWaitTime)
            {
                this.ActualWaitTime = actualWaitTime;
            }

            /// <summary>
            /// The time at which the customer starts receiving the service from the service provider 
            /// during a visit
            /// </summary>
            /// <param name="serviceStartTime">The simulation time when the actual service started</param>
            public void SetServiceStartTime(double serviceStartTime)
            {
                this.ServiceStartTime = serviceStartTime;
                this.ActualWaitTime = this.ServiceStartTime - this.QueueStartTime;
            }

            public void SetServiceLength(double serviceLength)
            {
                this.ServiceLength = serviceLength;
            }

            public void Reset()
            {
                this.SelectedProvider = null;
                this.ActualWaitTime = -1.0;
                this.AdvertisedWaitTime = -1.0;
                this.ProjectedWaitTime = 1.0;
                this.QueueStartTime = -1.0;
                this.ServiceStartTime = -1.0;
                this.ServiceLength = 0.0;
            }
    }
}