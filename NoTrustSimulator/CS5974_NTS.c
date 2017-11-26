#include <math.h>
#include <mem.h>
#include "smpl.h"
#include "cs5974_NTS.h"

#define	currentReportNumber	0

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

int main()
{
    srand(time(NULL));

    real report1LastPrinted;
    report1LastPrinted = -5.0;

    real te = 1015.00; //this is end time for simulation
    smpl(0,"5974"); //initialize smpl simulation
    //trace(2);
    setupServiceProviders();
    setupServiceRequesters();
    setupMaliciousSPSR();

    //initial report setup
    switch(currentReportNumber)
    {
        case 1:
            reportingCustomer = 1;
            printf("#  \t F_ID \t Visitors\t Mal\t In Service\t Avg Queue\t InQ\t Busy Period\t Next Avail Slot\tTrust Score\tAdv Wait Time\tActual Wait Time\tProjected Wait Time\tTotalSystemVisitors\tCurrent Time\n");
            break;
        case 2:

            break;
        case 3:

            break;
        case 4:

            break;
        case 5:

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
            case 1:
                figure1Report(&report1LastPrinted, currentTime);
                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            case 5:

                break;
            default:
                testReport(&report1LastPrinted, currentTime);
                break;
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

void testReport(real *report1LastPrinted, real currentTime)
{
    if(event != 3){
        return;
    }
    if((*report1LastPrinted <= (currentTime + 5.0))) {
        *report1LastPrinted += 5;
        printf("SRId\tSPId\tFId\tVstrs\tMal\tInQ\tBusy\tNxtAvl\tAdvr\tT Vsts\tCurrT\tTstSRVsts\n");

        int totalVisitors;
        totalVisitors = 0;
        for(int i = 0; i < N_SP; i++)
        {
            totalVisitors += providerArray[i].totalVisitorCount;
        }
        for(int testCustomerId = 0; testCustomerId < N_SR; testCustomerId++) {
            if(testCustomerId < 725 || testCustomerId > 727){
                continue;
            }
            for (int i = 0; i < N_SP; i++) {
                int testSRVisits = 0;
                //Service experience history will be displayed under last service rating
                real advertisedWT;
                advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], currentTime);
                testSRVisits =  customerArray[testCustomerId].serviceExperiences[i].totalVisits;
                printf("SR%d\tSP%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\t%d\t%.2f\t%d\n", testCustomerId,
                       i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious,
                       inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime,
                       advertisedWT, totalVisitors, currentTime, testSRVisits);
            }
        }
        printf("********************************************\n");
    }
}

void figure1Report(real* lastReportedTime, real currentTime)
{
    if(*lastReportedTime <= (currentTime + 5.0))
    {
        int totalVisitors;
        totalVisitors = 0;
        *lastReportedTime += 5;
        for(int i = 0; i < N_SP; i++)
        {
            totalVisitors += providerArray[i].totalVisitorCount;
        }
        for(int i = 0; i < N_SP; i++)
        {
            int wtCount;
            wtCount = 0;
            real trustScore, advertisedWT, actualWT, projectedWaitTime;
            advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], currentTime);
            actualWT = providerArray[i].nextAvailTime - currentTime;
            if(actualWT < 0.016700)
            {
                actualWT = 0.016700;
            }
            printf("SP%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%.2f\n",
                   i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious,
                   U(providerArray[i].id), Lq(providerArray[i].id), inq(providerArray[i].id), B(providerArray[i].id),
                   providerArray[i].nextAvailTime, providerArray[i].trustScore, advertisedWT, actualWT,
                   projectedWaitTime, totalVisitors, currentTime);
        }
    }
}

void myReport(int isFinalReport)
{
    printf("\nSimulation Report\n");
    printBasicFacilityInfo();
    //testReport();
}