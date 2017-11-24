#include <math.h>
#include <mem.h>
#include "smpl.h"
#include "cs5974.h"

#define	currentReportNumber	0

struct ServiceProvider providerArray[N_SP];
struct ServiceRequester customerArray[N_SR];
int event, customer, reportingCustomer;

int quickTest(){
    int result = 0;
    if(time() > 40.00){
        int totalVisitingSRs = 0;
        for(int indx = 0; indx < N_SR; indx++){
            if(providerArray[3].feedbacks[indx].totalFeedback > 0){
                totalVisitingSRs++;
                printf("customer: %d, Malicious: %d, Trust Score: %f, total +ve FB: %f, total NegativeFB: %f, Last Service Rating: %f, Exp T Visits: %d, total Visits: %d\n",
                       indx, customerArray[indx].isMalicious, providerArray[3].feedbacks[indx].trustScore,
                       providerArray[3].feedbacks[indx].positiveFeedback, providerArray[3].feedbacks[indx].negativeFeedback,
                       customerArray[indx].serviceExperiences[3].lastServiceRating,
                       customerArray[indx].serviceExperiences[3].totalVisits, providerArray[3].feedbacks[indx].totalFeedback);
            }
        }
        printf("Total visiting SRs: %d\n", totalVisitingSRs);
        result = -1;
    }
    return result;
}

