#include "smpl.h"
#include <math.h>

// Table 1 parameters
#define N_SP	10
#define N_SR	2200
#define M_SP	20
#define Ta	10.0
#define Ts	1.0

void myReport();
real getSPAdvertisedWaitTime();
struct ServiceProvider getLeastBusySP();
void addNewSRToQueue();
void removeSRFromQueue();

#define NON_EXISTENT -1

struct ServiceProvider
{
	int id;
	int numberOfSRVisitors;
	real trustScore;
	real actualWaitTime;
	int isMalicious;
	//int currentSRInService[M_SP];
	//int currentSRInQueue[M_SP];
	real availabilitySlotList[M_SP];//this is used to calculate the next available time
};

struct ServiceRequester
{
	int id;
	real startTime;
	int currentSPId;
	real currentServiceTime;
	real currentSPServiceStartTime;
	real currentSPQueueStartTime;
	real currentSPAdvertisedWaitTime;
	int visitsPerSP[N_SP];
	int isMalicious;
};

int main()
{
	struct ServiceProvider serviceProviderArray[N_SP];
	struct ServiceRequester customerArray[N_SR];
	int i,j;

	real te=20000.0;
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
        serviceProviderArray[i].actualWaitTime = 0.0;
        serviceProviderArray[i].isMalicious = 0;
              
        
        if(i == 3 || i == 6)
        {
            serviceProviderArray[i].isMalicious = 1;
        }
        
        
		int sr;
		/*
		for(sr = 0; sr < M_SP; sr++)
		{
			serviceProviderArray[i].currentSRInService[sr] = NON_EXISTENT;                 
		}
		//initialize currentSRInQueue array
		int srInQ = sizeof(serviceProviderArray[i].currentSRInQueue)/sizeof(int);
		for(sr = 0; sr < srInQ; sr++)
		{
			serviceProviderArray[i].currentSRInQueue[sr] = NON_EXISTENT;	
		}
		*/
		//initialize availabilitySlotList
		int avlSL = sizeof(serviceProviderArray[i].availabilitySlotList)/sizeof(real);
		for(sr = 0; sr < avlSL; sr++)
		{
			serviceProviderArray[i].availabilitySlotList[sr] = 0.0;	
		}
	}

	for(j = 0; j < N_SR; j++)
	{
                //ts[j] = expntl(Ta);
		//schedule(1, ts[j], j);		
		customerArray[j].startTime = expntl(Ta);
		schedule(1, customerArray[j].startTime, j);
		//printf("Scheduled arrival time of customer %d at %f\n", j, ts[j]);
 		customerArray[j].id = j;
		customerArray[j].currentSPId = NON_EXISTENT;
		customerArray[j].currentServiceTime = 0.0;
		customerArray[j].currentSPServiceStartTime = 0.0;
		customerArray[j].currentSPQueueStartTime = 0.0;
		customerArray[j].currentSPAdvertisedWaitTime = 0.0;
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
			if (customerArray[customer].currentSPId == NON_EXISTENT) // the customer has just arrived and has not selected a SP for service
			{
				// if (++server >= N_SP) server=0; //rotating SP for now; later on it needs to be replaced by trust-based SP selection
				//sp_id[customer] = serviceProviderArray[server];
				struct ServiceProvider selectedSP = getLeastBusySP(serviceProviderArray);  
				customerArray[customer].currentSPId = selectedSP.id;
				customerArray[customer].currentSPQueueStartTime = time();
				customerArray[customer].currentServiceTime = expntl(Ts);
				customerArray[customer].currentSPAdvertisedWaitTime = getSPAdvertisedWaitTime(selectedSP);
				//selectedSP.numberOfSRVisited++;
				int x;
				for(x = 0; x < N_SP; x++)
				{
					if(serviceProviderArray[x].id == customerArray[customer].currentSPId)
					{
						serviceProviderArray[x].numberOfSRVisitors++;
						customerArray[customer].visitsPerSP[x]++;
						//update the currentSRInQueue for this SP
						addNewSRToQueue(&serviceProviderArray[x], customerArray[customer]);
						break;
					}
				}
			}
			if (request(customerArray[customer].currentSPId,customer,0)==0) then
			{
				customerArray[customer].currentSPServiceStartTime = time();
				//printf("customer %d obtains service from SP with fd=%d; wait time is %f\n", customer, sp_id[customer], time()-ts[customer]);
				/*int y;
				for(y = 0; y < N_SP; y++)
				{
					if(serviceProviderArray[y].id == customerArray[customer].currentSPId)
					{
						int csr;
						for(csr = 0; csr < M_SP; csr++)
						{
							if(serviceProviderArray[y].currentSRInService[csr] == NON_EXISTENT)
							{
								serviceProviderArray[y].currentSRInService[csr] = customer;
								break;	
							}	
						}	
						//remove current SR from currentSRInQueue array
						removeSRFromQueue(&serviceProviderArray[y], customerArray[customer]);
					}	
				}
				*/
				schedule(3,customerArray[customer].currentServiceTime,customer);
				//schedule(3, Ts, customer);
			}
			break;
			case 3:  /* release server */
			release(customerArray[customer].currentSPId,customer);
			//printf("departure time of customer %d from SP with fd=%d is %f; total response time is %f\n", customer, sp_id[customer], time(), time()-ts[customer]);
			/*
			int s;
			for(s = 0; s < N_SP; s++)
			{
				if(serviceProviderArray[s].id == customerArray[customer].currentSPId)
				{
					int csr;
					for(csr = 0; csr < M_SP; csr++)
					{
						if(serviceProviderArray[s].currentSRInService[csr] == customer)
						{
							serviceProviderArray[s].currentSRInService[csr] = NON_EXISTENT;	
							break;
						}	
					}	
				}	
			}
			*/
			customerArray[customer].currentSPId = NON_EXISTENT; // reset so that next time we know this customer will arrive anew
			customerArray[customer].currentSPServiceStartTime = 0.0; 
			customerArray[customer].currentSPQueueStartTime = 0.0;
			customerArray[customer].currentSPAdvertisedWaitTime = 0.0;
			//TODO: Recalculate the wait time of the SP just released.
			
			schedule(1,expntl(Ta), customer); /* schedule next arrival time */
			break;
		}
	}
	myReport(serviceProviderArray);
	
	/*int r;
	for(r = 0; r < N_SR; r++)
	{
		printf("\nCId: %d \t Is malicious: %d\n", r, customerArray[r].isMalicious);
		int sp;
		for(sp = 0; sp < N_SP; sp++)
		{
			printf("\t %d", customerArray[r].visitsPerSP[sp]);
		}
	} */
}

