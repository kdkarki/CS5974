using System;
using System.Collections.Generic;
using System.Linq;

namespace TrustMgmtSimulation.Entities
{
    public class ServiceUnit
    {
        /// <summary>
        /// The time the server will be available to server a new customer
        /// </summary>
        /// <returns></returns>
        public double NextAvailableTime { get; set; }

        /// <summary>
        /// The customer currently being served and the time the service of the customer ends
        /// </summary>
        /// <returns></returns>
        public KeyValuePair<Customer, double> CurrentCustomerEndTime { get; set; }

        /// <summary>
        /// All the customers queued to receive service from service unit
        /// This does not include the customer currently in service
        /// </summary>
        /// <returns></returns>
        public Dictionary<Entities.Customer, double> QueuedCustomerServiceTime { get; set; }

        public ServiceUnit()
        {
            NextAvailableTime = 0.0;
            QueuedCustomerServiceTime = new Dictionary<Customer, double>();
        }

        public bool AddCustomerToQueue(Customer customer, double customerServiceTime)
        {
            bool isSuccessfullAdded = false;

            if(customer != null)
            {
                QueuedCustomerServiceTime.Add(customer, customerServiceTime);
                isSuccessfullAdded = true;
            }

            return isSuccessfullAdded;
        }

        public bool StartServiceForCustomer(Customer customer, double currentTime)
        {
            bool isServiceSuccessfullyStarted = false;

            if(QueuedCustomerServiceTime != null && QueuedCustomerServiceTime.Count > 0 
                && QueuedCustomerServiceTime.ContainsKey(customer))
                {
                    CurrentCustomerEndTime = KeyValuePair.Create(customer, (currentTime + QueuedCustomerServiceTime[customer]));
                    QueuedCustomerServiceTime.Remove(customer);
                    //TODO: Verify the calculation of next available time is correct
                    //      because queued customer service time should be in hours (it is) and when summed with 'currentTime'
                    //      it should be give correct next available time
                    NextAvailableTime = currentTime + QueuedCustomerServiceTime.Sum(c => c.Value);
                    isServiceSuccessfullyStarted = true;
                }

            return isServiceSuccessfullyStarted;
        }
    }
}