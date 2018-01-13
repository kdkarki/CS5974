
namespace TrustMgmtSimulation
{
    public class MM1_Bmeans : ISimulation
    {
        public void Run()
        {
            double Ta=200.0,Ts=100.0,mean = -1.0 ,hw = -1.0;
            int tk_id=0,customer=0, TOKENS = 1000,eventId = -1,server,nb = -1;
            double[] ts = new double[TOKENS]; /* start time stamp */
            bool cont=true;
            SMPLWrapper.smpl(0,"M/M/1 Queue");
            SMPLWrapper.init_bm(200,2000); /* let m0 be 200 and mb be 2000 observations */
            server=SMPLWrapper.facility("server",1);
            SMPLWrapper.schedule(1,0.0,tk_id);
            while (cont)
                {
                    SMPLWrapper.cause(ref eventId ,ref customer);
                    switch(eventId)
                    {
                        case 1:  /* arrival */
                        ts[customer] = SMPLWrapper.time();
                        SMPLWrapper.schedule(2,0.0,customer);
                        if (++tk_id >= TOKENS) tk_id=0;
                        SMPLWrapper.schedule(1,SMPLWrapper.expntl(Ta),tk_id);
                        break;
                        case 2:  /* request server */
                        if (SMPLWrapper.request(server,customer,0)==0)
                            SMPLWrapper.schedule(3,SMPLWrapper.expntl(Ts),customer);
                        break;
                        case 3:  /* release server */
                        SMPLWrapper.release(server,customer);
                        if (SMPLWrapper.obs(SMPLWrapper.time()-ts[customer]) == 1) cont = false;
                        break;
                    }
                }
            SMPLWrapper.civals(ref mean,  ref hw,  ref nb);
            System.Console.WriteLine("Mean is {0} and half width is {1} after {2} batches\n", mean, hw, nb);
        }
    }
}