void myReport(struct ServiceProvider SPArray[N_SP])
{
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t In Queue\t Busy Period\t Current Wait Time\n");
    int i;
    for(i = 0; i < N_SP; i++)
    {
        printf("SP%d \t %d \t %d \t %d \t %f \t %f \t %f \t %f\n", i, SPArray[i].id, SPArray[i].numberOfSRVisitors, SPArray[i].isMalicious, U(SPArray[i].id), Lq(SPArray[i].id), B(SPArray[i].id), SPArray[i].actualWaitTime);
    }
    report();
}

real getSPAdvertisedWaitTime(struct ServiceProvider* SP)
{
	//return SP->actualWaitTime;
	real multiplier;

	if(SP->isMalicious == 1)
	{
		//first get current queue length
		int currentQueueLength = inq(SP->id);
		if(currentQueueLength > 20)
		{
			multiplier = 1.0;
		}
		else
		{
			multiplier = 0.75;
		}	
	}
	else
	{
		multiplier = 1.0;
	}
	
	return (multiplier * SP->actualWaitTime);
}

struct ServiceProvider getLeastBusySP(struct ServiceProvider SPs[N_SP])
{
	int s;
    struct ServiceProvider selectedSP = SPs[0];
    real leastAvailabilityTime = getSPAdvertisedWaitTime(&SPs[0]);//current customers in queue
    int selectedSPVisitors = SPs[0].numberOfSRVisitors;

    for(s = 1; s < N_SP; s++)
    {
        real currentSPAvailabilityTime = getSPAdvertisedWaitTime(&SPs[s]); 
        
        if(currentSPAvailabilityTime < leastAvailabilityTime || (currentSPAvailabilityTime == leastAvailabilityTime && selectedSPVisitors > SPs[s].numberOfSRVisitors))
        {
            selectedSP = SPs[s];
            leastAvailabilityTime = currentSPAvailabilityTime;
            selectedSPVisitors = SPs[s].numberOfSRVisitors;
        }
    } 

    return selectedSP;
}

