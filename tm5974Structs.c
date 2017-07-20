#include "tm5974.h"

struct Witness //Represents a witness, an SR currently in service from an SP and advertising its experienced wait time rating
{
	int waitTimeRating;
	struct ServiceRequester* sRequester;
};

struct ServiceRequester //Represents a service requester (customer)
{
	int id;
	real startTime;//simulation clock time when the customer was first added to the system
	real currentServiceTime; //the exponential service time length this SR will use the SP for
	real currentSPServiceStartTime; //simulation clock time when the SR received service from SR
	real currentSPQueueStartTime; //simulation clock time when the SR was selected to request a server
	real currentSPAdvertisedWaitTime; //the length of time to wait before getting service as advertised by selected SP
	//real currentSPExperiencedWaitTime; //the length of wait time experienced by SR before getting service
	int currentSPWaitTimeRating; //The service wait time rating, a number from 1 to 5, with 1 being least satisfaction and 5 being most satisfaction.
	int visitsPerSP[N_SP];
	int isMalicious;
	real positiveFeedback;
	real negativeFeedback;
	struct ServiceProvider* currentSP;
	struct Witness witnesses[M_SP]; //an array of witnesses that were present at the SP when the SP was selected for the service
};

struct SRFeedback //Represents feedback from an SR towards an SP. Each SR will have +ve or -ve rating towards an SP for each transaction
{
	real positiveFeedback;
	real negativeFeedback;
	struct ServiceRequester* sRequester;
};

struct ServiceProvider //Represents a service provider
{
	int id;
	int numberOfSRVisitors;
	real trustScore;
	real nextAvailTimeSlot;//simulation clock time when the SP is available to service a new customer
	int isMalicious;	
	//int currentSRInQueue[M_SP];
	real availabilitySlotList[M_SP];//this is used to calculate the next available time
	struct ServiceRequester* currentSRInService[M_SP];//SRs being serviced by the SP
	struct SRFeedback feedbacks[N_SR]; //an array of feedback where each feedback is from a uniqure SR
};