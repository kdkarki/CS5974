using System;

namespace TrustMgmtSimulation.Entities
{
    public class Customer : IParticipant
    {
        public int Id { get; set; }
        
        public bool IsMalicious { get; set; }
        
    }
}