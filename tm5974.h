
// Table 1 parameters
#define N_SP	10
#define N_SR	2200
#define M_SP	20
#define Ta	10.0
#define Ts	1.0
//#define T_Margin	0.20 //threshold for wait time to be considered inaccurate
#define Param_R	0.50 //the wait time offset reported by a malicious SP
#define N_Rec	5
#define NON_EXISTENT -1

void myReport();
real getSPAdvertisedWaitTime();
struct ServiceProvider* getLeastBusySP();
void addNewSRToQueue();
void removeSRFromQueue();
void addSRToSPServiceArray();
void removeSRFromSPServiceArray();
void updateFeedbackAndWaitTime();