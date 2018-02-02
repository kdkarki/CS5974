using System;
using System.Collections.Generic;

namespace TrustMgmtSimulation.Entities
{
    public class CentralHub
    {
        //TODO:
        //      1. List of providers
        //      2. List of customers currently active (in queue or in service)
        //          - those that have event 1 started but event 3 not done.
        //      3. Provide a method to instantiate providers and customers
        //          - read different arguments and parameters (hardcoded or user input) and do initial setup
        //          - may be not instantiate customers but definitely providers
        //          - even though not its actual job, central hub should setup malicious and non-malicous SP 
        //            and SR because this seems to be the best (single) place to do it

        List<Provider> serviceProviders = new List<Provider>();
        List<Customer> customers = new List<Customer>();

        public void InitializeProvidersAndCustomers(int providerCount, int queueLength, int customerCount, int maliciousPercent)
        {
            //initialize service providers
            initializeProviders(providerCount, queueLength, maliciousPercent);

            //initialize customers
            initializeCustomers(customerCount, maliciousPercent);
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

                serviceProviders.Add(provider);
            }

        }

        private void initializeCustomers(int customerCount, int maliciousPercent)
        {
            for(int c = 1; c <= customerCount; c++)
            {
                bool isMalicious = false;
                Customer customer = new Customer(c, isMalicious);

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

                customers.Add(customer);
            }
        }
    }
}