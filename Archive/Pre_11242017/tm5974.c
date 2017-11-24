#include <math.h>
#include "smpl.h"
#include "tm5974.h"
#include "tm5974Structs.c"


int main()
{
	real maxTsTime, minTsTime, highestCentrality;
	maxTsTime = 0.0;
	minTsTime = 0.0;
	highestCentrality = 0.0;

	struct ServiceProvider serviceProviderArray[N_SP];
	struct ServiceRequester customerArray[N_SR];
	int i,j;

	real te=10000.0;
	int customer=0;
	int server=0;
	int event;

	smpl(0,"5974");
	//trace(2);

	for(i = 0; i < N_SP; i++)
	{
        char s[64];
        sprintf(s, "SP%d", i);
        int spid = facility(s, M_SP);
		serviceProviderArray[i].id = spid;
        serviceProviderArray[i].numberOfSRVisitors = 0;
        serviceProviderArray[i].trustScore = 0.5;
        serviceProviderArray[i].nextAvailTimeSlot = 0.0;
        serviceProviderArray[i].netSRCredibility = 0.0;
        serviceProviderArray[i].isMalicious = 0;
              
        
        if(i == 1 || i == 3 || i == 5)// || i == 7 || i == 9)
        {
            serviceProviderArray[i].isMalicious = 1;
        }
        
        
		int sr, avlSL, srInService;
		//initialize availabilitySlotList
		avlSL = sizeof(serviceProviderArray[i].availabilitySlotList)/sizeof(real);
		for(sr = 0; sr < avlSL; sr++)
		{
			serviceProviderArray[i].availabilitySlotList[sr] = 0.0;	
		}

		srInService = sizeof(serviceProviderArray[i].currentSRInService)/sizeof(int);
		for(sr = 0; sr < avlSL; sr++)
		{
			serviceProviderArray[i].currentSRInService[sr] = NON_EXISTENT;	
		}
	}

	for(j = 0; j < N_SR; j++)
	{
        customerArray[j].startTime = expntl(Ta);
		schedule(1, customerArray[j].startTime, j);
		//printf("Scheduled arrival time of customer %d at %f\n", j, ts[j]);
 		customerArray[j].id = j;
		customerArray[j].currentSP = NON_EXISTENT;
		customerArray[j].currentServiceTime = 0.0;
		customerArray[j].currentSPServiceStartTime = 0.0;
		customerArray[j].currentSPQueueStartTime = 0.0;
		customerArray[j].currentSPWaitTimeRating = NON_EXISTENT;
		customerArray[j].positiveFeedback = 1.0;
		customerArray[j].negativeFeedback = 1.0;
		customerArray[j].honesty = 0.0;
		customerArray[j].credibility = 0.0;
		int s;
		for(s = 0; s < N_SP; s++)
		{
			customerArray[j].visitsPerSP[s] = 0;
		}
		for(s = 0; s < M_SP; s++)
		{
			customerArray[j].witnesses[s].sRequester = NON_EXISTENT;
			customerArray[j].witnesses[s].waitTimeRating = NON_EXISTENT;
			customerArray[j].witnesses[s].waitTime = NON_EXISTENT;
		}
		
		if((j % 5 == 0) || (j % 9 == 0))//30% //(j % 5 == 0)//20%//(j % 4 == 0 || j % 5 == 0)//40%
		{
			customerArray[j].isMalicious = 1;
		}
		else
		{
			customerArray[j].isMalicious = 0;
		}
		
		//initialize feedback for all the SPs
		//Since sRequester is pointer to ServiceRequester, this has to be done here.
		int spIndx;
		for(spIndx = 0; spIndx < N_SP; spIndx++)
		{
			serviceProviderArray[spIndx].feedbacks[j].sRequester = &customerArray[j];
			serviceProviderArray[spIndx].feedbacks[j].positiveFeedback = 1.0;//for beta reputation, the initial rating should be 0.5 
			serviceProviderArray[spIndx].feedbacks[j].negativeFeedback = 1.0;//so both positive and negative feedback is 1 
			serviceProviderArray[spIndx].feedbacks[j].trustScore = 0.0;
		}
	}

	int lastReportedTime = 0;
printf("C_ID\tTime\tF_ID\tInQ\tTrust Score\tActual WT\tAdv WT\t\tProjected WT\tActual-Projected WT Diff\n");
	while (time()<te)
	{
		real timeNow = time();
		int timeNowInt = (int) timeNow;
		//if(((timeNowInt % 2000 == 0) && timeNow > 1500.00 && timeNowInt > lastReportedTime) || (timeNow < 300.00 && (timeNowInt % 10 == 0) && timeNowInt > lastReportedTime) || (timeNow < 2000.00 && (timeNowInt % 100 == 0) && timeNowInt > lastReportedTime) || timeNow == 0.00)
		if(timeNow < 10000.00)
		{
			if(customer == 2 && customerArray[customer].currentSP != NON_EXISTENT && (customerArray[customer].currentSP->id == 67 || customerArray[customer].currentSP->id == 45))//SR 2 is non-malicious and SPID 67 is malicious and SPID 45 is non-malicious
			{
			lastReportedTime = timeNowInt;
			//printf("************************************ Current Time: %f ************************************\n", time());
			//myReport(serviceProviderArray, customerArray, &highestCentrality, 0);
			real advertisedWT, actualWT, projectedWT, actProjDiff;		
    			advertisedWT = getSPAdvertisedWaitTime(customerArray[customer].currentSP, timeNow);
    			actualWT = customerArray[customer].currentSP->nextAvailTimeSlot - timeNow;
			if(actualWT < 0.016700) { actualWT = 0.0167; }
			projectedWT = (advertisedWT / customerArray[customer].currentSP->trustScore);
			actProjDiff = (fabs(projectedWT - actualWT) / actualWT);
			//printf("C_ID\tTime\tF_ID\tInQ\tTrust Score\tActual WT\tAdv WT\t\tProjected WT\tActual-Projected WT Diff\n");
			printf("%d\t%f\t%d\t%d\t%f\t%f\t%f\t%f\t%f\n", customer, timeNow, customerArray[customer].currentSP->id, inq(customerArray[customer].currentSP->id), customerArray[customer].currentSP->trustScore, actualWT, advertisedWT, projectedWT, actProjDiff);
			printf("\n");
			}
		}
		else { break; }
		cause(&event,&customer);
		switch(event)
		{
			case 1:  /* arrival */
			customerArray[customer].startTime = time();
			//printf("arrival time of customer %d is %f\n", customer, time());
			schedule(2,0.0,customer);
			break;

			case 2:  /* request server */			
			// If the customer has selected a SP and was waiting in the queue and now it is rescheduled by smpl to event 2, 
			// then request service using the original SP fd ID which is stored in sp_id[customer];
			// otherwise, the customer has just arrived and has not yet selected a SP for service
			if (customerArray[customer].currentSP == NON_EXISTENT) // the customer has just arrived and has not selected a SP for service
			{				
				real currentTime = time();
				struct ServiceProvider* selectedSP = getLeastBusySP(serviceProviderArray, currentTime);  
				customerArray[customer].currentSP = selectedSP;
				//capture the clock time when this customer was assigned a SP
				customerArray[customer].currentSPQueueStartTime = currentTime;
				customerArray[customer].currentServiceTime = expntl(Ts);
				//Set the service time as a uniformly distributed value rather than exponentially distributed value
				//customerArray[customer].currentServiceTime = uniform(0.75 * Ts, 1.5 * Ts);
				customerArray[customer].currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(selectedSP, currentTime);
				//get all the witnesses so that they can be rated later when the service is received
				int wtIndx;
				for(wtIndx = 0; wtIndx < M_SP; wtIndx++)
				{
					customerArray[customer].witnesses[wtIndx].sRequester = selectedSP->currentSRInService[wtIndx];
					if(selectedSP->currentSRInService[wtIndx] != NON_EXISTENT)
					{
						customerArray[customer].witnesses[wtIndx].waitTimeRating = selectedSP->currentSRInService[wtIndx]->currentSPWaitTimeRating;
					}
				}
				selectedSP->numberOfSRVisitors++;
				//update the currentSRInQueue for this SP
				addNewSRToQueue(selectedSP, customerArray[customer]);
				int x;
				for(x = 0; x < N_SP; x++)
				{
					if(serviceProviderArray[x].id == customerArray[customer].currentSP->id)
					{
						customerArray[customer].visitsPerSP[x]++;
						break;
					}
				}

				if(maxTsTime < customerArray[customer].currentServiceTime)
				{
					maxTsTime = customerArray[customer].currentServiceTime;
				}
				if(minTsTime == 0.0)
				{
					minTsTime = customerArray[customer].currentServiceTime;
				}
				else if(minTsTime > customerArray[customer].currentServiceTime)
				{
					minTsTime = customerArray[customer].currentServiceTime;
				}
			}
			if (request(customerArray[customer].currentSP->id,customer,0)==0) then
			{
				//capture the clock time when this customer received the service
				customerArray[customer].currentSPServiceStartTime = time();
				//printf("customer %d obtains service from SP with fd=%d; wait time is %f\n", customer, sp_id[customer], time()-ts[customer]);
				
				//add this SR to the SP.currentSRInService array
				addSRToSPServiceArray(&customerArray[customer], serviceProviderArray);

				//update the feedback to SP and SR based on the wait time
				updateFeedbackAndWaitTime(&customerArray[customer], &highestCentrality);

				schedule(3,customerArray[customer].currentServiceTime,customer);
			}
			break;
			case 3:  /* release server */
			release(customerArray[customer].currentSP->id,customer);
			//printf("departure time of customer %d from SP with fd=%d is %f; total response time is %f\n", customer, sp_id[customer], time(), time()-ts[customer]);
			
			//remove the current SR from SP.currentSRInService array
			removeSRFromSPServiceArray(&customerArray[customer], serviceProviderArray);

			customerArray[customer].currentSP = NON_EXISTENT; // reset so that next time we know this customer will arrive anew
			//reset witness for next time
			int s;
			for(s = 0; s < M_SP; s++)
			{
				customerArray[customer].witnesses[s].sRequester = NON_EXISTENT;
				customerArray[customer].witnesses[s].waitTimeRating = NON_EXISTENT;
				customerArray[customer].witnesses[s].waitTime = NON_EXISTENT;
			}
			customerArray[customer].currentSPServiceStartTime = 0.0; 
			customerArray[customer].currentSPQueueStartTime = 0.0;
			customerArray[customer].currentSPWaitTimeRating = NON_EXISTENT;
			
			schedule(1,expntl(Ta), customer); /* schedule next arrival time */
			//schedule(1, uniform((0.5 * Ta),(1.5 * Ta)), customer);//uniform arrival time
			break;
		}
	}
	myReport(serviceProviderArray, customerArray, &highestCentrality, 1);

	printf("\n");

	printf("Max Ts Time: %f\tMin Ts Time: %f\n", maxTsTime, minTsTime);
}

