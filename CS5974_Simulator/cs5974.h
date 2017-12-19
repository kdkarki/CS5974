// Table 1 parameters
#define N_SP	10	//total number of service providers
#define N_SR	2200	//total number of service requesters
#define M_SP	20 	//total number of SR to processed in parallel by an SP
#define Ta	10.0	//service requester arrival time
#define Ts	1.0		//service provider service time
//#define T_Margin	0.20 //threshold for wait time to be considered inaccurate
//#define Param_R 1.0 //the wait time offset reported by a malicious SP
#define N_Rec	5 //the number of top recommenders, SR, used to calculate SP trust score
#define H_Thres 0.6 //honesty threshold
#define LastExpTime_Thres   0.0835
#define NON_EXISTENT -1
#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])))

struct ServiceExperience //Represents service experienced by an SR from an SP
{
    int totalVisits;
    real lastServiceStartTime; //the simulation clock time where the service was received from the SP
    real lastServiceRating; //the rating provided to the SP based on the advertised time and the actual wait time experienced by the SR
    real lastServiceActualWaitTime; //the actual wait time experienced by SR during the last visit to the SP. This value will be used by SR to calculate projected wait time
    real lastServiceAdvertisedWaitTime; //the wait time advertised by the SR as the last visit wait time at the SP. This will be same as lasServiceActualWaitTime for non-malicious SP but different for malicious SP
    struct ServiceProvider* sProvider;
};

struct ServiceExperienceHistory
{
    int historyCount;
    int serviceProviderFID;
    struct ServiceExperience servExp[5];
};

struct Witness //Represents a witness, an SR that has received service from an SP
{
    real waitTimeRating;	//wait time rating given by the SR towards SP
    real witnessWaitTime; // wait time witnessed by the SR
    struct ServiceRequester* sRequester; //the witnessing SR
};

struct SRFeedback //Represents feedback from an SR towards an SP or another SR. Each SR will have +ve or -ve rating towards an SP/SR for each transaction
{
    int totalFeedback; //total reviews (sum of positiveFeedback and negativeFeedback) provided by sRequester
    real positiveFeedback;
    real negativeFeedback;
    real trustScore; //trust score from each SR towards an SP. Also, honesty score from this SR towards another SR
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
    struct SRFeedback feedbacks[N_SR]; //an array of feedback where each feedback is from a unique SR
};

struct ServiceRequester //Represents a service requester (customer)
{
    int id;
    int isMalicious;
    int totalReviews; //total number of reviews (+ and -) received from all other SRs
    //real positiveReviews; //positive reviews provided by other SRs
    //real negativeReviews; //negative reviews provided by other SRs
    real honesty; //honesty of the SR based on reviews provided by other SRs
    //real credibility; //credibility of the SR based on the centrality and honesty
    real currentServiceTime; //the exponential service time length this SR will use the SP for
    real currentSPServiceStartTime; //simulation clock time when the SR received service from SR
    real currentSPQueueStartTime; //simulation clock time when the SR was selected to request a server. Difference between Service Start Time and Queue Start Time is the actual wait time
    real currentSPAdvertisedWaitTime; //the length of time to wait before getting service as advertised by selected SP
    //int currentSPWaitTimeRating; //The service wait time rating, a number from 1 to 5, with 1 being least satisfaction and 5 being most satisfaction.
    //real currentSPWitnessWaitTime; //The wait time this SR will advertise to other SR as witness when getting service from SP
    struct ServiceProvider* currentSP;
    struct Witness witnesses[N_SR]; //an array of witnesses that have had previous service experience with selected SP
    struct ServiceExperience serviceExperiences[N_SP]; //service experience recorded after last experience with a service provider
    struct ServiceExperienceHistory experienceHistory[N_SP];
    struct SRFeedback feedbacks[N_SR]; //feedback provided by other SRs
};

//methods
void myReport(int isFinalReport);
void testReport(real *report1LastPrinted, real currentTime);
void figure1Report(real* lastReportedTime, real currentTime);
void setupServiceProviders();
void setupServiceRequesters();
void setupMaliciousSPSR();
void setSelectedServiceProvider(struct ServiceRequester* serRequester, struct ServiceProvider* selectedSP, real currentTime);
real getSPAdvertisedWaitTime(struct ServiceProvider* SP, real currentTime);
real getProjectedWaitTime(struct ServiceProvider* SP, struct ServiceRequester* servRequester, real advertisedWaitTime);
real calculateSPTrustScoreBySR(struct ServiceProvider *SP, struct ServiceRequester *servRequester, struct Witness *witnessArray, int witnessCount);
//void getAllWitnessOrderedByCredibility(struct ServiceProvider* servProvider, struct Witness* witnessArray, int* witnessCount);
void getrecommendersForSR(struct ServiceRequester* servRequester, struct ServiceProvider* servProvider, struct Witness* witnessArray, int *witnessCount);
//real rateSPExperience(real advertisedWaitTime, real actualWaitTime, struct ServiceProvider* servProvider, struct ServiceRequester* sRequester);
//void highestCentrality(struct ServiceProvider* servProvider, struct ServiceRequester* servRequester, real newRating);
void updateServiceExperience(struct ServiceRequester* servRequeters);
void addNewSRToQueueAndUpdateAvailabilitySlot(struct ServiceProvider* SP, struct ServiceRequester* SR);
struct ServiceProvider* getLeastBusySP(struct ServiceRequester* servRequester, real currentTime);
struct ServiceProvider *getMostTrustworthySP(struct ServiceRequester *servRequester);
struct ServiceProvider *getLeastBusySPAmongMostTrustworthySP(struct ServiceRequester *servRequester, real currentTime);
void cleanupServiceRequesterBeforeLeaving(struct ServiceRequester* servRequester);
void figure3Report(real *report1LastPrinted, real currentTime);
void figure4Report(real *report1LastPrinted, real currentTime);
void figure5And6Report(real *report1LastPrinted, real currentTime);