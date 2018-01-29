
namespace TrustMgmtSimulation.Entities
{
    public class WitnessFeedback : Feedback
    {
        /// <summary>
        /// The customer who is getting the feedback.
        /// If customer A is getting Witnessfeedback from customer B
        /// then FeedbackForCustomer is customer A in this example
        /// </summary>
        /// <returns></returns>
        public Customer FeedbackForCustomer { get; set; }

        /// <summary>
        /// The honesty from FeedbackForCustomer towards the witness customer
        /// </summary>
        /// <returns></returns>
        public double Honesty { get; set; }
        
        /// <summary>
        /// Update the positive and negative feedbacks with the new values and return new honesty.
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
            Honesty = (PositiveFeedbacks / (PositiveFeedbacks + NegativeFeedbacks));

            return Honesty;
        }
        
    }
}