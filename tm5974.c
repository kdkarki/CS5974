#include "smpl.h"
#include <math.h>

// Table 1 parameters
#define N_SP	10
#define N_SR	2200
#define M_SP	20
#define Ta	10.0
#define Ts	1.0
#define T_Margin	0.25 //threshold for wait time to be considered inaccurate
#define WaitTime_Offset	0.75 //the wait time offset reported by a malicious SP
#define NON_EXISTENT -1

void myReport();
real getSPAdvertisedWaitTime();
struct ServiceProvider* getLeastBusySP();
void addNewSRToQueue();
void removeSRFromQueue();
void addSRToSPServiceArray();
void removeSRFromSPServiceArray();
void updateFeedbackAndWaitTime();

struct ServiceRequester
{
	int id;
	real startTime;//simulation clock time when the customer was first added to the system
	real currentServiceTime; //the exponential service time length this SR will use the SP for
	real currentSPServiceStartTime; //simulation clock time when the SR received service from SR
	real currentSPQueueStartTime; //simulation clock time when the SR was selected to request a server
	real currentSPExperiencedWaitTime; //the length of time advertised by SP
	int visitsPerSP[N_SP];
	int isMalicious;
	int positiveFeedback;
	int negativeFeedback;
	struct ServiceProvider* currentSP;
};

struct SRFeedback
{
	int positiveFeedback;
	int negativeFeedback;
	struct ServiceRequester* sRequester;
};

struct ServiceProvider
{
	int id;
	int numberOfSRVisitors;
	real trustScore;
	real nextAvailTimeSlot;//simulation clock time when the SP is available to service a new customer
	int isMalicious;	
	//int currentSRInQueue[M_SP];
	real availabilitySlotList[M_SP];//this is used to calculate the next available time
	struct ServiceRequester* currentSRInService[M_SP];//SRs being serviced by the SP
	struct SRFeedback feedbacks[N_SR];
};

int main()
{
	struct ServiceProvider serviceProviderArray[N_SP];
	struct ServiceRequester customerArray[N_SR];
	int i,j;

	real te=200.0;
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
		customerArray[j].currentSPExperiencedWaitTime = 0.0;
		int s;
		for(s = 0; s < N_SP; s++)
		{
			customerArray[j].visitsPerSP[s] = 0;
		}
		if(j % 25 == 0)
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
			serviceProviderArray[spIndx].feedbacks[j].positiveFeedback = 0;//for beta reputation, the initial
			serviceProviderArray[spIndx].feedbacks[j].negativeFeedback = 0;//rating should be 0.5 so both positive and negative feedback is 1 
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
				//customerArray[customer].currentSPExperiencedWaitTime = getSPAdvertisedWaitTime(selectedSP, currentTime);
				
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
			}
			if (request(customerArray[customer].currentSP->id,customer,0)==0) then
			{
				//capture the clock time when this customer received the service
				customerArray[customer].currentSPServiceStartTime = time();
				//printf("customer %d obtains service from SP with fd=%d; wait time is %f\n", customer, sp_id[customer], time()-ts[customer]);
				
				//add this SR to the SP.currentSRInService array
				addSRToSPServiceArray(&customerArray[customer], serviceProviderArray);

				//update the feedback to SP and SR based on the wait time
				updateFeedbackAndWaitTime(&customerArray[customer], serviceProviderArray, customerArray);

				schedule(3,customerArray[customer].currentServiceTime,customer);
			}
			break;
			case 3:  /* release server */
			release(customerArray[customer].currentSP->id,customer);
			//printf("departure time of customer %d from SP with fd=%d is %f; total response time is %f\n", customer, sp_id[customer], time(), time()-ts[customer]);
			
			//remove the current SR from SP.currentSRInService array
			removeSRFromSPServiceArray(&customerArray[customer], serviceProviderArray);

			customerArray[customer].currentSP = NON_EXISTENT; // reset so that next time we know this customer will arrive anew
			customerArray[customer].currentSPServiceStartTime = 0.0; 
			customerArray[customer].currentSPQueueStartTime = 0.0;
			customerArray[customer].currentSPExperiencedWaitTime = 0.0;
			
			schedule(1,expntl(Ta), customer); /* schedule next arrival time */
			break;
		}
	}
	myReport(serviceProviderArray, customerArray);
}

