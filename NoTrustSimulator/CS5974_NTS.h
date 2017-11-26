// Table 1 parameters
#define N_SP	10	//total number of service providers
#define N_SR	2200	//total number of service requesters
#define M_SP	20 	//total number of SR to processed in parallel by an SP
#define Ta	10.0	//service requester arrival time
#define Ts	1.0		//service provider service time
//#define T_Margin	0.20 //threshold for wait time to be considered inaccurate
#define Param_R	1.0 //the wait time offset reported by a malicious SP
//#define N_Rec	5 //the number of top recommenders, SR, used to calculate SP trust score
//#define H_Thres 0.6 //honesty threshold
#define NON_EXISTENT -1
#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])))

struct ServiceExperience //Represents service experienced by an SR from an SP
{
    int totalVisits;
    struct ServiceProvider* sProvider;
};

struct ServiceProvider //Represents a service provider
{
    int id;
    real trustScore;
    int totalVisitorCount;
    int isMalicious;
    real nextAvailTime;//simulation clock time when the SP is available to service a new customer
    real availabilitySlotList[M_SP];//this is used to calculate the next available time
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
    struct ServiceExperience serviceExperiences[N_SP]; //service experience recorded after last experience with a service provider
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
void addNewSRToQueueAndUpdateAvailabilitySlot(struct ServiceProvider* SP, struct ServiceRequester* SR);
struct ServiceProvider* getLeastBusySP(struct ServiceRequester* servRequester, real currentTime);
void cleanupServiceRequesterBeforeLeaving(struct ServiceRequester* servRequester);