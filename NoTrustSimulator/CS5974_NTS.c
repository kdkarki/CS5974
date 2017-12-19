#include <math.h>
#include <mem.h>
#include <stdlib.h>
#include "smpl.h"
#include "cs5974_NTS.h"

int	currentReportNumber	= 7;
int maliciousPercent = 30;
double Param_R = 1.0;

int totalSPVisitsByGoodSRs, totalMaliciousSPVisitsByGoodSRs;

struct ServiceProvider providerArray[N_SP];
struct ServiceRequester customerArray[N_SR];
int event, customer, reportingCustomer;

int quickTest(){
    int result = 0;
    return result;
}

void setupMaliciousSPSR()
{
    int indxSP, indxSR;

    for(indxSP = 0; indxSP < N_SP; indxSP++)
    {
        if(indxSP == 1 || indxSP == 3 || indxSP == 5)// || indxSP == 7 || indxSP == 9)
        {
            providerArray[indxSP].isMalicious = 1;
        }
    }

    for(indxSR = 0; indxSR < N_SR; indxSR++)
    {
        if((indxSR % 5 == 0) || (indxSR % 9 == 0))//(indxSR % 2 == 0) || (indxSR % 5 == 0)60%//(indxSR % 5 == 0) || (indxSR % 9 == 0)//30% //(indxSR % 5 == 0)//20%//(indxSR % 4 == 0 || indxSR % 5 == 0)//40%
        {
            customerArray[indxSR].isMalicious = 1;
        }
        else
        {
            customerArray[indxSR].isMalicious = 0;
        }
    }
}

/**
 * First argument is figure/report number to run. Accepted Values: 7 - 8
 * Second argument is malicious percentage and is optional. This is 30% by default. Accepted values: 0%, 10%, 30%, 50%
 * Third argument is total running time as double value. This is default to 1010.00 hrs
 * Fourth argument is risk factor. Accepted values: 1.0, 0.75, 0.5
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    printf("\n\n******************************************************\n\nStarting the simulation for CS5974\nNon-Trust Based Simulation\n\n******************************************************\n\n");
    
    real te = 1015.00; //this is end time for simulation

    if(argc > 1)
    {
        currentReportNumber = atoi(argv[1]);
    }
    if(argc > 2)
    {
        maliciousPercent = atoi(argv[2]);
    }

    if(argc > 3)
    {
        sscanf(argv[3], "%lf", &te);
    }

    if(argc > 4)
    {
        sscanf(argv[4], "%lf", &Param_R);
    }

    printf("Simulation configuration:\n\tReport (figure) #: %d\n\tMaliciousness %% (P_m): %d\n\tTotal Simulation Time (hrs): %.2f\n\tRisk Factor (R_f): %.2f\n\n",
           currentReportNumber, maliciousPercent, te, Param_R);
    srand(time(NULL));

    real report1LastPrinted;
    report1LastPrinted = -5.0;

    smpl(0,"5974"); //initialize smpl simulation
    //trace(2);
    setupServiceProviders();
    setupServiceRequesters();
    setupMaliciousSPSR();

    //initial report setup
    switch(currentReportNumber)
    {
        case 7:
            printf("Current Time\tTotal SP Visits Since Last Count\t Total Malicious SP Visits Since Last Count\tMalicious SP Visit Percentage\tTotal SP Visits By Good SRs\tTotal Malicious SP Visits by Good SRs\n");

            break;
        case 8:

            break;
        default:
            break;
    }

    while(time() < te)
    {
        /*if(quickTest() == -1) {
            return -1;
        }*/
        real currentTime = time();
        switch(currentReportNumber)
        {
            case 7:
                figure7Report(&report1LastPrinted, currentTime);
                break;
            case 8:
                if(event == 2 && customerArray[customer].isMalicious == 0) {
                    figure8Report(&report1LastPrinted, currentTime);
                }
                break;
            default:
                printf("Enter either 7 or 8 for the report to be generated.");
                return -1;
        }
        cause(&event, &customer);
        switch(event)
        {
            case 1: //arrival of a customer
                schedule(2, 0.0, customer); //immediately schedule the customer for server request
                break;

            case 2: //request server
                // If the customer has selected a SP and was waiting in the queue and now it is rescheduled by smpl to event 2,
                // then request service using the original SP fd ID.
                // otherwise, the customer has just arrived and has not yet selected a SP for service
                if (customerArray[customer].currentSP == NULL) // the customer has just arrived and has not selected a SP for service
                {
                    real currTime = time();
                    struct ServiceProvider *selectedSP = getLeastBusySP(&customerArray[customer], currTime);
                    setSelectedServiceProvider(&customerArray[customer], selectedSP, currTime);
                }
                if (request(customerArray[customer].currentSP->id,customer,0)==0)
                {
                    //capture the clock time when this customer received the service
                    customerArray[customer].currentSPServiceStartTime = time();
                    for(int i = 0; i < N_SP; i++)
                    {
                        if(customerArray[customer].currentSP->id == customerArray[customer].serviceExperiences[i].sProvider->id)
                        {
                            customerArray[customer].serviceExperiences[i].lastServiceActualWaitTime = (customerArray[customer].currentSPServiceStartTime - customerArray[customer].currentSPQueueStartTime);
                            customerArray[customer].serviceExperiences[i].lastServiceAdvertisedWaitTime = customerArray[customer].currentSPAdvertisedWaitTime;
                            break;
                        }
                    }
                    schedule(3,customerArray[customer].currentServiceTime,customer);
                }
                break;

            case 3: //release server and depart
                release(customerArray[customer].currentSP->id,customer);
                cleanupServiceRequesterBeforeLeaving(&customerArray[customer]);
                schedule(1,expntl(Ta), customer); /* schedule next arrival time */
                //schedule(1, uniform((0.5 * Ta),(1.5 * Ta)), customer);//uniform arrival time
                break;
        }
    }

    myReport(1);

    return 0;
}

