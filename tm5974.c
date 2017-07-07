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
void updatedFeedback();

#define NON_EXISTENT -1

struct ServiceProvider
{
	int id;
	int numberOfSRVisitors;
	real trustScore;
	real nextAvailTimeSlot;//simulation clock time when the SP is available to service a new customer
	int isMalicious;
	int currentSRInService[M_SP];//SRs being serviced by the SP
	//int currentSRInQueue[M_SP];
	real availabilitySlotList[M_SP];//this is used to calculate the next available time
};

struct ServiceRequester
{
	int id;
	real startTime;//simulation clock time when the customer was first added to the system
	int currentSPId;
	real currentServiceTime; //the exponential service time length this SR will use the SP for
	real currentSPServiceStartTime; //simulation clock time when the SR received service from SR
	real currentSPQueueStartTime; //simulation clock time when the SR was selected to request a server
	real currentSPAdvertisedWaitTime; //the length of time advertised by SP
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
        serviceProviderArray[i].nextAvailTimeSlot = 0.0;
        serviceProviderArray[i].isMalicious = 0;
              
        
        if(i == 3 || i == 6)
        {
            serviceProviderArray[i].isMalicious = 1;
        }
        
        
		int sr, avlSL;
		//initialize availabilitySlotList
		avlSL = sizeof(serviceProviderArray[i].availabilitySlotList)/sizeof(real);
		for(sr = 0; sr < avlSL; sr++)
		{
			serviceProviderArray[i].availabilitySlotList[sr] = 0.0;	
		}
	}

	for(j = 0; j < N_SR; j++)
	{
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
				real currentTime = time();
				struct ServiceProvider selectedSP = getLeastBusySP(serviceProviderArray, currentTime);  
				customerArray[customer].currentSPId = selectedSP.id;
				//capture the clock time when this customer was assigned a SP
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
				//capture the clock time when this customer received the service
				customerArray[customer].currentSPServiceStartTime = time();
				//printf("customer %d obtains service from SP with fd=%d; wait time is %f\n", customer, sp_id[customer], time()-ts[customer]);
				
				//TODO: updateFeedback()

				schedule(3,customerArray[customer].currentServiceTime,customer);
			}
			break;
			case 3:  /* release server */
			release(customerArray[customer].currentSPId,customer);
			//printf("departure time of customer %d from SP with fd=%d is %f; total response time is %f\n", customer, sp_id[customer], time(), time()-ts[customer]);
			
			//TODO: remove the current SR from SP.currentSRInService array

			customerArray[customer].currentSPId = NON_EXISTENT; // reset so that next time we know this customer will arrive anew
			customerArray[customer].currentSPServiceStartTime = 0.0; 
			customerArray[customer].currentSPQueueStartTime = 0.0;
			customerArray[customer].currentSPAdvertisedWaitTime = 0.0;
			
			schedule(1,expntl(Ta), customer); /* schedule next arrival time */
			break;
		}
	}
	myReport(serviceProviderArray);
}

void myReport(struct ServiceProvider SPArray[N_SP])
{
	printf("\nSimulation Report\n");
    printf("#  \t F_ID \t Visitors\t Mal\t In Service\t In Queue\t Busy Period\t Current Wait Time\n");
    int i;
    for(i = 0; i < N_SP; i++)
    {
        printf("SP%d \t %d \t %d     \t %d \t %f \t %f \t %f \t %f\n", i, SPArray[i].id, SPArray[i].numberOfSRVisitors, SPArray[i].isMalicious, U(SPArray[i].id), Lq(SPArray[i].id), B(SPArray[i].id), SPArray[i].nextAvailTimeSlot);
    }
    //report();
}

real getSPAdvertisedWaitTime(struct ServiceProvider* SP, real currentTime)
{
	real multiplier;

	if(SP->isMalicious == 1)
	{	
		multiplier = 0.75;
	}
	else
	{
		multiplier = 1.0;
	}
	real advertisedWaitTime;
	advertisedWaitTime = multiplier * (SP->nextAvailTimeSlot - currentTime);
	if(advertisedWaitTime < 0)
	{
		advertisedWaitTime = 0.0;
	}
	return advertisedWaitTime;
}

struct ServiceProvider getLeastBusySP(struct ServiceProvider SPs[N_SP], real currentTime)
{
	int s;
    struct ServiceProvider selectedSP = SPs[0];
    real leastWaitTime = getSPAdvertisedWaitTime(&SPs[0], currentTime);//current customers in queue
    //real leastWaitTime = leastAvailabilityTime - currentTime;
    
    int selectedSPVisitors = SPs[0].numberOfSRVisitors;

    for(s = 1; s < N_SP; s++)
    {
        real currentSPLeastWaitTime = getSPAdvertisedWaitTime(&SPs[s], currentTime); 
        //real currentSPLeastWaitTime = currentSPAvailabilityTime - currentTime;
        if(currentSPLeastWaitTime < leastWaitTime)// || (currentSPLeastWaitTime == leastWaitTime && selectedSPVisitors > SPs[s].numberOfSRVisitors))
        {
            selectedSP = SPs[s];
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