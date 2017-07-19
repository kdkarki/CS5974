#include "tm5974.h"

struct Witness
{
	real waitTimeLength;
	struct ServiceRequester* sRequester;
};

struct ServiceRequester
{
	int id;
	real startTime;//simulation clock time when the customer was first added to the system
	real currentServiceTime; //the exponential service time length this SR will use the SP for
	real currentSPServiceStartTime; //simulation clock time when the SR received service from SR
	real currentSPQueueStartTime; //simulation clock time when the SR was selected to request a server
	real currentSPAdvertisedWaitTime; //the length of time to wait before getting service as advertised by selected SP
	real currentSPExperiencedWaitTime; //the length of wait time experienced by SR before getting service
	int visitsPerSP[N_SP];
	int isMalicious;
	int positiveFeedback;
	int negativeFeedback;
	struct ServiceProvider* currentSP;
	struct Witness witnesses[M_SP];
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