void setupServiceProviders()
{
    int i, sr, avlSL;
    for(i = 0; i < N_SP; i++)
    {
        char s[64];
        sprintf(s, "SP%d", i);
        int spid = facility(s, M_SP);
        providerArray[i].id = spid;
        providerArray[i].trustScore = 0.5;
        providerArray[i].totalVisitorCount = 0;
        providerArray[i].nextAvailTime = 0.0;
        providerArray[i].isMalicious = 0;

        //initialize availabilitySlotList
        avlSL = sizeof(providerArray[i].availabilitySlotList)/sizeof(real);
        for(sr = 0; sr < avlSL; sr++)
        {
            providerArray[i].availabilitySlotList[sr] = 0.0;
        }
    }
}

void setupServiceRequesters()
{
    int j, w, spIndx;
    for(j = 0; j < N_SR; j++)
    {
        schedule(1, expntl(Ta), j); //schedule the arrival of the customer
        //printf("Scheduled arrival time of customer %d at %.2f\n", j, ts[j]);
        customerArray[j].id = j;
        customerArray[j].currentSP = NULL;
        customerArray[j].currentServiceTime = 0.0;
        customerArray[j].currentSPServiceStartTime = 0.0;
        customerArray[j].currentSPQueueStartTime = 0.0;
        /*customerArray[j].positiveReviews = 1.0;
        customerArray[j].negativeReviews = 1.0;*/
        customerArray[j].totalReviews = 0;
        customerArray[j].honesty = 0.5;//initially honesty is 50%
        //customerArray[j].credibility = 0.0;
        customerArray[j].isMalicious = 0;

        //initialize service experience
        for(spIndx = 0; spIndx < N_SP; spIndx++)
        {
            customerArray[j].serviceExperiences[spIndx].totalVisits = 0;
            customerArray[j].serviceExperiences[spIndx].lastServiceActualWaitTime = -1.0;
            customerArray[j].serviceExperiences[spIndx].lastServiceAdvertisedWaitTime = -1.0;
            customerArray[j].serviceExperiences[spIndx].sProvider = &providerArray[spIndx];
        }

    }
}

void setSelectedServiceProvider(struct ServiceRequester* serRequester, struct ServiceProvider* selectedSP, real currentTime)
{
    selectedSP->totalVisitorCount++;
    serRequester->currentSP = selectedSP;
    //capture the clock time when this customer was assigned a SP
    serRequester->currentSPQueueStartTime = currentTime;
    serRequester->currentServiceTime = expntl(Ts);
    //Set the service time as a uniformly distributed value rather than exponentially distributed value
    //customerArray[customer].currentServiceTime = uniform(0.75 * Ts, 1.5 * Ts);
    serRequester->currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(selectedSP, currentTime);

    addNewSRToQueueAndUpdateAvailabilitySlot(selectedSP, serRequester);
}

