
namespace TrustMgmtSimulation.Entities
{
    public class ProviderFeedback : Feedback
    {
        /// <summary>
        /// The service provider who is getting the feedback.
        /// If provider P is getting Providedrfeedback from customer B
        /// then FeedbackForCProvider is provider A in this example
        /// </summary>
        /// <returns></returns>
        public Provider FeedbackForProvider { get; set; }

        /// <summary>
        /// The trust score of the service provider for the customer
        /// </summary>
        /// <returns></returns>
        public double TrustScore { get; private set; }
        
        /// <summary>
        /// Update the positive and negative feedbacks with the new values and return new trust score.
        /// </summary>
        /// <param name="newPositiveFeedback">New positive feedback value to be added to the existing positivefeedbacks value</param>
        /// <param name="newNegativeFeedback">New negative feedback value to be added to the existing negativefeedbacks value</param>
        /// <returns>The Honesty of the FeedbackForCustomer towards the witness based on updated feedbacks.</returns>
        public override double UpdateFeedbacks(double? newPositiveFeedback, double? newNegativeFeedback)
        {
            if(newPositiveFeedback.HasValue)
            {
                this.PositiveFeedbacks += newPositiveFeedback.Value;
            }
            if(newNegativeFeedback.HasValue)
            {
                this.NegativeFeedbacks += newNegativeFeedback.Value;
            }
            TrustScore = (PositiveFeedbacks / (PositiveFeedbacks + NegativeFeedbacks));

            return TrustScore;
        }
        
    }
}