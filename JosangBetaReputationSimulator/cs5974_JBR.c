#include <math.h>
#include <mem.h>
#include <stdlib.h>
#include "smpl.h"
#include "cs5974_JBR.h"

int	currentReportNumber = 7;
char decisionPolicy[4];
int maliciousPercent = 30;
double Param_R = 1.0;

struct ServiceProvider providerArray[N_SP];
struct ServiceRequester customerArray[N_SR];
int event, customer, reportingCustomer, totalSPVisitsByGoodSRs, totalMaliciousSPVisitsByGoodSRs;

void setupMaliciousSPSR()
{
    if(maliciousPercent == 0)
    {
        return;
    }
    //SP1 is malicious for all cases
    providerArray[1].isMalicious = 1;
    providerArray[2].isMalicious = 0;
    if(maliciousPercent > 10) {
        providerArray[3].isMalicious = 1;
    }
    else{
        providerArray[3].isMalicious = 0;
    }
    providerArray[4].isMalicious = 0;
    if(maliciousPercent > 10) {
        providerArray[5].isMalicious = 1;
    }
    else{
        providerArray[5].isMalicious = 0;
    }
    providerArray[6].isMalicious = 0;
    if(maliciousPercent == 50) {
        providerArray[7].isMalicious = 1;
    }
    else{
        providerArray[7].isMalicious = 0;
    }
    providerArray[8].isMalicious = 0;
    if(maliciousPercent == 50) {
        providerArray[9].isMalicious = 1;
    }
    else{
        providerArray[9].isMalicious = 0;
    }

    for(int indxSR = 0; indxSR < N_SR; indxSR++)
    {
        if(maliciousPercent == 10)
        {
            if(indxSR % 10 == 0)
            {
                customerArray[indxSR].isMalicious = 1;
            }
        }
        else if(maliciousPercent == 30)
        {
            if((indxSR % 5 == 0) || (indxSR % 9 == 0))
            {
                customerArray[indxSR].isMalicious = 1;
            }
        }
        else if(maliciousPercent == 50)
        {
            if(indxSR % 2 == 0)
            {
                customerArray[indxSR].isMalicious = 1;
            }
        }
        //(indxSR % 2 == 0) || (indxSR % 5 == 0)60%//(indxSR % 5 == 0) || (indxSR % 9 == 0)//30% //(indxSR % 5 == 0)//20%//(indxSR % 4 == 0 || indxSR % 5 == 0)//40%
    }
}

//
//First argument is figure/report number to run. Accepted Values: 1 - 8
//Second argument is decision policy to use. Accepted values: LB: Least Busy, MI: Most Trustworthy, LBMT: Least busy among most trustworthy
//Third argument is malicious percentage and is optional. This is 30% by default. Accepted values: 0%, 10%, 30%, 50%
/**
 * Expects 4 arguments.
 * First argument is figure/report number to run. Accepted Values: 7, 8
 * Second argument is malicious percentage and is optional. This is 30% by default. Accepted values: 0%, 10%, 30%, 50%
 * Third argument is total running time as double value. 250hrs by default
 * Fourth argument is
 * @param argc Argument count
 * @param argv Argument Values as array of string/char[]
 * @return
 */
