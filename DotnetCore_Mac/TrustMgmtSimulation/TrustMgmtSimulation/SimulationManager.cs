using System.Collections.Generic;
using System.Linq;
using TrustMgmtSimulation.Protocols;
using TrustMgmtSimulation.Entities;

namespace TrustMgmtSimulation
{
    public class SimulationManager
    {
        List<Provider> _serviceProviders = new List<Provider>();
        List<Customer> _customers = new List<Customer>();

        int _providerCount = 0, _queueLength = 0, _customerCount = 0, _maliciousPercent = 0;

        double _Ta = 0.0, _Ts = 0.0, _waitTimeThreshold = 0.0;

        ITrustProtocol _trustProtocol;

        /// <summary>
        /// Simulation manager parameterized constructor
        /// </summary>
        /// <param name="providerCount">Total number of service providers</param>
        /// <param name="queueLength">Total number of customer that can be serviced simultaneously by a service provider</param>
        /// <param name="customerCount">Total number of customers in the simulation</param>
        /// <param name="maliciousPercent">Total number of malicious actors (Providers and Customers) in the system</param>
        /// <param name="customerArrivalTime">Customer arrival average time</param>
        /// <param name="providerServiceTime">Average service time</param>
        /// <param name="waitTimeThreshold">The wait time beyound which the customer will abandon SP</param>
        /// <param name="trustProtocol">The type of trust protocol</param>
        public SimulationManager(int providerCount, int queueLength, int customerCount, int maliciousPercent, double customerArrivalTime, double providerServiceTime, double waitTimeThreshold, Protocols.ITrustProtocol trustProtocol)
        {
            _providerCount = providerCount;
            _queueLength = queueLength;
            _customerCount = customerCount;
            _maliciousPercent = maliciousPercent;
            _Ta = customerArrivalTime;
            _Ts = providerServiceTime;
            _waitTimeThreshold = waitTimeThreshold;
            _trustProtocol = trustProtocol;            
        }

        private void initializeProviders(int providerCount, int queueLength, int maliciousPercent)
        {
            for(int p = 0; p < providerCount; p++)
            {
                int pId = SMPLWrapper.facility($"SP{p}", queueLength);
                bool isMalicious = false;

                //determine if this provider should be malicious
                //also rather than selecting random provider to be malicious
                //a known set of providers will be set malicious based on
                //maliciousPercent so that the outcome is predictable

                //the expected maliciousness are 0%, 10%, 30%, and 50%

                if(maliciousPercent > 0)
                {
                    //SP1 is malicious for all cases
                    if(p == 1)
                        isMalicious = true;
                    else
                    {
                        if(maliciousPercent > 10)
                        {
                            if(p == 3 || p == 5)
                            {
                                //
                                isMalicious = true;
                            }
                        }
                        if(maliciousPercent == 50)
                        {
                            if(p == 7 || p == 9)
                            {
                                isMalicious = true;
                            }
                        }
                    }
                }

                Provider provider = new Provider(pId, isMalicious, queueLength);

                _serviceProviders.Add(provider);
            }

        }

        private void initializeCustomers(int customerCount, int maliciousPercent, double Ta)
        {
            for(int c = 1; c <= customerCount; c++)
            {
                bool isMalicious = false;                

                //determine if this customer should be malicious
                //(indxSR % 2 == 0) || (indxSR % 5 == 0) --> 60%
                //(indxSR % 5 == 0) || (indxSR % 9 == 0) --> 30% 
                //(indxSR % 5 == 0) --> 20%
                //(indxSR % 4 == 0 || indxSR % 5 == 0) --> 40%

                if(maliciousPercent == 10 
                    && c % 10 == 0)
                {
                    isMalicious = true;
                }
                else if(maliciousPercent == 30
                        && (c % 5 == 0 || c % 9 == 0))
                {
                    isMalicious = true;
                }
                else if(maliciousPercent == 50
                        && c % 2 == 0)
                {
                    isMalicious = true;
                } 

                Customer customer = new Customer(c, isMalicious);
                _customers.Add(customer);

                //schedule the customer for arrival
                SMPLWrapper.schedule(1, SMPLWrapper.expntl(Ta), customer.Id);
            }
        }

        /// <summary>
        /// Run simulation based on the parameter used during instantiation of Simulation Manager
        /// </summary>
        /// <param name="totalExecutionTime">Total simulation time in hours</param>
        public void RunSimulation(double totalExecutionTime)
        {
            
            //initialize SMPL
            SMPLWrapper.smpl(0, "5974: Non-Trust Based");
            //SMPLWrapper.trace(3);
            //initialize service providers
            initializeProviders(_providerCount, _queueLength, _maliciousPercent);

            //initialize customers
            initializeCustomers(_customerCount, _maliciousPercent, _Ta);
            /*
            int cToken = -1, simEvent = -1;
            
            while(SMPLWrapper.time() < totalExecutionTime)
            {
                var currentTime = SMPLWrapper.time();
                //cause an event to occur in smpl pipeline
                SMPLWrapper.cause(ref simEvent, ref cToken);
                Customer customer = _customers.First(c => c.Id == cToken);//Select the customer by Id
                switch(simEvent)
                {
                    case 1: //arrival of customer
                    //as soon a the customer arrives (in our case it means as soon as a customer is interested in finding a service provider)
                    //then schedule the customer for event 2, which is where the customer determines the best service provider and attempts to
                    //request service from the provider
                    SMPLWrapper.schedule(2, 0.0, cToken);
                    break;
                    case 2: //request server
                    // If the customer has selected a SP and was waiting in the queue and now it is rescheduled by smpl to event 2,
                    // then request service using the original SP fd ID.
                    // otherwise, the customer has just arrived and has not yet selected a SP for service
                    if(!customer.IsCurrentProviderSelected)// the customer has just arrived and has not selected a SP for service
                    {
                        var (selectedSP, selectedSPAdvWT, selectedSPPrjWT) = _trustProtocol.DetermineMostEligibleProvider(_serviceProviders, currentTime);

                        //TODO: If projected wait time is over a threshold then reschedule the customer for another visit
                        //      because there are no service providers available at this time
                        if(selectedSPPrjWT > _waitTimeThreshold)
                        {
                            //schedule customer for new arrival
                            SMPLWrapper.schedule(1, _Ta, cToken);
                        }
                        else
                        {
                            customer.InstantiateCurrentVisit(selectedSP, selectedSPAdvWT, selectedSPPrjWT, currentTime);
                        }
                    }
                    break;
                }
            }
            */
        }
    }
}