void setupMaliciousSPSR()
{
    int indxSP, indxSR;

    for(indxSP = 0; indxSP < N_SP; indxSP++)
    {
        if(indxSP == 1 || indxSP == 3 || indxSP == 4 || indxSP == 5 || indxSP == 7 || indxSP == 9)
        {
            providerArray[indxSP].isMalicious = 1;
        }
    }

    for(indxSR = 0; indxSR < N_SR; indxSR++)
    {
        if((indxSR % 2 == 0) || (indxSR % 5 == 0))//(indxSR % 2 == 0) || (indxSR % 5 == 0)60%//(indxSR % 5 == 0) || (indxSR % 9 == 0)//30% //(indxSR % 5 == 0)//20%//(indxSR % 4 == 0 || indxSR % 5 == 0)//40%
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
                    //struct ServiceProvider *selectedSP = getMostTrustworthySP(&customerArray[customer]);
                    //struct ServiceProvider *selectedSP = getLeastBusySPAmongMostTrustworthySP(&customerArray[customer], currentTime);
                    setSelectedServiceProvider(&customerArray[customer], selectedSP, currTime);
                }
                if (request(customerArray[customer].currentSP->id,customer,0)==0)
                {
                    //capture the clock time when this customer received the service
                    customerArray[customer].currentSPServiceStartTime = time();

                    //update the feedback to SP and SR based on the wait time
                    updateServiceExperience(&customerArray[customer]);

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

real getProjectedWaitTime(struct ServiceProvider* SP, struct ServiceRequester* servRequester, real advertisedWaitTime)
{
    int witnessCount, srHasPriorExperienceWithSP;
    witnessCount = 0;
    srHasPriorExperienceWithSP = 0;
    struct Witness witnessArray[N_SR];

    for(int s =0; s < N_SP; s++){
        if(servRequester->serviceExperiences[s].sProvider->id == SP->id){
            if(servRequester->serviceExperiences[s].lastServiceRating > -1.0){
                srHasPriorExperienceWithSP = 1;
            }
            break;
        }
    }
    getrecommendersForSR(servRequester, SP, witnessArray, &witnessCount);
    if(witnessCount > N_SR){
        printf("***********************ERROR****************************\n***Witness count greater than N_SR***\n*****************************************\n");
    }

    for(int v = 0; v < N_Rec; v++){
        if(witnessArray[v].sRequester != NULL && servRequester->feedbacks[witnessArray[v].sRequester->id].trustScore < H_Thres){
            witnessArray[v].sRequester = NULL;
        }
    }

    //after determining all witnesses, use the wait time of the top N_Rec witnesses to project the SP wait time
    real netCredibility, projWaitTimeByWitnesses, projectedWaitTime;
    netCredibility = 0.0;
    projWaitTimeByWitnesses = -1.0;
    if(srHasPriorExperienceWithSP == 1 || (witnessCount > 0 && witnessArray[0].sRequester != NULL)) {
        projWaitTimeByWitnesses = 0.0;
        // 1. Calculate sum of credibility of top N_Rec witnesses
        int n_rec_UpperLimit;
        n_rec_UpperLimit = N_Rec;
        if (srHasPriorExperienceWithSP == 1) {
            netCredibility += 1.0;//the inquiring SRs credibility is 1
            n_rec_UpperLimit = (N_Rec - 1);
        }
        for (int rIndx = 0; rIndx < n_rec_UpperLimit && rIndx < witnessCount; rIndx++) {
            if (witnessArray[rIndx].sRequester != NULL && servRequester->feedbacks[witnessArray[rIndx].sRequester->id].trustScore >= H_Thres) {
                netCredibility += servRequester->feedbacks[witnessArray[rIndx].sRequester->id].trustScore;
            }
        }
        //2. Calculate Equation 7: Projected wait time based on witnesses
        if (srHasPriorExperienceWithSP == 1) {
            for(int s =0; s < N_SP; s++) {
                if (servRequester->serviceExperiences[s].sProvider->id == SP->id) {
                    projWaitTimeByWitnesses += ((1.0 / netCredibility) *
                                                servRequester->serviceExperiences[s].lastServiceActualWaitTime);
                    break;
                }
            }
        }
        if (netCredibility > 0.0) {
            for (int rIndx = 0; rIndx < n_rec_UpperLimit && rIndx < witnessCount; rIndx++) {
                if (witnessArray[rIndx].sRequester != NULL && servRequester->feedbacks[witnessArray[rIndx].sRequester->id].trustScore >= H_Thres) {
                    projWaitTimeByWitnesses += (servRequester->feedbacks[witnessArray[rIndx].sRequester->id].trustScore / netCredibility) * witnessArray[rIndx].witnessWaitTime;
                }
            }
        }
        /*
        //3. Equation 9: Determine weight for witness projected wait time (this is normalized)
            if(servRequester != NULL){
                rho += (servRequester->credibility / netCredibility) * servRequester->honesty;
            }
        if (netCredibility > 0.0) {
            for (int rIndx = 0; rIndx < n_rec_UpperLimit && rIndx < witnessCount; rIndx++) {
                if (witnessArray[rIndx].sRequester != NULL) {
                    rho += (witnessArray[rIndx].sRequester->credibility / netCredibility) *
                           witnessArray[rIndx].sRequester->honesty;
                }
            }
        }

        //Rho can be as high as 0.5 so multiply it by 0.5
        rho = 0.5 * rho;*/
    }

    /*real spCalcTrustScore = SP->trustScore;
    if(servRequester != NULL){
        spCalcTrustScore = calculateSPTrustScoreBySR(SP, servRequester, witnessArray, witnessCount);
    }

    //4. Calculate Equation 8 to determine the projected wait time based on witnesses and advertised wait time
    projectedWaitTime = (((1 - rho) * (advertisedWaitTime/spCalcTrustScore)) + (rho * projWaitTimeByWitnesses));
    */
    if(projWaitTimeByWitnesses > -1.0){
        projectedWaitTime = projWaitTimeByWitnesses;
    }
    else{
        projectedWaitTime = advertisedWaitTime;
    }
    /*else {
        if(servRequester == NULL) {
            projectedWaitTime = (advertisedWaitTime / SP->trustScore);
        }
        else{
            projectedWaitTime = (advertisedWaitTime / SP->feedbacks[servRequester->id].trustScore);
        }
    }*/
    return projectedWaitTime;
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
    real currentSPAdvertisedWaitTime;
    leastBusySPCount = 1;
    //struct ServiceProvider* selectedSP = &providerArray[0];
    currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[0], currentTime);
    real leastProjectedWaitTime = getProjectedWaitTime(&providerArray[0], servRequester, currentSPAdvertisedWaitTime);
    leastBusySPs[0] = 0;
    for(s = 1; s < N_SP; s++)
    {
        currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[s], currentTime);
        real currentSPLeastProjectedWaitTime = getProjectedWaitTime(&providerArray[s], servRequester, currentSPAdvertisedWaitTime);

        if(currentSPLeastProjectedWaitTime < leastProjectedWaitTime)
        {
            //selectedSP = &providerArray[s];
            leastProjectedWaitTime = currentSPLeastProjectedWaitTime;
            //reset the leastBusySPCount array
            for(int x = 0; x < N_SP; x++) {
                leastBusySPs[x] = -1;
            }
            //add the new SP with least wait time to the array
            leastBusySPs[0] = s;
            leastBusySPCount = 1;
        }
        else if(currentSPLeastProjectedWaitTime == leastProjectedWaitTime){
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

struct ServiceProvider *getMostTrustworthySP(struct ServiceRequester *servRequester)
{
    //define an array to store one or more SP with best trust score
    int bestTrustScoreSPs [N_SP];
    for(int x = 0; x < N_SP; x++)
    {
        bestTrustScoreSPs[x] = -1;
    }
    int s, bestTrustScoreSPCount, wtCount;
    real bestSPTrustScore, currentSPTrustScore;
    bestTrustScoreSPCount = 1;

    struct Witness witnessArray[N_SR];
    for(int i = 0; i < N_SR; i++){
        witnessArray[i].sRequester = NULL;
    }
    getrecommendersForSR(servRequester, &providerArray[0], witnessArray, &wtCount);

    bestSPTrustScore = calculateSPTrustScoreBySR(&providerArray[0], servRequester, witnessArray, wtCount);
    bestTrustScoreSPs[0] = 0;
    for(s = 1; s < N_SP; s++)
    {
        for(int i = 0; i < N_SR; i++){
            witnessArray[i].sRequester = NULL;
        }
        getrecommendersForSR(servRequester, &providerArray[s], witnessArray, &wtCount);
        currentSPTrustScore = calculateSPTrustScoreBySR(&providerArray[s], servRequester, witnessArray, wtCount);

        if(currentSPTrustScore > bestSPTrustScore)
        {
            bestSPTrustScore = currentSPTrustScore;
            //reset the leastBusySPCount array
            for(int x = 0; x < N_SP; x++) {
                bestTrustScoreSPs[x] = -1;
            }
            //add the new SP with least wait time to the array
            bestTrustScoreSPs[0] = s;
            bestTrustScoreSPCount = 1;
        }
        else if(currentSPTrustScore == bestSPTrustScore){
            //then add the new SP in the bestTrustScoreSPs array
            for(int x = 0; x < N_SP; x++){
                if(bestTrustScoreSPs[x] == -1){
                    bestTrustScoreSPs[x] = s;
                    bestTrustScoreSPCount++;
                    break;
                }
            }
        }
    }
    if(bestTrustScoreSPCount == 1)
    {
        return &providerArray[bestTrustScoreSPs[0]];
    }
    else{
        int spIndxArray[bestTrustScoreSPCount];
        for(int y = 0; y < bestTrustScoreSPCount; y++){
            spIndxArray[y] =  bestTrustScoreSPs[y];
        }
        int randomIndx = rand();
        return &providerArray[spIndxArray[randomIndx % ARR_SIZE(spIndxArray)]];
    }
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
    int s, bestTrustScoreSPCount, wtCount;
    real bestSPTrustScore, currentSPTrustScore;

    struct Witness witnessArray[N_SR];
    for(int i = 0; i < N_SR; i++){
        witnessArray[i].sRequester = NULL;
    }
    getrecommendersForSR(servRequester, &providerArray[0], witnessArray, &wtCount);

    bestSPTrustScore = calculateSPTrustScoreBySR(&providerArray[0], servRequester, witnessArray, wtCount);
    bestTrustScoreSPs[0] = 0;
    bestTrustScores[0] = bestSPTrustScore;
    for(s = 1; s < N_SP; s++)
    {
        for(int i = 0; i < N_SR; i++){
            witnessArray[i].sRequester = NULL;
        }
        getrecommendersForSR(servRequester, &providerArray[s], witnessArray, &wtCount);
        currentSPTrustScore = calculateSPTrustScoreBySR(&providerArray[s], servRequester, witnessArray, wtCount);
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

    /*
    if(servRequester->id == 726)
    {
        printf("\n******************* Customer 726 *****************\n");
        for(int w = 0; w < N_Rec; w++)
        {
            printf("Selected SP%d: %d\n",w, selectedSPs[w]);
        }
        printf("\n**************************************************\n");
    }
    */
    //Now select an SPs with least wait time
    int leastWaitTimeSPs[N_Rec]; // there may be up to N_Rec SPs with least wait time
    for(int x = 0; x < N_Rec; x++)
    {
        leastWaitTimeSPs[x] = -1;
    }

    int leastBusySPCount;
    real currentSPAdvertisedWaitTime;
    leastBusySPCount = 1;

    currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[selectedSPs[0]], currentTime);
    real leastProjectedWaitTime = getProjectedWaitTime(&providerArray[selectedSPs[0]], servRequester, currentSPAdvertisedWaitTime);
    leastWaitTimeSPs[0] = selectedSPs[0];
    for(s = 1; s < N_Rec; s++)
    {
        currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(&providerArray[selectedSPs[s]], currentTime);
        real currentSPLeastProjectedWaitTime = getProjectedWaitTime(&providerArray[selectedSPs[s]], servRequester, currentSPAdvertisedWaitTime);

        if(currentSPLeastProjectedWaitTime < leastProjectedWaitTime)
        {
            leastProjectedWaitTime = currentSPLeastProjectedWaitTime;
            //reset the leastBusySPCount array
            for(int x = 0; x < N_Rec; x++) {
                leastWaitTimeSPs[x] = -1;
            }
            //add the new SP with least wait time to the array
            leastWaitTimeSPs[0] = selectedSPs[s];
            leastBusySPCount = 1;
        }
        else if(currentSPLeastProjectedWaitTime == leastProjectedWaitTime){
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

real calculateSPTrustScoreBySR(struct ServiceProvider *SP, struct ServiceRequester *servRequester, struct Witness *witnessArray, int witnessCount){
    real calculatedTrustScore = SP->feedbacks[servRequester->id].trustScore;
    int witnessUpperLimit = N_Rec;
    int srHasExperience = 0;

    if(servRequester != NULL && SP->feedbacks[servRequester->id].totalFeedback > 0){
        witnessUpperLimit = N_Rec - 1;
        srHasExperience = 1;
    }

    real newTrustScore, netSRCredibility;
    netSRCredibility = 0.0;
    newTrustScore = 0.0; //the new trust score of the SP calculated based on the latest updates

    //determine the total credibility of witnessUpperLimit witnesses
    if(srHasExperience == 1){
        netSRCredibility += 1;//credibility of current SR is 1
    }
    for(int wIndx = 0; wIndx < witnessUpperLimit && wIndx < witnessCount; wIndx++)
    {
        if(witnessArray[wIndx].sRequester != NULL && witnessArray[wIndx].sRequester->id >= 0 && servRequester->feedbacks[witnessArray[wIndx].sRequester->id].trustScore >= H_Thres)
        {
            netSRCredibility += servRequester->feedbacks[witnessArray[wIndx].sRequester->id].trustScore;
        }
    }
    if(srHasExperience){
        newTrustScore = (1/netSRCredibility) * SP->feedbacks[servRequester->id].trustScore;
    }
    for(int wIndx = 0; wIndx < witnessUpperLimit && wIndx < witnessCount; wIndx++)
    {
        if(witnessArray[wIndx].sRequester != NULL && witnessArray[wIndx].sRequester->id >= 0 && servRequester->feedbacks[witnessArray[wIndx].sRequester->id].trustScore >= H_Thres)
        {
            newTrustScore = newTrustScore + ((servRequester->feedbacks[witnessArray[wIndx].sRequester->id].trustScore / netSRCredibility) * SP->feedbacks[witnessArray[wIndx].sRequester->id].trustScore);
        }
    }

    if(newTrustScore > 0.0){
        calculatedTrustScore = newTrustScore;
    }

    return calculatedTrustScore;
}

/*void getAllWitnessOrderedByCredibility(struct ServiceProvider* servProvider, struct Witness* witnessArray, int *witnessCount)
{
    *witnessCount = 0;

    //set the array to null first
    for(int wIndx = 0; wIndx < N_SR; wIndx++)
    {
        witnessArray[wIndx].sRequester = NULL;
    }

    //iterate through each service requester and find out all the ones
    //that have prior experience with the SP
    for(int wIndx = 0; wIndx < N_SR; wIndx++)
    {
        //iterate the service experience of each SR to check if it has prior experience
        for(int sIndx = 0; sIndx < N_SP; sIndx++)
        {
            if(customerArray[wIndx].serviceExperiences[sIndx].sProvider->id == servProvider->id)
            {
                struct ServiceRequester* currentSR = &customerArray[wIndx];
                if(currentSR->serviceExperiences[sIndx].lastServiceRating > -1.0)
                {
                    //if the customer has experience with the SP then increment the counter
                    *witnessCount++;
                    for(int w = 0; w < N_SR; w++)
                    {
                        if(witnessArray[w].sRequester == NULL)
                        {
                            witnessArray[w].sRequester = currentSR;
                            witnessArray[w].waitTimeRating = currentSR->serviceExperiences[sIndx].lastServiceRating;
                            witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[sIndx].lastServiceAdvertisedWaitTime;
                            break;
                        }
                        else if(witnessArray[w].sRequester->credibility < currentSR->credibility)
                        {
                            struct ServiceRequester* tempSR = witnessArray[w].sRequester;
                            witnessArray[w].sRequester = currentSR;
                            witnessArray[w].waitTimeRating = currentSR->serviceExperiences[sIndx].lastServiceRating;
                            witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[sIndx].lastServiceAdvertisedWaitTime;
                            currentSR = tempSR;
                        }
                    }
                }
                break;
            }
        }
    }
}
*/
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
            //if(customerArray[wIndx].feedbacks[servRequester->id].trustScore > H_Thres)
            //{
                *witnessCount = *witnessCount + 1;
                if(*witnessCount > N_SR){
                    printf("************ERROR************\nWitness count greater than N_SR\n");
                }
                struct ServiceRequester* currentSR = &customerArray[wIndx];
                for(int w = 0; w < N_SR; w++)
                {
                    if (witnessArray[w].sRequester == NULL) {
                        witnessArray[w].sRequester = currentSR;
                        witnessArray[w].waitTimeRating = currentSR->serviceExperiences[spIndx].lastServiceRating;
                        witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[spIndx].lastServiceAdvertisedWaitTime;
                        break;
                    } else if (servRequester->feedbacks[currentSR->id].trustScore > servRequester->feedbacks[witnessArray[w].sRequester->id].trustScore) {
                        struct ServiceRequester *tempSR = witnessArray[w].sRequester;
                        witnessArray[w].sRequester = currentSR;
                        witnessArray[w].waitTimeRating = currentSR->serviceExperiences[spIndx].lastServiceRating;
                        witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[spIndx].lastServiceAdvertisedWaitTime;
                        currentSR = tempSR;
                    }
                    /*else if (servRequester->feedbacks[currentSR->id].trustScore == servRequester->feedbacks[witnessArray[w].sRequester->id].trustScore
                            && customerArray[currentSR->id].serviceExperiences[spIndx].lastServiceStartTime > customerArray[witnessArray[w].sRequester->id].serviceExperiences[spIndx].lastServiceStartTime) {
                        struct ServiceRequester *tempSR = witnessArray[w].sRequester;
                        witnessArray[w].sRequester = currentSR;
                        witnessArray[w].waitTimeRating = currentSR->serviceExperiences[spIndx].lastServiceRating;
                        witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[spIndx].lastServiceAdvertisedWaitTime;
                        currentSR = tempSR;
                    }*/
                }
            //}
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
    for(int svcIndx = 0; svcIndx < N_SP; svcIndx++)
    {
        if(servRequeter->serviceExperiences[svcIndx].sProvider->id == SP->id)
        {
            svcExp = &servRequeter->serviceExperiences[svcIndx];
            servExpHistory = &servRequeter->experienceHistory[svcIndx];
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

    //now provide feedback for each witness. Witness is an SR that had service experience from the SP in past
    for(int fdIndx = 0; fdIndx < N_SR; fdIndx++)
    {
        /*if(servRequeter->witnesses[fdIndx].sRequester == NULL){
            break;
        }*/
        //do NOT give feedback to ownself
        if(servRequeter->witnesses[fdIndx].sRequester != NULL && servRequeter->witnesses[fdIndx].sRequester->id != servRequeter->id)
        {
            real witnessRatingNew;
            real normRating;

            if(servRequeter->isMalicious == 1)
            {
                if(servRequeter->witnesses[fdIndx].sRequester->isMalicious == 1)
                {
                    witnessRatingNew = 5.0;
                }
                else
                {
                    witnessRatingNew = 1.0;
                }
            }
            else
            {
                //determine the difference between the wait time rating experienced by current SR and the witness
                real waitTimeRatingDiffWithWitness = fabs(svcExp->lastServiceRating - servRequeter->witnesses[fdIndx].waitTimeRating);

                //determine the new rating for the witness
                witnessRatingNew = 5.0 - waitTimeRatingDiffWithWitness;
            }

            normRating = (witnessRatingNew / 5.0);

            servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].positiveFeedback += normRating;
            servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].negativeFeedback += (1 - normRating);
            servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].totalFeedback++;
            //calculate the honesty based on servRequester for the witness
            servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].trustScore = servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].positiveFeedback / (servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].positiveFeedback + servRequeter->witnesses[fdIndx].sRequester->feedbacks[servRequeter->id].negativeFeedback);

            servRequeter->witnesses[fdIndx].sRequester->totalReviews++;
            /*
            if(*highestCentrality == 0.00 || (*highestCentrality < servRequeter->witnesses[fdIndx].sRequester->totalReviews))
            {
                *highestCentrality = servRequeter->witnesses[fdIndx].sRequester->totalReviews;
            }
             */
            //now calculate the overall honesty of the SR
            real positiveFB, negativeFB;
            positiveFB = 0;
            negativeFB = 0;
            for(int x = 0; x < N_SR; x++){
                if(servRequeter->witnesses[fdIndx].sRequester->id != x){//don't use ownself to calculate honesty
                    if(servRequeter->witnesses[fdIndx].sRequester->feedbacks[x].totalFeedback > 0){
                        positiveFB += servRequeter->witnesses[fdIndx].sRequester->feedbacks[x].positiveFeedback;
                        negativeFB += servRequeter->witnesses[fdIndx].sRequester->feedbacks[x].negativeFeedback;
                    }
                }
            }
            servRequeter->witnesses[fdIndx].sRequester->honesty = positiveFB / (positiveFB + negativeFB);
            //servRequeter->witnesses[fdIndx].sRequester->credibility = servRequeter->witnesses[fdIndx].sRequester->honesty * (servRequeter->witnesses[fdIndx].sRequester->totalReviews / *highestCentrality);
        }
    }

    /*
    //Update the trust score of the SP
    //determine top N_Rec SR to calculate the trust score of SP
    struct SRFeedback *highestRecommenders[N_Rec];
    int rIndx, srIndx;
    for(rIndx = 0; rIndx < N_Rec; rIndx++)
    {
        highestRecommenders[rIndx] = NULL;
    }

    for(srIndx = 0; srIndx < N_SR; srIndx++)
    {
        if(SP->feedbacks[srIndx].trustScore > 0.0)
        {
            struct SRFeedback *currentRec = &SP->feedbacks[srIndx];
            for(rIndx = 0; rIndx < N_Rec; rIndx++)
            {
                if(highestRecommenders[rIndx] == NULL)
                {
                    highestRecommenders[rIndx] = currentRec;
                    break;
                }
                else
                {
                    if(highestRecommenders[rIndx]->sRequester->credibility < currentRec->sRequester->credibility)
                    {
                        struct SRFeedback *tempSRF = highestRecommenders[rIndx];
                        highestRecommenders[rIndx] = currentRec;
                        currentRec = tempSRF;
                    }
                }
            }

        }
    }

    //update the trust score of the SP based on the new trust score from the SR
    //int srIndx;
    real newTrustScore, netSRCredibility;
    netSRCredibility = 0.0;
    newTrustScore = 0.0; //the new trust score of the SP calculated based on the latest updates

    //determine the total credibility of of N_Rec
    for(rIndx = 0; rIndx < N_Rec; rIndx++)
    {
        if(highestRecommenders[rIndx] != NULL)
        {
            netSRCredibility += highestRecommenders[rIndx]->sRequester->credibility;
        }
    }
    for(rIndx = 0; rIndx < N_Rec; rIndx++)
    {
        if(highestRecommenders[rIndx] != NULL)
        {
            newTrustScore = newTrustScore + ((highestRecommenders[rIndx]->sRequester->credibility / netSRCredibility) * highestRecommenders[rIndx]->trustScore);
        }
    }

    if(newTrustScore > 0.00)
        SP->trustScore = newTrustScore;
    */
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
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t Avg Queue\t InQ\t Busy Period\t Next Avail Slot\tTrust Score\tAdv Wait Time\tActual Wait Time\tProjected Wait Time\n");
    real ct = time();
    for(int i = 0; i < N_SP; i++)
    {
        int fbIndx, fbSRCount;
        real posFB, negFB, trustScore, advertisedWT, actualWT, projectedWaitTime;
        advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], ct);
        projectedWaitTime = getProjectedWaitTime(&providerArray[i], NULL, advertisedWT);
        actualWT = providerArray[i].nextAvailTime - ct;
        if(actualWT < 0.016700)
        {
            actualWT = 0.016700;
        }
        posFB = 0.0;
        negFB = 0.0;
        trustScore = 0.0;
        fbSRCount = 0;
        for(fbIndx = 0; fbIndx < N_SR; fbIndx++)
        {
            posFB = posFB + providerArray[i].feedbacks[fbIndx].positiveFeedback;
            negFB = negFB + providerArray[i].feedbacks[fbIndx].negativeFeedback;

            if(providerArray[i].feedbacks[fbIndx].trustScore > 0.0)
            {
                fbSRCount++;
                trustScore = trustScore + providerArray[i].feedbacks[fbIndx].trustScore;
            }
        }
        if(fbSRCount > 0)
        {
            trustScore = trustScore / fbSRCount;
        }
        printf("SP%d \t %d\t%d\t\t %d \t %.2f \t %.2f \t %d \t %.2f \t %.2f \t\t %.2f\t %.2f\t %.2f\t%.2f\n", i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious, U(providerArray[i].id), Lq(providerArray[i].id), inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime, providerArray[i].trustScore, advertisedWT, actualWT, projectedWaitTime);
        /*for(int as = 0; as < M_SP; as++)
        {
        	printf("\t\tAvail Slot: %d, Value: %.2f\n", as, providerArray[i].availabilitySlotList[as]);
        }*/
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

void printRecommenders(int customerId){
    printf("Current Time: %f\n", time());
    for(int spIndx = 0; spIndx < N_SP; spIndx++){
        printf("SPID: %d\tIsMalicious: %d\n", spIndx, providerArray[spIndx].isMalicious);
        int witnessCount = 0;
        struct Witness witnessArray[N_SR];
        getrecommendersForSR(&customerArray[customerId], &providerArray[spIndx], witnessArray, &witnessCount);
        if(witnessCount > 0){
            printf("\tWitness Count: %d\n", witnessCount);
            for(int wtIndx = 0; wtIndx < N_SR; wtIndx++) {
                if(witnessArray[wtIndx].sRequester == NULL){
                    break;
                }
                //Service experience history
                char serviceExp[150];
                strcpy(serviceExp,"");
                for(int seIndx = 0; seIndx <  customerArray[witnessArray[wtIndx].sRequester->id].experienceHistory[spIndx].historyCount; seIndx++){
                    char str[19];
                    sprintf(str,"%.1f|%.3f|%.3f || ", customerArray[witnessArray[wtIndx].sRequester->id].experienceHistory[spIndx].servExp[seIndx].lastServiceRating, customerArray[witnessArray[wtIndx].sRequester->id].experienceHistory[spIndx].servExp[seIndx].lastServiceActualWaitTime, customerArray[witnessArray[wtIndx].sRequester->id].experienceHistory[spIndx].servExp[seIndx].lastServiceAdvertisedWaitTime);
                    strcat(serviceExp, str);
                }
                printf("\tWitness Id: %d, Is Malicious: %d, Honesty: %f, Experience History: %s\n",
                       witnessArray[wtIndx].sRequester->id, witnessArray[wtIndx].sRequester->isMalicious,
                       witnessArray[wtIndx].sRequester->feedbacks[1].trustScore,
                       serviceExp);
            }
        }
        else
        {
            int totalSRwithExp = 0;
            for(int srIndx = 0; srIndx < N_SR; srIndx++){
                if(customerArray[srIndx].serviceExperiences[spIndx].lastServiceRating > -1.0){
                    totalSRwithExp++;
                }
            }
            printf("\tTotal SRs with experience: %d\n", totalSRwithExp);
        }
    }
}

void testReport(real *report1LastPrinted, real currentTime)
{
    //int testCustomerId = 1;

    //if(customer != testCustomerId){ return;}
    if(event != 3){
        return;
    }
    //if((customer == testCustomerId) || (*report1LastPrinted <= (currentTime + 5.0))) {
    if((*report1LastPrinted <= (currentTime + 5.0))) {
        *report1LastPrinted += 5;
        /*printRecommenders(testCustomerId);
        return;*/
        /*printf("Current Event: %d", event);
        if(event == 2 && customerArray[testCustomerId].currentSP != NULL){
            printf("\tCurrent SP: %d\n", customerArray[testCustomerId].currentSP->id);
        }
        else{
            printf("\n");
        }*/
        printf("SRId\tSPId\tFId\tVstrs\tMal\tInQ\tBusy\tNxtAvl\tTrst\tAdvr\tActl\tPrjctd\tT Vsts\tCurrT\tTstSRVsts\tLstRtng|ActWT|AdvWT\tWtnsCount\n");

        int totalVisitors, witnessCount;
        char serviceExp[150];
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
                real trustScore, advertisedWT, actualWT, projectedWaitTime;
                advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], currentTime);
                projectedWaitTime = getProjectedWaitTime(&providerArray[i], &customerArray[testCustomerId],
                                                         advertisedWT);
                actualWT = providerArray[i].nextAvailTime - currentTime;
                if (actualWT < 0.016700) {
                    actualWT = 0.016700;
                }

                witnessCount = 0;
                struct Witness witnessArray[N_SR];
                getrecommendersForSR(&customerArray[testCustomerId], &providerArray[i], witnessArray, &witnessCount);
                trustScore = calculateSPTrustScoreBySR(&providerArray[i], &customerArray[testCustomerId], witnessArray,
                                                       witnessCount);

                printf("SR%d\tSP%d\t%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%.2f\t%d\t%s\t%d\n", testCustomerId,
                       i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious,
                       inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime, trustScore,
                       advertisedWT, actualWT, projectedWaitTime, totalVisitors, currentTime,
                       providerArray[i].feedbacks[testCustomerId].totalFeedback, serviceExp, witnessCount);
                /*for(int as = 0; as < M_SP; as++)
                {
                    printf("\t\tAvail Slot: %d, Value: %.2f\n", as, providerArray[i].availabilitySlotList[as]);
                }*/
            }
        }
        printf("********************************************\n");
    }
}

