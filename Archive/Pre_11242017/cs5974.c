#include <math.h>
#include "smpl.h"
#include "cs5974.h"

#define	currentReportNumber	1

struct ServiceProvider providerArray[N_SP];
struct ServiceRequester customerArray[N_SR];
int currentCustomer;

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
	int event, customer;
	real highestCentrality, report1LastPrinted;
	highestCentrality = 0.0;
	report1LastPrinted = -5.0;

	real te = 100.00; //this is end time for simulation
	smpl(0,"5974"); //initialize smpl simulation
	//trace(2);
	setupServiceProviders();
	setupServiceRequesters();
	setupMaliciousSPSR();

	//initial report setup
	switch(currentReportNumber)
		{
			case 1:
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
		}

	while(time() < te)
	{
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
			{if(customer == 1963){currentCustomer = customer;}
				real currentTime = time();
				struct ServiceProvider* selectedSP = getLeastBusySP(currentTime);
				setSelectedServiceProvider(&customerArray[customer], selectedSP, currentTime);
				
			}
			if (request(customerArray[customer].currentSP->id,customer,0)==0)
			{
				//capture the clock time when this customer received the service
				customerArray[customer].currentSPServiceStartTime = time();
				
				//update the feedback to SP and SR based on the wait time
				updateServiceExperience(&customerArray[customer], &highestCentrality);

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

	myReport(highestCentrality, 1);

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
		customerArray[j].positiveReviews = 1.0;
		customerArray[j].negativeReviews = 1.0;
		customerArray[j].totalReviews = 0;
		customerArray[j].honesty = 0.5;//initially honesty is 50%
		customerArray[j].credibility = 0.0;
		
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
			customerArray[j].serviceExperiences[spIndx].lastServiceStartTime = -1.0;
			customerArray[j].serviceExperiences[spIndx].lastServiceRating = -1.0;
			customerArray[j].serviceExperiences[spIndx].lastServiceActualWaitTime = -1.0;
			customerArray[j].serviceExperiences[spIndx].sProvider = &providerArray[spIndx];
		}
		//initialize service provider SRFeedback
		for(spIndx = 0; spIndx < N_SP; spIndx++)
		{
			providerArray[spIndx].feedbacks[j].sRequester = &customerArray[j];
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
	getAllWitnessOrderedByHonesty(selectedSP, serRequester->witnesses, &witnessCount);
	addNewSRToQueueAndUpdateAvailabilitySlot(selectedSP, serRequester);
	/*
	int wtIndx, spIndx;
	for(wtIndx = 0; wtIndx < N_SR; wtIndx++)
	{
		if(customerArray[wtIndx].id != serRequester->id)
		{
			for(spIndx = 0; spIndx < N_SP; spIndx++)
			{
				if(customerArray[wtIndx].serviceExperiences[spIndx].sProvider->id == selectedSP->id && customerArray[wtIndx].serviceExperiences[spIndx].lastServiceRating > -1.0)
				{
					//if the lastService rating is greater than -1 then it means the SR has service experience with the SP
					int w;
					for(w = 0; w < N_SR; w++)
					{
						//find a witness slot that is not null and place the witness
						if(serRequester->witnesses[w]->sRequester == NULL)
						{
							serRequester->witnesses[w]->sRequester = &customerArray[wtIndx];
							serRequester->witnesses[w]->waitTimeRating = customerArray[wtIndx].serviceExperiences[spIndx].lastServiceRating;
							serRequester->witnesses[w]->witnessWaitTime = customerArray[wtIndx].serviceExperiences[spIndx].lastServiceActualWaitTime;
							break;
						}
					}
					break;
				}
			}
		}
	}
	*/
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

real getProjectedWaitTime(struct ServiceProvider* SP, real advertisedWaitTime)
{
	int witnessCount;
	struct Witness witnessArray[N_SR];
	/*for(int i = 0; i < N_SR; i++)
	{
		witnessArray[i].sRequester = NULL;
	}*/
	getAllWitnessOrderedByHonesty(SP, witnessArray, &witnessCount);
	/*
	//set the array to null first
	for(int wIndx = 0; wIndx < N_SR; wIndx++)
	{
		witnessArray[wIndx] = NULL;
	}

	//iterate through each service requester and find out all the ones
	//that have prior experience with the SP
	for(int wIndx = 0; wIndx < N_SR; wIndx++)
	{
		//iterate the service experience of each SR to check if it has prior experience
		for(int sIndx = 0; sIndx < N_SP; sIndx++)
		{
			if(customerArray[wIndx].serviceExperiences[sIndx].sProvider->id == SP->id)
			{
				struct ServiceRequester* currentWT = &customerArray[wIndx];
				if(currentWT->serviceExperiences[sIndx].lastServiceRating > -1.0)
				{
					//if the customer has experience with the SP then increment the counter
					witnessCount++;
					for(int w = 0; w < N_SR; w++)
					{
						if(witnessArray[w] == NULL)
						{
							witnessArray[w] = currentWT;
							break;
						}
						else if(witnessArray[w]->credibility < currentWT->credibility)
						{
							struct ServiceRequester* tempSR = witnessArray[w];
							witnessArray[w] = currentWT;
							currentWT = tempSR;
						}
					}
				}
				break;
			}
		}
	}
	*/
	//after determining all witnesses, use the wait time of the top N_Rec witnesses to project the SP wait time
	real netCredibility, projWaitTimeByWitnesses, rho, projectedWaitTime;
	netCredibility = 0.0;
	projWaitTimeByWitnesses = 0.0;
	rho = 0.0;
	// 1. Calculate sum of credibility of top N_Rec witnesses
	for(int rIndx = 0; rIndx < N_Rec; rIndx++)
	{
		if(witnessArray[rIndx].sRequester != NULL)
		{
			netCredibility += witnessArray[rIndx].sRequester->credibility;
		}
	}
	//2. Calculate Equation 7: Projecte wait time based on witnesses
	if(netCredibility > 0.0)
	{
		for(int rIndx = 0; rIndx < N_Rec; rIndx++)
		{
			if(witnessArray[rIndx].sRequester != NULL)
			{
				projWaitTimeByWitnesses += (witnessArray[rIndx].sRequester->credibility / netCredibility) * witnessArray[rIndx].witnessWaitTime;
			}
		}
	}
	//3. Equation 9: Determine weight for witness projeted wait time (this is normalized)
	if(netCredibility > 0.0)
	{
		for(int rIndx = 0; rIndx < N_Rec; rIndx++)
		{
			if(witnessArray[rIndx].sRequester != NULL)
			{
				rho += (witnessArray[rIndx].sRequester->credibility / netCredibility) * witnessArray[rIndx].sRequester->honesty;
			}
		}
	}

	//Rho can be as high as 0.5 so multiply it by 0.5
	rho = 0.5 * rho;

	//4. Calculate Equation 8 to determine the projected wait time basd on witnesses and addvertised wait time
	projectedWaitTime = (((1 - rho) * (advertisedWaitTime/SP->trustScore)) + (rho * projWaitTimeByWitnesses));

	return projectedWaitTime;
}

struct ServiceProvider* getLeastBusySP(real currentTime)
{
	int s;
    struct ServiceProvider* selectedSP = &providerArray[0];
    real leastProjectedWaitTime = getProjectedWaitTime(&providerArray[0], getSPAdvertisedWaitTime(&providerArray[0], currentTime));
    for(s = 1; s < N_SP; s++)
    {
        real currentSPLeastProjectedWaitTime = getProjectedWaitTime(&providerArray[s], getSPAdvertisedWaitTime(&providerArray[s], currentTime)); 
        
        if(currentSPLeastProjectedWaitTime < leastProjectedWaitTime)
        {
            selectedSP = &providerArray[s];
            leastProjectedWaitTime = currentSPLeastProjectedWaitTime;
        }
    }
    return selectedSP;
}

void getAllWitnessOrderedByHonesty(struct ServiceProvider* servProvider, struct Witness* witnessArray, int* witnessCount)
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
		//if(currentCustomer == 1963){printf("wIndx: %d\n", wIndx);}
		//iterate the service experience of each SR to check if it has prior experience
		for(int sIndx = 0; sIndx < N_SP; sIndx++)
		{
			if(customerArray[wIndx].serviceExperiences[sIndx].sProvider->id == servProvider->id)
			{
				struct ServiceRequester* currentSR = &customerArray[wIndx];
				if(currentSR->serviceExperiences[sIndx].lastServiceRating > -1.0)
				{
					//if the customer has experience with the SP then increment the counter
					//*witnessCount++;
					for(int w = 0; w < N_SR; w++)
					{//if(currentCustomer == 1963 && wIndx == 1922 && w > 1500){printf("wIndx: %d, w: %d, witness sRequester: %d\n", wIndx, w, witnessArray[w].sRequester->id);}
						if(witnessArray[w].sRequester == NULL)
						{
							witnessArray[w].sRequester = currentSR;
							witnessArray[w].waitTimeRating = currentSR->serviceExperiences[sIndx].lastServiceRating;
							witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[sIndx].lastServiceActualWaitTime;
							//if(currentCustomer == 1963){printf("I am here. currentSR: %d,  credibility: %.2f, witness: %d, witness sReq: %d\n", currentSR->id, currentSR->credibility, &witnessArray[w], witnessArray[w].sRequester->id);}
							break;
						}
						else if(witnessArray[w].sRequester->credibility < currentSR->credibility)
						{//printf("I am in else if.\n");
							struct ServiceRequester* tempSR = witnessArray[w].sRequester;
							witnessArray[w].sRequester = currentSR;
							witnessArray[w].waitTimeRating = currentSR->serviceExperiences[sIndx].lastServiceRating;
							witnessArray[w].witnessWaitTime = currentSR->serviceExperiences[sIndx].lastServiceActualWaitTime;
							currentSR = tempSR;
						}
					}
				}
				break;
			}
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
void updateServiceExperience(struct ServiceRequester* servRequeter, real* highestCentrality)
{
	struct ServiceProvider* SP;
	SP = servRequeter->currentSP;

	//find the service experience to update;
	struct ServiceExperience *svcExp;
	for(int svcIndx = 0; svcIndx < N_SP; svcIndx++)
	{
		if(servRequeter->serviceExperiences[svcIndx].sProvider->id == SP->id)
		{
			svcExp = &servRequeter->serviceExperiences[svcIndx];
			break;
		}
	}

	svcExp->lastServiceStartTime = servRequeter->currentSPServiceStartTime;

	//First determine the actual wait time and set the wait time rating
	if(servRequeter->isMalicious == 0)
	{
		//determine the acutal wait time by subtracting the clock time when the SR was in queue from when SR received service
		real currentSRActualWaitTime = servRequeter->currentSPServiceStartTime - servRequeter->currentSPQueueStartTime;

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
	}
	else
	{
		if(SP->isMalicious == 1)
		{
			svcExp->lastServiceRating = 5.0;
			svcExp->lastServiceActualWaitTime = servRequeter->currentSPAdvertisedWaitTime; //A malicious SR will concurr with whatever the malicous SP is advertising
		}
		else
		{
			svcExp->lastServiceRating = 1.0;
			svcExp->lastServiceActualWaitTime = (servRequeter->currentSPAdvertisedWaitTime * (1 + Param_R)); //A malicious SR will increase the wait time of a non-malicious SP by factor R
		}
	}

	//determine the normalized rating towards the SP for this transaction
	real f, newAlpha, newBeta;
	f = (svcExp->lastServiceRating) / 5.0;

	SP->feedbacks[servRequeter->id].positiveFeedback = SP->feedbacks[servRequeter->id].positiveFeedback + f;
	SP->feedbacks[servRequeter->id].negativeFeedback = SP->feedbacks[servRequeter->id].negativeFeedback + 1.0 - f;

	//calcualte the trust score towards the SP from this SR
	SP->feedbacks[servRequeter->id].trustScore = SP->feedbacks[servRequeter->id].positiveFeedback / (SP->feedbacks[servRequeter->id].positiveFeedback + SP->feedbacks[servRequeter->id].negativeFeedback);
	
	//now provide feedback for each witness. Witness is an SR that had service experience from the SP in past
	for(int fdIndx = 0; fdIndx < N_SR; fdIndx++)
	{
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
				int waitTimeRatingDiffWithWitness = fabs(svcExp->lastServiceRating - servRequeter->witnesses[fdIndx].waitTimeRating);

				//determine the new rating for the witness
				witnessRatingNew = 5.0 - waitTimeRatingDiffWithWitness;
			}

			normRating = (witnessRatingNew / 5.0);

			servRequeter->witnesses[fdIndx].sRequester->positiveReviews = servRequeter->witnesses[fdIndx].sRequester->positiveReviews + normRating;
			servRequeter->witnesses[fdIndx].sRequester->negativeReviews = servRequeter->witnesses[fdIndx].sRequester->negativeReviews + 1 - normRating;

			servRequeter->witnesses[fdIndx].sRequester->totalReviews++;

			if(*highestCentrality == 0.00 || (*highestCentrality < servRequeter->witnesses[fdIndx].sRequester->totalReviews))
			{
				*highestCentrality = servRequeter->witnesses[fdIndx].sRequester->totalReviews;
			}

			servRequeter->witnesses[fdIndx].sRequester->honesty = servRequeter->witnesses[fdIndx].sRequester->positiveReviews / (servRequeter->witnesses[fdIndx].sRequester->positiveReviews + servRequeter->witnesses[fdIndx].sRequester->negativeReviews);
			servRequeter->witnesses[fdIndx].sRequester->credibility = servRequeter->witnesses[fdIndx].sRequester->honesty * (servRequeter->witnesses[fdIndx].sRequester->totalReviews / *highestCentrality);
		}
	}

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
    	projectedWaitTime = getProjectedWaitTime(&providerArray[i], advertisedWT);
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

void printBasicCustomerInfo(real highestCentrality)
{
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
    	real centrality = customerArray[cIndx].positiveReviews + customerArray[cIndx].negativeReviews;

    	if(customerArray[cIndx].isMalicious == 1)
    	{
    		malSRCount++;
    		totalMalHonesty += customerArray[cIndx].honesty;
    		totalMaliciousCredibility += customerArray[cIndx].credibility;
    	}
    	else
    	{
    		nonMalSRCount++;
    		totalNonMalHonesty += customerArray[cIndx].honesty;
    		totalNonMaliciousCredibility += customerArray[cIndx].credibility;
    	}
    	
    	printf("%d\t %.2f\t %.2f\t %.2f\t %.2f\t\t %.2f\t %.2f\t\t %d\n", cIndx, customerArray[cIndx].positiveReviews, customerArray[cIndx].negativeReviews, centrality, (centrality / highestCentrality), customerArray[cIndx].honesty, customerArray[cIndx].credibility, customerArray[cIndx].isMalicious);
    	
    }
    printf("Average Malicious SR Honesty: %.2f\nAverage Non-Malicious SR Honesty: %.2f\n", (totalMalHonesty/malSRCount), (totalNonMalHonesty/nonMalSRCount));
    printf("Average Malicious SR Credibility: %.2f\nAverage Non-Malicious SR Credibility: %.2f\n", (totalMaliciousCredibility/malSRCount), (totalNonMaliciousCredibility/nonMalSRCount));
    printf("Highest Centrality: %.2f\n", highestCentrality);
}

void testReport()
{
	int witnessCount;
	struct Witness witnessArray[N_SR];
	getAllWitnessOrderedByHonesty(&providerArray[1], witnessArray, &witnessCount);

	for(int i = 0; i < N_SR; i++)
	{
		if(witnessArray[i].sRequester != NULL)
		{
			printf("Witness: %d, sRequeter: %d, wait time rating: %d, witnessWaitTime: %.2f, total reviews: %d, credibility: %.2f, witness honesty: %.2f\n", i, witnessArray[i].sRequester->id, witnessArray[i].waitTimeRating, witnessArray[i].witnessWaitTime, witnessArray[i].sRequester->totalReviews, witnessArray[i].sRequester->credibility, witnessArray[i].sRequester->honesty);
		}
	}

	real netCredibility, projWaitTimeByWitnesses, rho, projectedWaitTime;
	netCredibility = 0.0;
	projWaitTimeByWitnesses = 0.0;
	rho = 0.0;
	// 1. Calculate sum of credibility of top N_Rec witnesses
	for(int rIndx = 0; rIndx < N_Rec; rIndx++)
	{
		if(witnessArray[rIndx].sRequester != NULL)
		{
			netCredibility += witnessArray[rIndx].sRequester->credibility;
		}
	}
	//2. Calculate Equation 7: Projecte wait time based on witnesses
	if(netCredibility > 0.0)
	{
		for(int rIndx = 0; rIndx < N_Rec; rIndx++)
		{
			if(witnessArray[rIndx].sRequester != NULL)
			{
				projWaitTimeByWitnesses += (witnessArray[rIndx].sRequester->credibility / netCredibility) * witnessArray[rIndx].witnessWaitTime;
			}
		}
	}
	//3. Equation 9: Determine weight for witness projeted wait time (this is normalized)
	if(netCredibility > 0.0)
	{
		for(int rIndx = 0; rIndx < N_Rec; rIndx++)
		{
			if(witnessArray[rIndx].sRequester != NULL)
			{
				rho += (witnessArray[rIndx].sRequester->credibility / netCredibility) * witnessArray[rIndx].sRequester->honesty;
			}
		}
	}

	//Rho can be as high as 0.5 so multiply it by 0.5
	rho = 0.5 * rho;

	printf("Witness projected wait time: %.2f, netCredibility: %.2f, rho: %.2f\n", projWaitTimeByWitnesses, netCredibility, rho);
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
	    	int fbIndx, fbSRCount;
	    	real posFB, negFB, trustScore, advertisedWT, actualWT, projectedWaitTime;
	    	advertisedWT = getSPAdvertisedWaitTime(&providerArray[i], currentTime);
	    	projectedWaitTime = getProjectedWaitTime(&providerArray[i], advertisedWT);
	    	actualWT = providerArray[i].nextAvailTime - currentTime;
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
	        printf("SP%d\t%d\t%d\t%d\t%.2f\t%.2f\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%.2f\n", i, providerArray[i].id, providerArray[i].totalVisitorCount, providerArray[i].isMalicious, U(providerArray[i].id), Lq(providerArray[i].id), inq(providerArray[i].id), B(providerArray[i].id), providerArray[i].nextAvailTime, providerArray[i].trustScore, advertisedWT, actualWT, projectedWaitTime, totalVisitors, currentTime);
	        /*for(int as = 0; as < M_SP; as++)
	        {
	        	printf("\t\tAvail Slot: %d, Value: %.2f\n", as, providerArray[i].availabilitySlotList[as]);
	        }*/
	    }
	}
}

void myReport(real highestCentrality, int isFinalReport)
{
	printf("\nSimulation Report\n");
	printBasicFacilityInfo();
	printBasicCustomerInfo(highestCentrality);
	//testReport();
    
}