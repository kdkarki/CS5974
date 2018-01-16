using System;

namespace TrustMgmtSimulation.Entities
{
    public interface IParticipant
    {
        int Id { get; set; }
        
        bool IsMalicious { get; set; }
    }
}