int main(int argc, char *argv[] )
{
    printf("\n\n******************************************************\n\nStarting the simulation for CS5974\nJosang's Beta Reputation \n\n******************************************************\n\n");
    srand(time(NULL));
    real te = 250.00; //this is end time for simulation

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
           currentReportNumber,maliciousPercent, te, Param_R);

    real report1LastPrinted;
    report1LastPrinted = -5.0;
    totalSPVisitsByGoodSRs = 0;
    totalMaliciousSPVisitsByGoodSRs = 0;

    smpl(0,"5974"); //initialize smpl simulation
    //trace(2);
    setupServiceProviders();
    setupServiceRequesters();
    setupMaliciousSPSR();

    //initial report setup
    switch(currentReportNumber)
    {
        case 7:
            printf("Current Time\tSP Visits Since Last Count\tMalicious SP Visits Since Last Count\tMalicious SP Visit Percentage\tTotal SP Visits By Good SRs\tTotal Malicious SP Visits by Good SR\n");
            break;
        case 8:

            break;
        default:
            break;
    }

    while(time() < te)
    {
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
                    struct ServiceProvider *selectedSP =  getLeastBusySPAmongMostTrustworthySP(&customerArray[customer], currentTime);
                    setSelectedServiceProvider(&customerArray[customer], selectedSP, currTime);
                }
                if (request(customerArray[customer].currentSP->id,customer,0)==0)
                {
                    //capture the clock time when this customer received the service
                    customerArray[customer].currentSPServiceStartTime = time();

                    //update the feedback to SP and SR based on the wait time
                    updateServiceExperience(&customerArray[customer]);

                    /*if(customerArray[customer].isMalicious == 0 && customerArray[customer].currentSP->isMalicious == 0)
                    {
                        testReport(&report1LastPrinted, currentTime);
                    }*/

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

        //initialize all witnesses as nulls
        for(w = 0; w < N_SR; w++)
        {
            customerArray[j].witnesses[w].sRequester = NULL;
            customerArray[j].witnesses[w].waitTimeRating = -1.0;
            customerArray[j].witnesses[w].witnessWaitTime = -1.0;
        }
        //initialize service experience
        for(spIndx = 0; spIndx < N_SP; spIndx++)
        {
            customerArray[j].serviceExperiences[spIndx].totalVisits = 0;
            customerArray[j].serviceExperiences[spIndx].lastServiceStartTime = -1.0;
            customerArray[j].serviceExperiences[spIndx].lastServiceRating = -1.0;
            customerArray[j].serviceExperiences[spIndx].lastServiceActualWaitTime = -1.0;
            customerArray[j].serviceExperiences[spIndx].lastServiceAdvertisedWaitTime = -1.0;
            customerArray[j].serviceExperiences[spIndx].sProvider = &providerArray[spIndx];
        }
        //initialize service experience history
        for(spIndx = 0; spIndx < N_SP; spIndx++)
        {
            customerArray[j].experienceHistory[spIndx].historyCount = 0;
        }
        //initialize service provider SRFeedback
        for(spIndx = 0; spIndx < N_SP; spIndx++)
        {
            providerArray[spIndx].feedbacks[j].sRequester = &customerArray[j];
            providerArray[spIndx].feedbacks[j].trustScore = 0.5;
            providerArray[spIndx].feedbacks[j].positiveFeedback = 1.0;
            providerArray[spIndx].feedbacks[j].negativeFeedback = 1.0;
        }

    }

    //now setup SRFeedback foreach SR
    for(j = 0; j < N_SR; j++)
    {
        for(int x = 0; x < N_SR; x++){
            if(x != j){
                customerArray[j].feedbacks[x].negativeFeedback = 1;
                customerArray[j].feedbacks[x].positiveFeedback = 1;
                customerArray[j].feedbacks[x].totalFeedback = 0;
                customerArray[j].feedbacks[x].trustScore = 0.5;
                customerArray[j].feedbacks[x].sRequester = &customerArray[x];
            }
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
    //get all the witnesses so that they can be rated later when the service is received
    //determine all the witnesses and store them to provide feedback later
    int witnessCount;
    //getAllWitnessOrderedByCredibility(selectedSP, serRequester->witnesses, &witnessCount);
    getrecommendersForSR(serRequester, selectedSP, serRequester->witnesses, &witnessCount);
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

struct ServiceProvider *getLeastBusySPAmongMostTrustworthySP(struct ServiceRequester *servRequester, real currentTime)
{
    //define an array to store one or more SP with best trust score
    int bestTrustScoreSPs [N_SP];
    real bestTrustScores [N_SP];//this stores the trust score of each SP by index
    for(int x = 0; x < N_SP; x++)
    {
        bestTrustScoreSPs[x] = -1;
        bestTrustScores[x] = 0.0;
    }
    int s;
    real bestSPTrustScore, currentSPTrustScore;

    bestSPTrustScore = providerArray[0].feedbacks[servRequester->id].trustScore;
    bestTrustScoreSPs[0] = 0;
    bestTrustScores[0] = bestSPTrustScore;
    for(s = 1; s < N_SP; s++)
    {
        /*for(int i = 0; i < N_SR; i++){
            witnessArray[i].sRequester = NULL;
        }*/
        currentSPTrustScore = providerArray[s].feedbacks[servRequester->id].trustScore;
        bestTrustScores[s] = currentSPTrustScore;
        //Now store the index of the current SP in bestTrustScoreSPs array sorted in descending order
        int currentSPIndx = s;
        for(int x = 0; x < N_SP; x++)
        {
            if(bestTrustScoreSPs[x] == -1)
            {
                bestTrustScoreSPs[x] = currentSPIndx;
                break;
            }
            else
            {
                //if the trust score of the SP at index x is less than current SP's trust score
                //then replace the id at index x with that of currentSPIndx and put move the
                //id of the SP at index x below
                if(bestTrustScores[bestTrustScoreSPs[x]] < currentSPTrustScore)
                {
                    int tempSPIndx = bestTrustScoreSPs[x];
                    bestTrustScoreSPs[x] = currentSPIndx;
                    currentSPIndx = tempSPIndx;
                }
            }
        }
    }
    //Now determine N_Rec number of SP with highest trust score.
    //If there are more than 5 then select 5 randomly.
    //However, a more complicated example is:
    //  First 2 SPs sorted in descending order by trust score have higher trust score than next 6 SPs
    //  where all of the next 6 SPs have same trust score. Then only 3 our of 6 SPs need to be selected
    //  randomly to make a list of N_Rec SPs. To do this start from the middle (SP5) of the list and compare from top
    //  to find out where the trust score of SP0-SP4 is same as SP5. If SP4 trust score is higher than SP5 then we can
    //  just take first 5. If SP3 is same as SP5 then we need to find how man SPs below SP5 have same trust score as SP5
    //  Once the list of all SPs below SP3 with same trust score as SP3 is determined then randomly select enough number
    //  of SPs to make N_Rec number of SPs which should include SPs with trust score higher than that of SP3.
    //  From this list of N_Rec SP, sort it based on least wait time. If it there are 2 or more SPs with same projected
    //  wait time then randomly select and SP.

    int selectedSPs[N_Rec];
    for(int x = 0; x < N_Rec; x++)
    {
        selectedSPs[x] = -1;
    }

    //find the trust score of first SP in the second half of the list
    real trustScoreSP5 = bestTrustScores[bestTrustScoreSPs[(N_SP/2)]];
    int selSPIndx;

    for(selSPIndx = 0; selSPIndx < (N_SP/2); selSPIndx++)
    {
        if(bestTrustScores[bestTrustScoreSPs[selSPIndx]] > trustScoreSP5)
        {
            selectedSPs[selSPIndx] = bestTrustScoreSPs[selSPIndx];
        }
        else{
            break;
        }
    }

    //If value of selSPIndx is 5 (N_SP/2) then it means all SPs in selectedSPs have trust score higher than trustScoreSP5
    //and there is no need to find any other SPs.
    //If not then there must be at least one (or at most 5) SPs with same trust score as trustScoreSP5
    if(selSPIndx < (N_SP/2))
    {
        //there can be maximum of N_SP - selSPIndx
        int SPsWithSameTrustScore[N_SP - selSPIndx];
        for(int x = 0; x < (N_SP - selSPIndx); x++)
        {
            SPsWithSameTrustScore[x] = -1;
        }
        int eqlSPIndx;
        for(eqlSPIndx = selSPIndx; eqlSPIndx < N_SP; eqlSPIndx++)
        {
            if(bestTrustScores[bestTrustScoreSPs[eqlSPIndx]] < trustScoreSP5){
                break;
            }
            else{
                SPsWithSameTrustScore[eqlSPIndx - selSPIndx] = bestTrustScoreSPs[eqlSPIndx];
            }
        }

        int countSPsWithSameTrustScore = eqlSPIndx - selSPIndx;

        //total number of SPs remaining to be filled up in selectedSPs is N_rec - selSPIndx
        int countRemainingSPsToBeSelected = N_Rec - selSPIndx;

        if(countSPsWithSameTrustScore < countRemainingSPsToBeSelected)
        {
            printf("*************ERROR*****************\n*\t\t\t\t\t*\n*\tcountSPsWithSameTrustScore < countRemainingSPsToBeSelected\t*\n***********************************");
        }

        //randomly select SPs from SPsWithSameTrustScore and add to selectedSPs
        for(int n = 0; n < countRemainingSPsToBeSelected; n++)
        {
            int selectedIndx = uniform(0.0, (double)(countSPsWithSameTrustScore - n));

            selectedSPs[selSPIndx + n] = SPsWithSameTrustScore[selectedIndx];

            //Now remove the selectedIndx SP from SPsWithSameTrustScore and bring any SP ids under selectedIndx 1 index up
            for(int y = 0; y < (eqlSPIndx - selectedIndx); y++)
            {
                if(selectedIndx < y)
                {
                    SPsWithSameTrustScore[y - 1] = SPsWithSameTrustScore[y];
                }
            }
        }
    }

    //Now select an SPs with least wait time
    int leastWaitTimeSPs[N_Rec]; // there may be up to N_Rec SPs with least wait time
    for(int x = 0; x < N_Rec; x++)
    {
        leastWaitTimeSPs[x] = -1;
    }

    int leastBusySPCount;
    leastBusySPCount = 1;

    real leastAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[selectedSPs[0]], currentTime);
    leastWaitTimeSPs[0] = selectedSPs[0];
    for(s = 1; s < N_Rec; s++)
    {
        real currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[selectedSPs[s]], currentTime);

        if(currentSPAdvertisedWaitTime < leastAdvertisedWaitTime)
        {
            leastAdvertisedWaitTime = currentSPAdvertisedWaitTime;
            //reset the leastBusySPCount array
            for(int x = 0; x < N_Rec; x++) {
                leastWaitTimeSPs[x] = -1;
            }
            //add the new SP with least wait time to the array
            leastWaitTimeSPs[0] = selectedSPs[s];
            leastBusySPCount = 1;
        }
        else if(currentSPAdvertisedWaitTime == leastAdvertisedWaitTime){
            //then add the new SP in the leastBusySPs array
            for(int x = 0; x < N_SP; x++){
                if(leastWaitTimeSPs[x] == -1){
                    leastWaitTimeSPs[x] = selectedSPs[s];
                    leastBusySPCount++;
                    break;
                }
            }
        }
    }
    if(leastBusySPCount == 1)
    {
        return &providerArray[leastWaitTimeSPs[0]];
    }
    else{
        int spIndxArray[leastBusySPCount];
        for(int y = 0; y < leastBusySPCount; y++){
            spIndxArray[y] =  leastWaitTimeSPs[y];
        }
        int randomIndx = rand();
        return &providerArray[spIndxArray[randomIndx % ARR_SIZE(spIndxArray)]];
    }
}

