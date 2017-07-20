#include "smpl.h"
#include <math.h>
#include "tm5974.h"
#include <tm5974Structs.c>


int main()
{
	real maxTsTime, minTsTime;
	maxTsTime = 0.0;
	minTsTime = 0.0;
	struct ServiceProvider serviceProviderArray[N_SP];
	struct ServiceRequester customerArray[N_SR];
	int i,j;

	real te=2000.0;
	int customer=0;
	int server=0;
	int event;
	//real ts[N_SR]; /* start time stamp */
	//int sp_id[N_SR]; /* SP ID selected */

	smpl(0,"5974");
	//trace(2);

	for(i = 0; i < N_SP; i++)
	{
        char s[64];
        sprintf(s, "SP%d", i);
        int spid = facility(s, M_SP);
		serviceProviderArray[i].id = spid;//facility(s, M_SP);
        serviceProviderArray[i].numberOfSRVisitors = 0;
        serviceProviderArray[i].trustScore = 0.0;
        serviceProviderArray[i].nextAvailTimeSlot = 0.0;
        serviceProviderArray[i].isMalicious = 0;
              
        
        if(i == 3 || i == 6)
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
		}
		
		if(j % 10 == 0)
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
			serviceProviderArray[spIndx].feedbacks[j].positiveFeedback = 0.0;//for beta reputation, the initial
			serviceProviderArray[spIndx].feedbacks[j].negativeFeedback = 0.0;//rating should be 0.5 so both positive and negative feedback is 1 
		}
	}

	while (time()<te)
	{
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
				customerArray[customer].currentSPQueueStartTime = time();
				customerArray[customer].currentServiceTime = expntl(Ts);
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
				updateFeedbackAndWaitTime(&customerArray[customer]);//, serviceProviderArray, customerArray);

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
			}
			customerArray[customer].currentSPServiceStartTime = 0.0; 
			customerArray[customer].currentSPQueueStartTime = 0.0;
			customerArray[customer].currentSPWaitTimeRating = NON_EXISTENT;
			
			schedule(1,expntl(Ta), customer); /* schedule next arrival time */
			break;
		}
	}
	myReport(serviceProviderArray, customerArray);

	printf("\n");

	printf("Max Ts Time: %f\tMin Ts Time: %f\n", maxTsTime, minTsTime);
}

void myReport(struct ServiceProvider* SPArray, struct ServiceRequester* CustomerArray)
{
	printf("\nSimulation Report\n");
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t Avg Queue\t In Queue\t Busy Period\t Current Wait Time \t\t + Feedback \t\t - Feedback\n");
    int i;
    for(i = 0; i < N_SP; i++)
    {
    	int fbIndx;
    	real posFB, negFB;
    	posFB = 0.0;
    	negFB = 0.0;
    	for(fbIndx = 0; fbIndx < N_SR; fbIndx++)
    	{
    		posFB = posFB + SPArray[i].feedbacks[fbIndx].positiveFeedback;
    		negFB = negFB + SPArray[i].feedbacks[fbIndx].negativeFeedback;
    	}
        printf("SP%d \t %d \t %d     \t %d \t %f \t %f \t %d \t\t %f \t %f \t\t     %f \t     %f\n", i, SPArray[i].id, SPArray[i].numberOfSRVisitors, SPArray[i].isMalicious, U(SPArray[i].id), Lq(SPArray[i].id), inq(SPArray[i].id), B(SPArray[i].id), SPArray[i].nextAvailTimeSlot, posFB, negFB);
    }
    char str[8];
    scanf("%s", str);
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
    real totalMalHonesty, totalNonMalHonesty;
    malSRCount = 0;
    nonMalSRCount = 0;
    totalMalHonesty = 0.0;
    totalNonMalHonesty = 0.0;
    printf("CId \t + Feedback\t- Feedback\t Centrality\t Honesty\t Credibility\t\t Mal\n");
    for(cIndx = 0; cIndx < N_SR; cIndx++)
    {
    	real centrality = CustomerArray[cIndx].positiveFeedback + CustomerArray[cIndx].negativeFeedback;

    	if(CustomerArray[cIndx].isMalicious == 1)
    	{
    		malSRCount++;
    		totalMalHonesty += CustomerArray[cIndx].honesty;
    	}
    	else
    	{
    		nonMalSRCount++;
    		totalNonMalHonesty += CustomerArray[cIndx].honesty;
    	}

    	printf("%d\t %f\t%f\t %f\t %f\t %f\t\t %d\n", cIndx, CustomerArray[cIndx].positiveFeedback, CustomerArray[cIndx].negativeFeedback, centrality, CustomerArray[cIndx].honesty, CustomerArray[cIndx].credibility, CustomerArray[cIndx].isMalicious);
    	if(cIndx == 500)
    	{
    		scanf("%s", str);
    	}
    }
    printf("Average Malicious Honesty: %f\nAverage Non-Malicious Honesty: %f\n", (totalMalHonesty/malSRCount), (totalNonMalHonesty/nonMalSRCount));
}

