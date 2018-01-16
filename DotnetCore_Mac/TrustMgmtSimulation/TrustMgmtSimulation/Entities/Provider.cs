using System;

namespace TrustMgmtSimulation.Entities
{
    public class Provider : IParticipant
    {
        public int Id { get; set; }
        
        public bool IsMalicious { get; set; }
    }
}