void addNewSRToQueue(struct ServiceProvider *SP, struct ServiceRequester SR)
{
	//first add SR to the bottom of currentSRInQueue
	/*
	int srQIndx, srQLength;
	srQLength = sizeof(SP->currentSRInQueue)/sizeof(int);
	for(srQIndx = 0; srQIndx < srQLength; srQIndx++)
	{
		if(SP->currentSRInQueue[srQIndx] == NON_EXISTENT)
		{
			SP->currentSRInQueue[srQIndx] = SR.id;	
			if(srQIndx > 55)
			{
				//printf("SPID: %d, SRID: %d, \n\tSP Queue %d value: %d\n", SP->id, SR.id, srQIndx, SP->currentSRInQueue[srQIndx]);
			}
			break;
		}
	}
	*/
	//capture current time
	real currentTime = time();

	//next find the index of the slot with the lowest time
	int timeSlotLength = sizeof((*SP).availabilitySlotList)/sizeof(real);
	int leastTimeSlotIndex = 0; 
	real leastTimeSlot = (*SP).availabilitySlotList[0]; 
	int sl;
	for(sl = 1; sl < timeSlotLength; sl++)
	{
		/*if(((*SP).availabilitySlotList[sl] == NON_EXISTENT || (*SP).availabilitySlotList[sl] < currentTime) && (*SP).availabilitySlotList[sl] < leastTimeSlot)
		{
			leastTimeSlotIndex = sl;	
			if((*SP).availabilitySlotList[sl] == NON_EXISTENT || (*SP).availabilitySlotList[sl] < currentTime)
			{
				leastTimeSlot = 0.0;	
			}
		}
		*/ 
		if(SP->availabilitySlotList[sl] < leastTimeSlot)
		{
			leastTimeSlotIndex = sl;
			leastTimeSlot = SP->availabilitySlotList[sl];	
		}
	}

	//next update the new time for the selected slot such that the time in the slot will represent when the slot will be available next
	if(SP->availabilitySlotList[leastTimeSlotIndex] == NON_EXISTENT || SP->availabilitySlotList[sl] < currentTime)
	{
		SP->availabilitySlotList[leastTimeSlotIndex] = currentTime + SR.currentServiceTime;
	}
	else
	{
		SP->availabilitySlotList[leastTimeSlotIndex] = SP->availabilitySlotList[leastTimeSlotIndex] + SR.currentServiceTime;
	}

	//finally update the waitTime of the SP by finding the least wait time in the availability slot list
	int leastWaitTime;
	leastWaitTime = SP->availabilitySlotList[0];
	for(sl = 1; sl < timeSlotLength; sl++)
	{
		if((*SP).availabilitySlotList[sl] < leastWaitTime)
		{
			leastWaitTime = SP->availabilitySlotList[sl];
		}
	}
	
	if(leastWaitTime == NON_EXISTENT || leastWaitTime < currentTime)
	{
		leastWaitTime = currentTime;
	}

	SP->actualWaitTime = leastWaitTime; //This is the time when the next slot will be available
}

/*void removeSRFromQueue(struct ServiceProvider *SP, struct ServiceRequester SR)
{
	//An SR is removed from the queue list when its request for SP is successful
	//The SR to removed from the queue should the first in the queue but if it is not then log the error
	if(SP->currentSRInQueue[0] != SR.id)
	{
		//printf("The first element in the queue is not the SR that just got server request. First SR in queue: %d, SR to be removed: %d, SP Id: %d\n", (*SP).currentSRInQueue[0], SR.id, (*SP).id);
		int qIndx, qLength;
		qLength = sizeof(SP->currentSRInQueue)/sizeof(int);
		for(qIndx = 0; qIndx < qLength; qIndx++)
		{
			//printf("\tQueue index: %d, value: %d\n", qIndx, SP->currentSRInQueue[qIndx]);
			if((*SP).currentSRInQueue[qIndx] == SR.id)
			{
				printf("\tThe id was found at index %d\n", qIndx);
				int i;
				for(i = qIndx; i < qLength; i++)
				{
					if((*SP).currentSRInQueue[i] == NON_EXISTENT)
					{
						break;
					}
					SP->currentSRInQueue[i] = SP->currentSRInQueue[i+1];
				}
				break;
			}
		}
	}
	else if(SP->currentSRInQueue[0] == SR.id)
	{
		//move all the valid content of the queue one index up
		int qIndx, qLength;
		qLength = sizeof(SP->currentSRInQueue)/sizeof(int);
		for(qIndx = 0; qIndx < qLength; qIndx++)
		{
			SP->currentSRInQueue[qIndx] = SP->currentSRInQueue[qIndx + 1];
			if(SP->currentSRInQueue[qIndx + 1] == NON_EXISTENT)
			{
				break;	
			}
		}
	}
}
*/