real getSPAdvertisedWaitTime(struct ServiceProvider* SP, real currentTime)
{
	real multiplier;

	if(SP->isMalicious == 1)
	{	
		multiplier = WaitTime_Offset;
	}
	else
	{
		multiplier = 1.0;
	}
	real advertisedWaitTime;
	advertisedWaitTime = multiplier * (SP->nextAvailTimeSlot - currentTime);
	if(advertisedWaitTime > 0.0)
	{
		//printf("Advertised wait time greater than 0.00. SP%d, wait time: %f\n", SP->id, advertisedWaitTime);
	}
	if(advertisedWaitTime < 0)
	{
		advertisedWaitTime = 0.0;
	}
	return advertisedWaitTime;
}

struct ServiceProvider* getLeastBusySP(struct ServiceProvider* SPs, real currentTime)
{
	int s;
    struct ServiceProvider* selectedSP = &SPs[0];
    real leastWaitTime = getSPAdvertisedWaitTime(&SPs[0], currentTime);//current customers in queue
    //real leastWaitTime = leastAvailabilityTime - currentTime;
    
    int selectedSPVisitors = SPs[0].numberOfSRVisitors;

    for(s = 1; s < N_SP; s++)
    {
        real currentSPLeastWaitTime = getSPAdvertisedWaitTime(&SPs[s], currentTime); 
        //real currentSPLeastWaitTime = currentSPAvailabilityTime - currentTime;
        if(currentSPLeastWaitTime < leastWaitTime)// || (currentSPLeastWaitTime == leastWaitTime && selectedSPVisitors > SPs[s].numberOfSRVisitors))
        {
            selectedSP = &SPs[s];
            //leastAvailabilityTime = currentSPAvailabilityTime;
            leastWaitTime = currentSPLeastWaitTime;
            selectedSPVisitors = SPs[s].numberOfSRVisitors;
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
			//SP->currentSRInService[srInServIndx] = SR->id;
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
void updateFeedbackAndWaitTime(struct ServiceRequester* SR)//, struct ServiceProvider* SPArray, struct ServiceRequester* SRArray)
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
		real waitTimeDiff = fabs(currentSRActualWaitTime - currentSPAdvertisedWaitTime);

		//determine the perentage of difference
		real diffPercentage = waitTimeDiff / currentSRActualWaitTime;

		if(diffPercentage > 0.25)
		{
			printf("diffPercentage is greater than 0.2. SR: %d\tSP: %d\tDifference: %f\n", SR->id, SP->id, diffPercentage);
		}

		if(diffPercentage < 0.2)
		{
			SR->currentSPWaitTimeRating = 5;
		}
		else if (diffPercentage >= 20 && diffPercentage < 40)
		{
			SR->currentSPWaitTimeRating = 4;
		}
		else if(diffPercentage >= 50 && diffPercentage < 60)
		{
			SR->currentSPWaitTimeRating = 3;
		}
		else if(diffPercentage >= 60 && diffPercentage < 80)
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

	if(f < 1.0)
	{
		//printf("f is less than 1.0. SR: %d\t SP: %d\t f: %f\t +ve: %f\t -ve: %f\n", SR->id, SP->id, f, SP->feedbacks[SR->id].positiveFeedback, SP->feedbacks[SR->id].negativeFeedback);
	}

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

			SR->witnesses[fdIndx].sRequester->honesty = SR->witnesses[fdIndx].sRequester->positiveFeedback / (SR->witnesses[fdIndx].sRequester->positiveFeedback + SR->witnesses[fdIndx].sRequester->negativeFeedback);
			SR->witnesses[fdIndx].sRequester->credibility = SR->witnesses[fdIndx].sRequester->honesty * (SR->witnesses[fdIndx].sRequester->positiveFeedback + SR->witnesses[fdIndx].sRequester->negativeFeedback);
		}
	}
				
}