void myReport(struct ServiceProvider* SPArray, struct ServiceRequester* CustomerArray, real* highestCentrality, int isFinalReport)
{
	printf("\nSimulation Report\n");
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t Avg Queue\t InQ\t Busy Period\t Next Avail Slot\tTrust Score\tAdv Wait Time\tActual Wait Time\n");
    int i;
    real ct = time();
    for(i = 0; i < N_SP; i++)
    {
    	int fbIndx, fbSRCount;
    	real posFB, negFB, trustScore, advertisedWT, actualWT;
    	advertisedWT = getSPAdvertisedWaitTime(&SPArray[i], ct);
    	actualWT = SPArray[i].nextAvailTimeSlot - ct;
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
    		posFB = posFB + SPArray[i].feedbacks[fbIndx].positiveFeedback;
    		negFB = negFB + SPArray[i].feedbacks[fbIndx].negativeFeedback;

    		if(SPArray[i].feedbacks[fbIndx].trustScore > 0.0)
    		{
    			fbSRCount++;
    			trustScore = trustScore + SPArray[i].feedbacks[fbIndx].trustScore;
    		}
    	}
    	if(fbSRCount > 0)
    	{
    		trustScore = trustScore / fbSRCount;
    	}
        printf("SP%d \t %d \t %d     \t %d \t %f \t %f \t %d \t %f \t %f \t\t %f\t %f\t %f\n", i, SPArray[i].id, SPArray[i].numberOfSRVisitors, SPArray[i].isMalicious, U(SPArray[i].id), Lq(SPArray[i].id), inq(SPArray[i].id), B(SPArray[i].id), SPArray[i].nextAvailTimeSlot, SPArray[i].trustScore, advertisedWT, actualWT);
    }
    if(isFinalReport == 1)
    {
	    char str[8];
	    //scanf("%s", str);
	    //report();
	    /*for(i = 0; i < N_SP; i++)
	    {
	    	printf("\nCurrent SR in Service for SP%d\n\t", i);
	    	int sr;
	    	for(sr = 0; sr < M_SP; sr++)
	    	{
	    		printf("%d\t", SPArray[i].currentSRInService[sr]->id);
	    	}
	    }
	    */
	    printf("\n\n");
	    int cIndx,  malSRCount, nonMalSRCount; 
	    real totalMalHonesty, totalNonMalHonesty, totalMaliciousCredibility, totalNonMaliciousCredibility;
	    malSRCount = 0;
	    nonMalSRCount = 0;
	    totalMalHonesty = 0.0;
	    totalNonMalHonesty = 0.0;
	    totalMaliciousCredibility = 0.0;
	    totalNonMaliciousCredibility = 0.0;
	    printf("CId \t + Feedback\t- Feedback\t Centrality\t Norm. Centrality\t Honesty\t Credibility\t\t Mal\n");
	    for(cIndx = 0; cIndx < N_SR; cIndx++)
	    {
	    	real centrality = CustomerArray[cIndx].positiveFeedback + CustomerArray[cIndx].negativeFeedback;

	    	if(CustomerArray[cIndx].isMalicious == 1)
	    	{
	    		malSRCount++;
	    		totalMalHonesty += CustomerArray[cIndx].honesty;
	    		totalMaliciousCredibility += CustomerArray[cIndx].credibility;
	    	}
	    	else
	    	{
	    		nonMalSRCount++;
	    		totalNonMalHonesty += CustomerArray[cIndx].honesty;
	    		totalNonMaliciousCredibility += CustomerArray[cIndx].credibility;
	    	}
	    	
	    	printf("%d\t %f\t %f\t %f\t %f\t\t %f\t %f\t\t %d\n", cIndx, CustomerArray[cIndx].positiveFeedback, CustomerArray[cIndx].negativeFeedback, centrality, (centrality / *highestCentrality), CustomerArray[cIndx].honesty, CustomerArray[cIndx].credibility, CustomerArray[cIndx].isMalicious);
	    	
	    }
	    printf("Average Malicious SR Honesty: %f\nAverage Non-Malicious SR Honesty: %f\n", (totalMalHonesty/malSRCount), (totalNonMalHonesty/nonMalSRCount));
	    printf("Average Malicious SR Credibility: %f\nAverage Non-Malicious SR Credibility: %f\n", (totalMaliciousCredibility/malSRCount), (totalNonMaliciousCredibility/nonMalSRCount));
	    printf("Highest Centrality: %f\n", *highestCentrality);
	}
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
	advertisedWaitTime = multiplier * (SP->nextAvailTimeSlot - currentTime);
	

	if(advertisedWaitTime < 0.0167)
	{
		advertisedWaitTime = 0.0167; //minimum wait time is 1 minute
	}
	return advertisedWaitTime;
}