void figure1Report(real* lastReportedTime, real currentTime)
{
    //printf("I am in case 1\n");
    //printf("last reported time: %.2f, currentTime: %.2f\n", *lastReportedTime, currentTime);
    //int currentTimeInt = (int)currentTime;
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
            projectedWaitTime = getProjectedWaitTime(&providerArray[i], &customerArray[reportingCustomer], advertisedWT);
            actualWT = providerArray[i].nextAvailTime - currentTime;
            if(actualWT < 0.016700)
            {
                actualWT = 0.016700;
            }
            struct Witness witnessArray[N_SR];
            getrecommendersForSR(&customerArray[reportingCustomer], &providerArray[i], witnessArray, &wtCount);

            trustScore = calculateSPTrustScoreBySR(&providerArray[i], &customerArray[reportingCustomer], witnessArray, wtCount);


            printf("SP%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%.2f\n", i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious, U(providerArray[i].id), Lq(providerArray[i].id), inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime, providerArray[i].trustScore, advertisedWT, actualWT, projectedWaitTime, totalVisitors, currentTime);
            /*for(int as = 0; as < M_SP; as++)
            {
                printf("\t\tAvail Slot: %d, Value: %.2f\n", as, providerArray[i].availabilitySlotList[as]);
            }*/
        }
    }
}

void myReport(int isFinalReport)
{
    printf("\nSimulation Report\n");
    printBasicFacilityInfo();
    printBasicCustomerInfo();
    //testReport();
}