void myReport(struct ServiceProvider* SPArray, struct ServiceRequester* CustomerArray)
{
	printf("\nSimulation Report\n");
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t Avg Queue\t In Queue\t Busy Period\t Current Wait Time \t + Feedback \t - Feedback\n");
    int i;
    for(i = 0; i < N_SP; i++)
    {
    	int fbIndx, posFB, negFB;
    	posFB = 0;
    	negFB = 0;
    	for(fbIndx = 0; fbIndx < N_SR; fbIndx++)
    	{
    		posFB = posFB + SPArray[i].feedbacks[fbIndx].positiveFeedback;
    		negFB = negFB + SPArray[i].feedbacks[fbIndx].negativeFeedback;
    	}
        printf("SP%d \t %d \t %d     \t %d \t %f \t %f \t %d \t\t %f \t %f \t\t     %d \t     %d\n", i, SPArray[i].id, SPArray[i].numberOfSRVisitors, SPArray[i].isMalicious, U(SPArray[i].id), Lq(SPArray[i].id), inq(SPArray[i].id), B(SPArray[i].id), SPArray[i].nextAvailTimeSlot, posFB, negFB);
    }
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
    int cIndx;
    for(cIndx = 0; cIndx < N_SR; cIndx++)
    {
    	printf("Customer ID: %d\t %d\t%d\n", cIndx, CustomerArray[cIndx].positiveFeedback, CustomerArray[cIndx].negativeFeedback);
    }
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

void updateFeedbackAndWaitTime(struct ServiceRequester* SR, struct ServiceProvider* SPArray, struct ServiceRequester* SRArray)
{
	int spIndx, fdIndx, feedbackSP, feedbackSR;
	struct ServiceProvider* SP;

	//determine the acutal wait time by subtracting the clock time when the SR was in queue from when SR received service
	real currentSRActualWaitTime = SR->currentSPServiceStartTime - SR->currentSPQueueStartTime;

	real currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(SR->currentSP, time());

	//determine the difference between the actual wait time and current advertised wait time
	real waitTimeDiff = currentSRActualWaitTime - currentSPAdvertisedWaitTime;

	//determine the perentage of difference
	real diffPercentage = waitTimeDiff / currentSRActualWaitTime;

	if(diffPercentage > T_Margin)
	{
		feedbackSP = -1;
	}
	else
	{
		feedbackSP = 1;
	}
	
	for(spIndx = 0; spIndx < N_SP; spIndx++)
	{
		if(SR->currentSP->id == SPArray[spIndx].id)
		{
			SP = &SPArray[spIndx];
			break;
		}
	}

	if(feedbackSP == 1)
	{
		SP->feedbacks[SR->id].positiveFeedback++;
	}
	else
	{
		SP->feedbacks[SR->id].negativeFeedback++;
	}

	//now provide feedback for each witness. Witness is an SR that is currently in service
	for(fdIndx = 0; fdIndx < M_SP; fdIndx++)
	{
		//do NOT give feedback to ownself
		if(SP->currentSRInService[fdIndx] != NON_EXISTENT && SP->currentSRInService[fdIndx]->id != SR->id)
		{
			if(SP->currentSRInService[fdIndx]->currentSP->id != SP->id)
			{
				printf("ERROR ***** SP->currentSRInService[fdIndx]->currentSP->id != SP->id ***** ERROR");
			}
			//determine the difference between the wait time experienced by current SR and the witness
			real waitTimeDiffWithWitness = currentSRActualWaitTime - SP->currentSRInService[fdIndx]->currentSPExperiencedWaitTime;

			//determine the perentage of difference
			real wtDiffPercentage = waitTimeDiffWithWitness / currentSRActualWaitTime;

			if(diffPercentage > T_Margin)
			{
				feedbackSR = -1;
			}
			else
			{
				feedbackSR = 1;
			}

			if(feedbackSR == 1)
			{
				SP->currentSRInService[fdIndx]->positiveFeedback++;
			}
			else
			{
				SP->currentSRInService[fdIndx]->negativeFeedback++;
			}
		}
	}

	//also update the actual wait time observed by the current SR so that it can act as witness
	//A malicious SR will advertise incorrect wait time for non-malicious SP and will advertise 
	//the same wait time as SP for malicious SP

	real multiplier;
	if(SR->isMalicious == 1)
	{
		multiplier = 1.0 + (1.0 - WaitTime_Offset);
	}
	else
	{
		multiplier = 1.0;
	}

	SR->currentSPExperiencedWaitTime = multiplier * currentSPAdvertisedWaitTime;
				
}
