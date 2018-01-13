

namespace TrustMgmtSimulation
{
    public class MMC1 : ISimulation
    {
        public void Run()
        {
            double Ta=200.0,Ts=100.0,te=200000.0;
            int customer=1, eventId = -1, server = -1;
            
            SMPLWrapper.smpl(0,"M/M/1 Queue");
            
            server = SMPLWrapper.facility("server",1);
            SMPLWrapper.schedule(1,0.0,customer);
            while (SMPLWrapper.time()<te)
            {
                SMPLWrapper.cause(ref eventId, ref customer);
                switch(eventId)
                {
                    case 1: 
                    SMPLWrapper.schedule(2,0.0,customer);
                    SMPLWrapper.schedule(1,SMPLWrapper.expntl(Ta),customer);
                    break;
                    case 2: 
                    if (SMPLWrapper.request(server,customer,0)==0)
                        SMPLWrapper.schedule(3, SMPLWrapper.expntl(Ts),customer);
                    break;
                    case 3: 
                    SMPLWrapper.release(server,customer);
                    break;
                }
            }
            SMPLWrapper.report();
        }
    }
}