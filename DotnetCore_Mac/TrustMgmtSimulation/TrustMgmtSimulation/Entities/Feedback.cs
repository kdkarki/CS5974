
namespace TrustMgmtSimulation.Entities
{
    /// <summary>
    /// Base feedback class that will be derived into SR-feedback and SP-feeback
    /// </summary>
    public abstract class Feedback
    {
        public int TotalFeedbacks { get; set; }
        
        public double PositiveFeedbacks { get; internal set; }
        
        public double NegativeFeedbacks { get; internal set; }

        public abstract double UpdateFeedbacks(double? newPositiveFeedback, double? newNegativeFeedback);
        
    }
}