void getrecommendersForSR(struct ServiceRequester *servRequester, struct ServiceProvider *servProvider, struct Witness *witnessArray, int *witnessCount)
{
    //set the array to null first
    for(int wIndx = 0; wIndx < N_SR; wIndx++)
    {
        witnessArray[wIndx].sRequester = NULL;
    }
    *witnessCount = 0;

    //find the SP id first
    int spIndx;
    for(int sIndx = 0; sIndx < N_SP; sIndx++)
    {
        if(servProvider->id == providerArray[sIndx].id){
            spIndx = sIndx;
            break;
        }
    }

    //iterate through each service requester and find out all the ones
    //that have prior experience with the SP
    for(int wIndx = 0; wIndx < N_SR; wIndx++)
    {
        if(wIndx == servRequester->id){//don't use self as witness
            continue;
        }
        if(customerArray[wIndx].serviceExperiences[spIndx].lastServiceRating > -1.0)
        {
            //Return all witness as long as they have prior experience
            witnessArray[*witnessCount].sRequester = &customerArray[wIndx];
            witnessArray[*witnessCount].waitTimeRating = customerArray[wIndx].serviceExperiences[spIndx].lastServiceRating;
            witnessArray[*witnessCount].witnessWaitTime = customerArray[wIndx].serviceExperiences[spIndx].lastServiceAdvertisedWaitTime;
            *witnessCount = *witnessCount + 1;
        }
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
}

//Once an SR is able to successfully scheduled for service with an SP then the SR needs to provide feedback
//towards the SP as well as towards the Witnesses based on its wait time experience. It also needs to update 
//its wait time rating towards the SP so that it can act as witness for others
void updateServiceExperience(struct ServiceRequester* servRequeter)
{
    struct ServiceProvider* SP;
    SP = servRequeter->currentSP;

    //find the service experience to update;
    struct ServiceExperience *svcExp = NULL;
    struct ServiceExperienceHistory *servExpHistory = NULL;
    int currentSPIndx = -1;
    for(currentSPIndx = 0; currentSPIndx < N_SP; currentSPIndx++)
    {
        if(servRequeter->serviceExperiences[currentSPIndx].sProvider->id == SP->id)
        {
            svcExp = &servRequeter->serviceExperiences[currentSPIndx];
            servExpHistory = &servRequeter->experienceHistory[currentSPIndx];
            break;
        }
    }

    if(svcExp == NULL){
        printf("****************ERROR****************\n****************ERROR****************\n****************ERROR****************\n");
        return;
    }

    svcExp->lastServiceStartTime = servRequeter->currentSPServiceStartTime;
    svcExp->totalVisits++;

    //determine the actual wait time by subtracting the clock time when the SR was in queue from when SR received service
    real currentSRActualWaitTime = servRequeter->currentSPServiceStartTime - servRequeter->currentSPQueueStartTime;

    //First determine the actual wait time and set the wait time rating
    if(servRequeter->isMalicious == 0)
    {
        real currentSPAdvertisedWaitTime = servRequeter->currentSPAdvertisedWaitTime;

        //determine the difference between the actual wait time and current advertised wait time
        real waitTimeDiff;

        if(currentSRActualWaitTime <= 0.0167 && currentSPAdvertisedWaitTime == 0.0167)
        {
            //if the SR wait time was less than 1 minute and SP had advertised a wait time of 1 min (0.0167)
            //then se the waitTimeDiff to 0.00
            waitTimeDiff = 0.00;
        }
        else
        {
            waitTimeDiff = fabs(currentSRActualWaitTime - currentSPAdvertisedWaitTime);
        }

        //determine the percentage of difference
        real diffPercentage;
        if(currentSRActualWaitTime == 0.00 || waitTimeDiff == 0.00)
        {
            diffPercentage = 0.00;
        }
        else
        {
            diffPercentage = waitTimeDiff / currentSRActualWaitTime;
        }

        if(diffPercentage < 0.2)
        {
            svcExp->lastServiceRating = 5.0;
        }
        else if (diffPercentage >= 0.20 && diffPercentage < 0.40)
        {
            svcExp->lastServiceRating = 4.0;
        }
        else if(diffPercentage >= 0.40 && diffPercentage < 0.60)
        {
            svcExp->lastServiceRating = 3.0;
        }
        else if(diffPercentage >= 0.60 && diffPercentage < 0.80)
        {
            svcExp->lastServiceRating = 2.0;
        }
        else
        {
            svcExp->lastServiceRating = 1.0;
        }

        //Set the wait time experienced by the SR so that it can be used as witness time
        svcExp->lastServiceActualWaitTime = currentSRActualWaitTime;
        svcExp->lastServiceAdvertisedWaitTime = currentSRActualWaitTime;
    }
    else
    {
        svcExp->lastServiceActualWaitTime = currentSRActualWaitTime;
        if(SP->isMalicious == 1)
        {
            svcExp->lastServiceRating = 5.0;
            svcExp->lastServiceAdvertisedWaitTime = servRequeter->currentSPAdvertisedWaitTime; //A malicious SR will concur with whatever the malicious SP is advertising

        }
        else
        {
            svcExp->lastServiceRating = 1.0;
            svcExp->lastServiceAdvertisedWaitTime = (servRequeter->currentSPAdvertisedWaitTime * (1 + Param_R)); //A malicious SR will increase the wait time of a non-malicious SP by factor R
        }
    }

    //update service experience history
    if(servExpHistory->historyCount == 5){
        //move items 1-4 one index up so that the last index is free
        for(int seIndx = 1; seIndx < 5;  seIndx++){
            servExpHistory->servExp[seIndx - 1] = servExpHistory->servExp[seIndx];
        }
        servExpHistory->historyCount--;//decrement the history count temporarily so the new history record is inserted in the right index. This will be incremented after the new history is update.
    }

    servExpHistory->servExp[servExpHistory->historyCount].lastServiceRating = svcExp->lastServiceRating;
    servExpHistory->servExp[servExpHistory->historyCount].totalVisits = svcExp->totalVisits;
    servExpHistory->servExp[servExpHistory->historyCount].sProvider = svcExp->sProvider;
    servExpHistory->servExp[servExpHistory->historyCount].lastServiceAdvertisedWaitTime = servRequeter->currentSPAdvertisedWaitTime;
    servExpHistory->servExp[servExpHistory->historyCount].lastServiceActualWaitTime = svcExp->lastServiceActualWaitTime;
    servExpHistory->servExp[servExpHistory->historyCount].lastServiceStartTime = svcExp->lastServiceStartTime;

    servExpHistory->historyCount++;


    //determine the normalized rating towards the SP for this transaction
    real f, newAlpha, newBeta;
    f = (svcExp->lastServiceRating) / 5.0;

    SP->feedbacks[servRequeter->id].positiveFeedback = SP->feedbacks[servRequeter->id].positiveFeedback + f;
    SP->feedbacks[servRequeter->id].negativeFeedback = SP->feedbacks[servRequeter->id].negativeFeedback + 1.0 - f;
    SP->feedbacks[servRequeter->id].totalFeedback++;

    //calculate the trust score towards the SP from this SR
    SP->feedbacks[servRequeter->id].trustScore = SP->feedbacks[servRequeter->id].positiveFeedback / (SP->feedbacks[servRequeter->id].positiveFeedback + SP->feedbacks[servRequeter->id].negativeFeedback);

    //now update feedback for all other SRs (witnesses) that had service experience from the SP in past
    for(int fdIndx = 0; fdIndx < N_SR; fdIndx++)
    {
        //do NOT give feedback to ownself
        if(customerArray[fdIndx].serviceExperiences[currentSPIndx].lastServiceRating != -1 && fdIndx != servRequeter->id)
        {
            SP->feedbacks[fdIndx].positiveFeedback = SP->feedbacks[fdIndx].positiveFeedback + f;
            SP->feedbacks[fdIndx].negativeFeedback = SP->feedbacks[fdIndx].negativeFeedback + 1.0 - f;
            SP->feedbacks[fdIndx].totalFeedback++;
        }
    }
}

void cleanupServiceRequesterBeforeLeaving(struct ServiceRequester* servRequester)
{
    servRequester->currentSP = NULL;
    servRequester->currentServiceTime = 0.0;
    servRequester->currentSPServiceStartTime = 0.0;
    servRequester->currentSPQueueStartTime = 0.0;
    //initialize all witnesses as nulls
    for(int w = 0; w < N_SR; w++)
    {
        servRequester->witnesses[w].sRequester = NULL;
        servRequester->witnesses[w].waitTimeRating = -1.0;
        servRequester->witnesses[w].witnessWaitTime = -1.0;
    }
}

void printBasicFacilityInfo()
{
    printf("#\tF_ID\tVisitors\tMal\tIn Service\tAvg Queue\tInQ\tBusy Period\n");
    real ct = time();
    for(int i = 0; i < N_SP; i++)
    {
        int fbIndx, fbSRCount;
        real posFB, negFB, trustScore, advertisedWT, actualWT, projectedWaitTime;

        printf("SP%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\t%.2f\n", i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious, U(providerArray[i].id), Lq(providerArray[i].id), inq(providerArray[i].id), B(providerArray[i].id));
    }
}

void printBasicCustomerInfo()
{
    printf("\n\n");
    int cIndx,  malSRCount, nonMalSRCount;
    real totalMalHonesty, totalNonMalHonesty, totalMaliciousCredibility, totalNonMaliciousCredibility, positiveFB, negativeFB;
    malSRCount = 0;
    nonMalSRCount = 0;
    totalMalHonesty = 0.0;
    totalNonMalHonesty = 0.0;
    totalMaliciousCredibility = 0.0;
    totalNonMaliciousCredibility = 0.0;
    printf("CId \t + Feedback\t- Feedback\t Centrality\t Honesty\t\t Mal\n");
    for(cIndx = 0; cIndx < N_SR; cIndx++)
    {
        real centrality = customerArray[cIndx].totalReviews;
        positiveFB = 0;
        negativeFB = 0;
        for(int x = 0; x < N_SR; x++){
            if(x != cIndx){
                if(customerArray[cIndx].feedbacks[x].totalFeedback > 0){
                    positiveFB += customerArray[cIndx].feedbacks[x].positiveFeedback;
                    negativeFB+= customerArray[cIndx].feedbacks[x].negativeFeedback;
                }
            }
        }

        if(customerArray[cIndx].isMalicious == 1)
        {
            malSRCount++;
            totalMalHonesty += customerArray[cIndx].honesty;
            //totalMaliciousCredibility += customerArray[cIndx].credibility;
        }
        else
        {
            nonMalSRCount++;
            totalNonMalHonesty += customerArray[cIndx].honesty;
            //totalNonMaliciousCredibility += customerArray[cIndx].credibility;
        }

        printf("%d\t %.2f\t %.2f\t %.2f\t\t %.2f\t\t %d\n", cIndx, positiveFB, negativeFB, centrality, customerArray[cIndx].honesty, customerArray[cIndx].isMalicious);

    }
    printf("Average Malicious SR Honesty: %.2f\nAverage Non-Malicious SR Honesty: %.2f\n", (totalMalHonesty/malSRCount), (totalNonMalHonesty/nonMalSRCount));
    printf("Average Malicious SR Credibility: %.2f\nAverage Non-Malicious SR Credibility: %.2f\n", (totalMaliciousCredibility/malSRCount), (totalNonMaliciousCredibility/nonMalSRCount));
}

void myReport(int isFinalReport)
{
    printf("\n\n\nFinal Simulation Report\n\n\n");
    printBasicFacilityInfo();
    printBasicCustomerInfo();
}

void figure7Report(real *report1LastPrinted, real currentTime)
{
    if((*report1LastPrinted <= (currentTime + 5.0))) {
        *report1LastPrinted += 5;

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
            maliciouSPVisitPercentage =  (totalMaliciousSPVisitsSinceLastCount * 100.0 / (real) totalSPVisitsSinceLastCount);
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

        printf("SRId\tSPId\tFId\tVstrs\tMal\tInQ\tBusy\tNxtAvl\tTrst\tAdvr\tActl\tT Vsts\tCurrT\tTstSRVsts\tLast Visited SR\tLstRtng|ActWT|AdvWT\tWtnsCount\n");

        int totalVisitors, witnessCount;
        char serviceExp[150];
        totalVisitors = 0;
        for(int i = 0; i < N_SP; i++)
        {
            totalVisitors += providerArray[i].totalVisitorCount;
        }
        //for(int x = 0; x < 3; x++) {
        int testCustomerId = customer;
        for (int i = 0; i < N_SP; i++) {
            //Service experience history will be displayed under last service rating
            strcpy(serviceExp, "");
            for (int seIndx = 0;
                 seIndx < customerArray[testCustomerId].experienceHistory[i].historyCount; seIndx++) {
                char str[19];
                sprintf(str, "%.1f|%.3f|%.3f || ",
                        customerArray[testCustomerId].experienceHistory[i].servExp[seIndx].lastServiceRating,
                        customerArray[testCustomerId].experienceHistory[i].servExp[seIndx].lastServiceActualWaitTime,
                        customerArray[testCustomerId].experienceHistory[i].servExp[seIndx].lastServiceAdvertisedWaitTime);
                strcat(serviceExp, str);
            }
            int fbIndx, fbSRCount;
            real trustScore, advertisedWT, actualWT;
            advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], currentTime);
            actualWT = providerArray[i].nextAvailTime - currentTime;
            if (actualWT < 0.016700) {
                actualWT = 0.016700;
            }

            witnessCount = 0;
            struct Witness witnessArray[N_SR];
            getrecommendersForSR(&customerArray[testCustomerId], &providerArray[i], witnessArray, &witnessCount);
            trustScore = providerArray[i].feedbacks[testCustomerId].trustScore;
            int lastVisitedSP = -1;
            if(customerArray[testCustomerId].currentSP != NULL)
            {
                lastVisitedSP = customerArray[testCustomerId].currentSP->id;
            }
            printf("SR%d\tSP%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%.2f\t%d\t%d\t%s\t%d\n", testCustomerId,
                   i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious,
                   inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime, trustScore,
                   advertisedWT, actualWT, totalVisitors, currentTime,
                   providerArray[i].feedbacks[testCustomerId].totalFeedback, lastVisitedSP, serviceExp,
                   witnessCount);
            /*for(int as = 0; as < M_SP; as++)
            {
                printf("\t\tAvail Slot: %d, Value: %.2f\n", as, providerArray[i].availabilitySlotList[as]);
            }*/
        }
        //}
        printf("********************************************\n");
    }
}