real getSPAdvertisedWaitTime(struct ServiceProvider* SP, real currentTime)
{
    real multiplier;

    if(SP->isMalicious == 1)
    {
        multiplier = (1.0 - Param_R);
    }
    else
    {
        multiplier = 1.0;
    }
    real advertisedWaitTime;
    advertisedWaitTime = multiplier * (SP->nextAvailTime - currentTime);


    if(advertisedWaitTime < 0.0167)
    {
        advertisedWaitTime = 0.0167; //minimum wait time is 1 minute
    }

    return advertisedWaitTime;
}

struct ServiceProvider* getLeastBusySP(struct ServiceRequester* servRequester, real currentTime)
{
    //define an array to store one or more SP with least projected wait time
    int leastBusySPs [N_SP];
    for(int x = 0; x < N_SP; x++)
    {
        leastBusySPs[x] = -1;
    }
    int s, leastBusySPCount;
    real currentSPAdvertisedWaitTime, leastWaitTime;
    leastBusySPCount = 1;
    currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[0], currentTime);
    leastWaitTime =currentSPAdvertisedWaitTime;
    leastBusySPs[0] = 0;
    for(s = 1; s < N_SP; s++)
    {
        currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[s], currentTime);

        if(currentSPAdvertisedWaitTime < leastWaitTime)
        {
            leastWaitTime = currentSPAdvertisedWaitTime;
            //reset the leastBusySPCount array
            for(int x = 0; x < N_SP; x++) {
                leastBusySPs[x] = -1;
            }
            //add the new SP with least wait time to the array
            leastBusySPs[0] = s;
            leastBusySPCount = 1;
        }
        else if(currentSPAdvertisedWaitTime == leastWaitTime){
            //then add the new SP in the leastBusySPs array
            for(int x = 0; x < N_SP; x++){
                if(leastBusySPs[x] == -1){
                    leastBusySPs[x] = s;
                    leastBusySPCount++;
                    break;
                }
            }
        }
    }
    if(leastBusySPCount == 1)
    {
        return &providerArray[leastBusySPs[0]];
    }
    else{
        int spIndxArray[leastBusySPCount];
        for(int y = 0; y < leastBusySPCount; y++){
            spIndxArray[y] =  leastBusySPs[y];
        }
        int randomIndx = rand();
        return &providerArray[spIndxArray[randomIndx % ARR_SIZE(spIndxArray)]];
    }
}

void addNewSRToQueueAndUpdateAvailabilitySlot(struct ServiceProvider* SP, struct ServiceRequester* SR)
{
    //capture current time
    real currentTime = time();

    //next find the index of the slot with the lowest time
    int timeSlotLength = sizeof(SP->availabilitySlotList)/sizeof(real);
    int leastTimeSlotIndex = 0;
    real leastTimeSlot = SP->availabilitySlotList[0];
    int sl;
    for(sl = 1; sl < timeSlotLength; sl++)
    {
        if(SP->availabilitySlotList[sl] < leastTimeSlot)
        {
            leastTimeSlotIndex = sl;
            leastTimeSlot = SP->availabilitySlotList[sl];
        }
    }

    //next update the new time for the selected slot such that the time in the slot will represent when the slot will be available next
    if(SP->availabilitySlotList[leastTimeSlotIndex] == 0.0 || SP->availabilitySlotList[leastTimeSlotIndex] < currentTime)
    {
        SP->availabilitySlotList[leastTimeSlotIndex] = currentTime + SR->currentServiceTime;
    }
    else
    {
        SP->availabilitySlotList[leastTimeSlotIndex] = SP->availabilitySlotList[leastTimeSlotIndex] + SR->currentServiceTime;
    }

    //finally update the waitTime of the SP by finding the least wait time in the availability slot list
    real leastWaitTime;
    leastWaitTime = SP->availabilitySlotList[0];
    for(sl = 1; sl < timeSlotLength; sl++)
    {
        if(SP->availabilitySlotList[sl] < leastWaitTime)
        {
            leastWaitTime = SP->availabilitySlotList[sl];
        }
    }

    if(leastWaitTime == NON_EXISTENT || leastWaitTime < currentTime)
    {
        leastWaitTime = currentTime;
    }

    SP->nextAvailTime = leastWaitTime; //This is the time when the next slot will be available

    for(int svcIndx = 0; svcIndx < N_SP; svcIndx++)
    {
        if(SR->serviceExperiences[svcIndx].sProvider->id == SP->id)
        {
            SR->serviceExperiences[svcIndx].totalVisits++;
            break;
        }
    }
}

void cleanupServiceRequesterBeforeLeaving(struct ServiceRequester* servRequester)
{
    servRequester->currentSP = NULL;
    servRequester->currentServiceTime = 0.0;
    servRequester->currentSPServiceStartTime = 0.0;
    servRequester->currentSPQueueStartTime = 0.0;
}

