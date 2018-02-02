using System;

namespace TrustMgmtSimulation.Entities
{
    public class Provider : IParticipant
    {
        public int Id { get; private set; }
        
        public bool IsMalicious { get; set; }

        public double TrustScore { get; private set; }

        public int TotalVisitors { get; private set; }

        public double NextAvailTime { get; private set; }

        public double[] QueueSlots { get; private set; }

        public Provider(int id, bool isMalicious, int queueLength)
        {
            this.Id = Id;
            this.IsMalicious = isMalicious;
            this.TrustScore = 0.5;//initial trust score should be 50%
            this.TotalVisitors = 0;
            this.NextAvailTime = 0.0;
            this.QueueSlots = new double[queueLength];
            for(int aSlot = 0; aSlot < queueLength; aSlot++)
            {
                QueueSlots[aSlot] = 0.0;
            }      
        }
                
    }
}