real getProjectedWaitTime(struct ServiceProvider* SP, real advertisedWaitTime)
{

}

struct ServiceProvider* getLeastBusySP(struct ServiceProvider* SPs, real currentTime)
{
	int s;
    struct ServiceProvider* selectedSP = &SPs[0];
    real leastWaitTime = getSPAdvertisedWaitTime(&SPs[0], currentTime);//current customers in queue
    leastWaitTime = leastWaitTime / SPs[0].trustScore;

    for(s = 1; s < N_SP; s++)
    {
        real currentSPLeastWaitTime = getSPAdvertisedWaitTime(&SPs[s], currentTime); 
        currentSPLeastWaitTime = currentSPLeastWaitTime / SPs[s].trustScore;
        if(currentSPLeastWaitTime < leastWaitTime)
        {
            selectedSP = &SPs[s];
            leastWaitTime = currentSPLeastWaitTime;
        }
    }
    return selectedSP;
}

void addNewSRToQueue(struct ServiceProvider *SP, struct ServiceRequester SR)
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
	if(SP->availabilitySlotList[leastTimeSlotIndex] == NON_EXISTENT || SP->availabilitySlotList[leastTimeSlotIndex] < currentTime)
	{
		SP->availabilitySlotList[leastTimeSlotIndex] = currentTime + SR.currentServiceTime;
	}
	else
	{
		SP->availabilitySlotList[leastTimeSlotIndex] = SP->availabilitySlotList[leastTimeSlotIndex] + SR.currentServiceTime;
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

	SP->nextAvailTimeSlot = leastWaitTime; //This is the time when the next slot will be available
}