void printBasicFacilityInfo()
{
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t Avg Queue\t InQ\t Busy Period\t Next Avail Slot\tAdv Wait Time\n");
    real ct = time();
    for(int i = 0; i < N_SP; i++)
    {
        real advertisedWT;
        advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], ct);
        printf("SP%d \t %d\t%d\t\t %d \t %.2f \t %.2f \t %d \t %.2f \t %.2f\t %.2f\n", i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious, U(providerArray[i].id), Lq(providerArray[i].id), inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime, advertisedWT);
        /*for(int as = 0; as < M_SP; as++)
        {
        	printf("\t\tAvail Slot: %d, Value: %.2f\n", as, providerArray[i].availabilitySlotList[as]);
        }*/
    }
}

void figure7Report(real* lastReportedTime, real currentTime)
{
    if((*lastReportedTime <= (currentTime + 5.0))) {
        *lastReportedTime += 5;

        //iterate through each SR and find total SP visits and update totalSPVisitsByGoodSRs and totalMaliciousSPVisitsByGoodSRs
        int latestTotalSPVisits = 0, latestTotalMaliciousSPVisits = 0;

        for(int sr = 0; sr <N_SR; sr++)
        {
            if(customerArray[sr].isMalicious == 1)
            {
                continue;
            }
            for(int sp = 0; sp < N_SP; sp++)
            {
                if(customerArray[sr].serviceExperiences[sp].totalVisits > 0)
                {
                    latestTotalSPVisits += customerArray[sr].serviceExperiences[sp].totalVisits;
                    if(customerArray[sr].serviceExperiences[sp].sProvider->isMalicious == 1)
                    {
                        latestTotalMaliciousSPVisits += customerArray[sr].serviceExperiences[sp].totalVisits;
                    }
                }
            }
        }

        int totalSPVisitsSinceLastCount = latestTotalSPVisits - totalSPVisitsByGoodSRs;
        int totalMaliciousSPVisitsSinceLastCount = latestTotalMaliciousSPVisits - totalMaliciousSPVisitsByGoodSRs;

        real maliciouSPVisitPercentage = 0.0;
        if(totalMaliciousSPVisitsSinceLastCount > 0) {
            maliciouSPVisitPercentage =  (totalMaliciousSPVisitsSinceLastCount * 100.00) / (real)totalSPVisitsSinceLastCount;
        }

        totalSPVisitsByGoodSRs = latestTotalSPVisits;
        totalMaliciousSPVisitsByGoodSRs = latestTotalMaliciousSPVisits;

        printf("%.2f\t%d\t%d\t%.2f\t%d\t%d\n",
               currentTime, totalSPVisitsSinceLastCount, totalMaliciousSPVisitsSinceLastCount,
               maliciouSPVisitPercentage, totalSPVisitsByGoodSRs, totalMaliciousSPVisitsByGoodSRs);
    }
}

void figure8Report(real *report1LastPrinted, real currentTime)
{
    //Display figure 4 report
    if((*report1LastPrinted <= (currentTime + 5.0)))
    {
        *report1LastPrinted += 5;

        printf("SRId\tSPId\tFId\tVstrs\tMal\tInQ\tBusy\tAdvr\tActl\tT Vsts\tCurrT\tLast Visited SR\n");

        int totalVisitors;
        char serviceExp[150];
        totalVisitors = 0;
        for(int i = 0; i < N_SP; i++)
        {
            totalVisitors += providerArray[i].totalVisitorCount;
        }
        int testCustomerId = customer;
        for (int i = 0; i < N_SP; i++) {
            int fbIndx, fbSRCount;
            real trustScore, advertisedWT, actualWT, projectedWaitTime;
            advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], currentTime);
            actualWT = providerArray[i].nextAvailTime - currentTime;
            if (actualWT < 0.016700) {
                actualWT = 0.016700;
            }

            int lastVisitedSP = -1;
            if(customerArray[testCustomerId].currentSP != NULL)
            {
                lastVisitedSP = customerArray[testCustomerId].currentSP->id;
            }
            printf("SR%d\tSP%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\t%d\t%.2f\t%d\n", testCustomerId,
                   i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious,
                   inq(providerArray[i].id), B(providerArray[i].id),
                   advertisedWT, actualWT, totalVisitors, currentTime, lastVisitedSP);
        }
        printf("********************************************\n");
    }
}

void myReport(int isFinalReport)
{
    printf("\nSimulation Report\n");
    printBasicFacilityInfo();
}