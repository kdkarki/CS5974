using System;

namespace TrustMgmtSimulation.Entities
{
    public interface IParticipant
    {
        int Id { get; }
        
        bool IsMalicious { get; }
    }
}