void addSRToSPServiceArray(struct ServiceRequester* SR, struct ServiceProvider* SPArray)
{
	struct ServiceProvider* SP;
	int spIndx;
	for(spIndx = 0; spIndx < N_SP; spIndx++)
	{
		if(SR->currentSP->id == SPArray[spIndx].id)
		{
			SP = &SPArray[spIndx];
			break;
		}
	}

	int srInServIndx;
	for(srInServIndx = 0; srInServIndx < M_SP; srInServIndx++)
	{
		if(SP->currentSRInService[srInServIndx] == NON_EXISTENT)
		{
			SP->currentSRInService[srInServIndx] = SR;
			break;
		}
	}
}

void removeSRFromSPServiceArray(struct ServiceRequester* SR, struct ServiceProvider* SPArray)
{
	struct ServiceProvider* SP;
	int spIndx;
	for(spIndx = 0; spIndx < N_SP; spIndx++)
	{
		if(SR->currentSP->id == SPArray[spIndx].id)
		{
			SP = &SPArray[spIndx];
			break;
		}
	}

	int srInServIndx;
	for(srInServIndx = 0; srInServIndx < M_SP; srInServIndx++)
	{
		if(SP->currentSRInService[srInServIndx] == SR)
		{
			SP->currentSRInService[srInServIndx] = NON_EXISTENT;
			break;
		}
	}
}

