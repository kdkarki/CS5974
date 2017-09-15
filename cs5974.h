// Table 1 parameters
#define N_SP	10	//total number of service providers
#define N_SR	2200	//total number of service requesters
#define M_SP	20 	//total number of SR to processed in parallel by an SP
#define Ta	10.0	//service requester arrival time
#define Ts	1.0		//service provider service time
//#define T_Margin	0.20 //threshold for wait time to be considered inaccurate
#define Param_R	1.0 //the wait time offset reported by a malicious SP
#define N_Rec	5 //the number of top recommeders, SR, used to calculate SP trust score
#define NON_EXISTENT -1

struct ServiceExperience //Represents service experienced by an SR from an SP
{
	real lastServiceStartTime; //the simulation clock time where the service was received from the SP
	real lastServiceRating; //the rating provided to the SP based on the advertised time and the actual wait time experienced by the SR
	real lastServiceActualWaitTime; //trust score from each SR towards an SP.
	struct ServiceProvider* sProvider;
};

struct Witness //Represents a witness, an SR that has received service from an SP
{
	int waitTimeRating;	//wait time rating given by the SR towards SP
	real witnessWaitTime; // wait time witnessed by the SR
	struct ServiceRequester* sRequester; //the witnessing SR
};

struct SRFeedback //Represents feedback from an SR towards an SP. Each SR will have +ve or -ve rating towards an SP for each transaction
{
	real positiveFeedback;
	real negativeFeedback;
	real trustScore; //trust score from each SR towards an SP.
	struct ServiceRequester* sRequester;
};

struct ServiceProvider //Represents a service provider
{
	int id;
	real trustScore;
	int totalVisitorCount;	
	int isMalicious;
	real nextAvailTime;//simulation clock time when the SP is available to service a new customer
	real availabilitySlotList[M_SP];//this is used to calculate the next available time
	struct SRFeedback feedbacks[N_SR]; //an array of feedback where each feedback is from a uniqure SR
};

struct ServiceRequester //Represents a service requester (customer)
{
	int id;
	int isMalicious;
	int totalReviews; //total number of reviews (+ and -) received from other SRs
	real positiveReviews; //positive reviews provided by other SRs
	real negativeReviews; //negative reviews provided by other SRs
	real honesty; //honesty of the SR based on reviews provided by other SRs
	real credibility; //credibility of the SR based on the centrality and honesty
	real currentServiceTime; //the exponential service time length this SR will use the SP for
	real currentSPServiceStartTime; //simulation clock time when the SR received service from SR
	real currentSPQueueStartTime; //simulation clock time when the SR was selected to request a server. Difference between Service Start Time and Queue Start Time is the actual wait time
	real currentSPAdvertisedWaitTime; //the length of time to wait before getting service as advertised by selected SP
	//int currentSPWaitTimeRating; //The service wait time rating, a number from 1 to 5, with 1 being least satisfaction and 5 being most satisfaction.
	//real currentSPWitnessWaitTime; //The wait time this SR will advertise to other SR as witness when getting service from SP
	struct ServiceProvider* currentSP;
	struct Witness witnesses[N_SR]; //an array of witnesses that have had previous service experience with selected SP
	struct ServiceExperience serviceExperiences[N_SP]; //service experience recorded after last experience with a service provider
};

//methods
void myReport();
void testReport();
void figure1Report();
void setupServiceProviders();
void setupServiceRequesters();
void setupMaliciousSPSR();
void setSelectedServiceProvider();
real getSPAdvertisedWaitTime();
real getProjectedWaitTime();
void getAllWitnessOrderedByHonesty(struct ServiceProvider* servProvider, struct Witness* witnessArray, int* witnessCount);
real rateSPExperience(real advertisedWaitTime, real actualWaitTime, struct ServiceProvider* servProvider, struct ServiceRequester* sRequester);
void highestCentrality(struct ServiceProvider* servProvider, struct ServiceRequester* servRequester, real newRating);
void updateServiceExperience(struct ServiceRequester* servRequeters, real* highestCentrality);
void addNewSRToQueueAndUpdateAvailabilitySlot();
struct ServiceProvider* getLeastBusySP();
void cleanupServiceRequesterBeforeLeaving();