//Once an SR is able to successfully scheduled for service with an SP then the SR needs to provide feedback
//towards the SP as well as towards the WT based on its wait time experience. It also needs to update its 
//wait time rating towards the SP so that it can act as witness for others
void updateFeedbackAndWaitTime(struct ServiceRequester* SR, real* highestCentrality)
{
	//TODO: A Service Provider will advertise a minimum wait time of 1 minute even if the wait time is 0
	//		This needs to be taken into account when determining the wait time rating
	int spIndx, fdIndx;
	struct ServiceProvider* SP;
	SP = SR->currentSP;

	//First determine the actual wait time and set the wait time rating

	if(SR->isMalicious == 0)
	{
			//determine the acutal wait time by subtracting the clock time when the SR was in queue from when SR received service
		real currentSRActualWaitTime = SR->currentSPServiceStartTime - SR->currentSPQueueStartTime;

		real currentSPAdvertisedWaitTime = SR->currentSPAdvertisedWaitTime; //getSPAdvertisedWaitTime(SR->currentSP, time());

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
			/*printf("Wait time is more than 0.00. Current Time: %f SR: %d SP: %d In Queue: %d Current SR Actual Wait Time: %f SP Advertised Wait Time: %f\n", time(), SR->id, SP->id, inq(SP->id), currentSRActualWaitTime, currentSPAdvertisedWaitTime);
			char str[8];
    		scanf("%s", str);
    		printf("Advertised Wait Time: %f\n", getSPAdvertisedWaitTime(SP, time()));
    		*/
		}

		//determine the perentage of difference
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
			SR->currentSPWaitTimeRating = 5;
		}
		else if (diffPercentage >= 0.20 && diffPercentage < 0.40)
		{
			SR->currentSPWaitTimeRating = 4;
		}
		else if(diffPercentage >= 0.50 && diffPercentage < 0.60)
		{
			SR->currentSPWaitTimeRating = 3;
		}
		else if(diffPercentage >= 0.60 && diffPercentage < 0.80)
		{
			SR->currentSPWaitTimeRating = 2;
		}
		else
		{
			SR->currentSPWaitTimeRating = 1;
		}
	}
	else
	{
		if(SP->isMalicious == 1)
		{
			SR->currentSPWaitTimeRating = 5;
		}
		else
		{
			SR->currentSPWaitTimeRating = 1;
		}
	}
	
	//determine the normalized rating towards the SP for this transaction
	real f, newAlpha, newBeta;
	f = ((real) SR->currentSPWaitTimeRating) / 5.0;

	SP->feedbacks[SR->id].positiveFeedback = SP->feedbacks[SR->id].positiveFeedback + f;
	SP->feedbacks[SR->id].negativeFeedback = SP->feedbacks[SR->id].negativeFeedback + 1.0 - f;

	//calcualte the trust score towards the SP from this SR
	SP->feedbacks[SR->id].trustScore = SP->feedbacks[SR->id].positiveFeedback / (SP->feedbacks[SR->id].positiveFeedback + SP->feedbacks[SR->id].negativeFeedback);
	
	//now provide feedback for each witness. Witness is an SR that is currently in service
	for(fdIndx = 0; fdIndx < M_SP; fdIndx++)
	{
		//do NOT give feedback to ownself
		if(SR->witnesses[fdIndx].sRequester != NON_EXISTENT && SR->witnesses[fdIndx].sRequester->id != SR->id)
		{
			if(SP->currentSRInService[fdIndx]->currentSP->id != SP->id)
			{
				printf("ERROR ***** SP->currentSRInService[fdIndx]->currentSP->id != SP->id ***** ERROR");
			}

			int witnessRatingNew;
			real normRating;

			if(SR->isMalicious == 1)
			{
				if(SR->witnesses[fdIndx].sRequester->isMalicious == 1)
				{
					witnessRatingNew = 5;
				}
				else
				{
					witnessRatingNew = 1;
				}
			}
			else
			{
				//determine the difference between the wait time rating experienced by current SR and the witness
				int waitTimeRatingDiffWithWitness = abs(SR->currentSPWaitTimeRating - SR->witnesses[fdIndx].waitTimeRating);

				//determine the new rating for the witness
				witnessRatingNew = 5 - waitTimeRatingDiffWithWitness;
			}

			normRating = ((real) witnessRatingNew) / 5.0;

			SR->witnesses[fdIndx].sRequester->positiveFeedback = SR->witnesses[fdIndx].sRequester->positiveFeedback + normRating;
			SR->witnesses[fdIndx].sRequester->negativeFeedback = SR->witnesses[fdIndx].sRequester->negativeFeedback + 1 - normRating;

			if(*highestCentrality == 0.00 || (*highestCentrality < (SR->witnesses[fdIndx].sRequester->positiveFeedback + SR->witnesses[fdIndx].sRequester->negativeFeedback)))
			{
				*highestCentrality = SR->witnesses[fdIndx].sRequester->positiveFeedback + SR->witnesses[fdIndx].sRequester->negativeFeedback;
			}

			SR->witnesses[fdIndx].sRequester->honesty = SR->witnesses[fdIndx].sRequester->positiveFeedback / (SR->witnesses[fdIndx].sRequester->positiveFeedback + SR->witnesses[fdIndx].sRequester->negativeFeedback);
			SR->witnesses[fdIndx].sRequester->credibility = SR->witnesses[fdIndx].sRequester->honesty * ((SR->witnesses[fdIndx].sRequester->positiveFeedback + SR->witnesses[fdIndx].sRequester->negativeFeedback) / *highestCentrality);
		}
	}

	/*
	//To update the trust score towards SP, net credibility of all SR that have provided feedback to this SP needs to calculated first
	//first reset the net credibility
	SP->netSRCredibility = 0.00;
	for(fdIndx = 0; fdIndx < N_SR; fdIndx++)
	{
		if(SP->feedbacks[fdIndx].trustScore > 0.0)
		{
			SP->netSRCredibility = SP->netSRCredibility + SP->feedbacks[fdIndx].sRequester->credibility;
		}
	}
	*/

	//determine top N_Rec SR to calculate the trust score of SP
	struct SRFeedback *highestRecommenders[N_Rec];
	int rIndx, srIndx;
	for(rIndx = 0; rIndx < N_Rec; rIndx++)
	{
		highestRecommenders[rIndx] = NON_EXISTENT;
	}

	for(srIndx = 0; srIndx < N_SR; srIndx++)
	{
		if(SP->feedbacks[srIndx].trustScore > 0.0)
		{
			struct SRFeedback *currentRec = &SP->feedbacks[srIndx];
			for(rIndx = 0; rIndx < N_Rec; rIndx++)
			{
				if(highestRecommenders[rIndx] == NON_EXISTENT)
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
	real newTrustScore;
	newTrustScore = 0.0; //the new trust score of the SP calculated based on the latest updates

	// Calculate the new trust score as weighted sum of trust from each SR
	// where credibility of the SR is used for weight
	/*for(srIndx = 0; srIndx < N_SR; srIndx++)
	{
		//use the trust score of the SR only if the trust score the SR towards the SP is greater than 0.0
		//This will ensure that the SR has provided at least 1 feedback towards the SP
		if(SP->feedbacks[srIndx].trustScore > 0.0)
		{
			newTrustScore = newTrustScore + ((SP->feedbacks[srIndx].sRequester->credibility / SP->netSRCredibility) * SP->feedbacks[srIndx].trustScore);
		}
	}
	*/
	//determine the total credibility of of N_Rec
	SP->netSRCredibility = 0.0;
	for(rIndx = 0; rIndx < N_Rec; rIndx++)
	{
		if(highestRecommenders[rIndx] != NON_EXISTENT)
		{
			SP->netSRCredibility += highestRecommenders[rIndx]->sRequester->credibility;
		}
	}
	for(rIndx = 0; rIndx < N_Rec; rIndx++)
	{
		if(highestRecommenders[rIndx] != NON_EXISTENT)
		{
			newTrustScore = newTrustScore + ((highestRecommenders[rIndx]->sRequester->credibility / SP->netSRCredibility) * highestRecommenders[rIndx]->trustScore);
		}
	}

	if(newTrustScore > 0.00)
		SP->trustScore = newTrustScore;
				
}
