#include "tcp-adaptive-reno.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE("TcpAdaptiveReno");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED(TcpAdaptiveReno);

TypeId TcpAdaptiveReno::GetTypeId(void){

    static TypeId tid = TypeId("ns3::TcpAdaptiveReno")
            .SetParent<TcpNewReno>()
            .SetGroupName ("Internet")
            .AddConstructor<TcpAdaptiveReno>()
            .AddAttribute("FilterType", "Use this to choose no filter or Tustin's approximation filter",
                            EnumValue(TcpAdaptiveReno::TUSTIN), MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                            MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
            .AddTraceSource("EstimatedBW", "The estimated bandwidth",
                            MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                            "ns3::TracedValueCallback::Double")
            ;
    return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno(void) :
    TcpWestwoodPlus(),
    m_minimumRtt(Time(0)),
    m_currentRtt(Time(0)),
    m_jPacketLossRtt(Time(0)),
    m_conjestionRtt(Time(0)),
    m_prevConjestionRtt(Time(0)),
    m_incrementWnd(0),
    m_baseWnd(0),
    m_probeWnd(0)
{
  NS_LOG_FUNCTION (this);
}

TcpAdaptiveReno::TcpAdaptiveReno(const TcpAdaptiveReno& sock) :
    TcpWestwoodPlus(sock),
    m_minimumRtt(Time (0)),
    m_currentRtt(Time (0)),
    m_jPacketLossRtt(Time (0)),
    m_conjestionRtt(Time (0)),
    m_prevConjestionRtt(Time(0)),
    m_incrementWnd(0),
    m_baseWnd(0),
    m_probeWnd(0)
{
  NS_LOG_FUNCTION(this);
  NS_LOG_LOGIC("Invoked the copy constructor");
}

TcpAdaptiveReno::~TcpAdaptiveReno(void){}

/*
this function is called with acked signal
*/
void TcpAdaptiveReno::PktsAcked(Ptr<TcpSocketState> tcb , uint32_t packetsAcked , const Time& rtt){

    NS_LOG_FUNCTION(this << tcb << packetsAcked << rtt);
    if (rtt.IsZero()){
        NS_LOG_WARN ("RTT measured is zero!");
        return;
    }

    m_ackedSegments += packetsAcked;


    // calculate min rtt here
    if(m_minimumRtt.IsZero()){ 
        m_minimumRtt = rtt; 
    }
    else if(rtt <= m_minimumRtt){ 
        m_minimumRtt = rtt; 
    }

    m_currentRtt = rtt;
    NS_LOG_LOGIC("Min RTT is : "<<m_minimumRtt.GetMilliSeconds() << "ms");
    NS_LOG_LOGIC ("CurRtt: " << m_currentRtt.GetMilliSeconds() << "ms");

    TcpWestwoodPlus::EstimateBW(rtt , tcb);
}


double TcpAdaptiveReno::EstimateCongestionLevel(){

  /*

      VARIABLE m_a = EXPONENTIAL SMOOTHING FACTOR
      m_maxRTT = (1-m_a)*m_prevMaxRtt + m_a*m_maxRTT
      
      RETURN:
      congestion_level = min((
          (m_currentRtt - m_minimumRtt) / (m_maxRTT - m_minimumRtt)
      ), 1)

  */

    float m_a = 0.85; // exponential smoothing factor
    if(m_prevConjestionRtt < m_minimumRtt) m_a = 0; // the initial value should take the full current Jth loss Rtt
  
    double conjestion_Rtt = m_a * m_prevConjestionRtt.GetSeconds() + (1 - m_a ) * m_jPacketLossRtt.GetSeconds(); // TODO : do i need to reset m_maxRtt value to this
    m_conjestionRtt = Seconds(conjestion_Rtt); // for next step calcu
    NS_LOG_LOGIC("conj rtt : " << m_conjestionRtt << " ; m_prevConjestionRtt : " << m_prevConjestionRtt << " ; j rtt : " << m_jPacketLossRtt);

    return std::min((m_currentRtt.GetSeconds() - m_minimumRtt.GetSeconds()) / (conjestion_Rtt - m_minimumRtt.GetSeconds()) , 1.0);
}


void TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb){

  /*

      c = EstimateCongestionLevel() // congestion level
      scalingFactor_m = 10 // in Mbps
      m_maxIncWnd = EstimateBW() / scalingFactor_m

      alpha = 10 
      beta = 2 * m_maxIncWnd * (1/alpha - (1/alpha + 1)/(e^alpha))
      gamma = 1 - 2 * m_maxIncWnd * (1/alpha - (1/alpha + 1/2)/(e^alpha))

      m_incrementWnd = (m_maxIncWnd / (e^(alpha * c))) + (beta * c) + gamma

  */

    double congestion = EstimateCongestionLevel();
    int scalingFactor_m = 1000; // 10 mbps in paper 
  
  // m_currentBW; -> already calculated in packetsAck
    //double m_maxIncWnd = m_currentBW / scalingFactor_m * static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) ; 
    double m_maxIncWnd = static_cast<double>(m_currentBW.Get().GetBitRate() / scalingFactor_m * static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize));

    double alpha = 10; // 2 10
    double beta = 2 * m_maxIncWnd * ((1 / alpha) - (((1 / alpha) + 1) / (std::exp(alpha))));
    double gamma = 1 - (2 * m_maxIncWnd * ((1 / alpha ) - (((1 / alpha) + 0.5) / (std::exp(alpha)))));

    m_incrementWnd = (int)((m_maxIncWnd / std::exp(alpha * congestion)) + (beta * congestion) + gamma);

    NS_LOG_LOGIC ("MinRtt: " << m_minimumRtt.GetMilliSeconds () << "ms");
    NS_LOG_LOGIC ("Conjestion_Rtt: " << m_conjestionRtt.GetMilliSeconds () << "ms");
    NS_LOG_LOGIC("maxInc: "<<m_maxIncWnd<<"; congestion: "<<congestion<<" ; beta: "<<beta<<" ; gamma: "<<gamma<<
    " ; exp(alpha * congestion): "<<std::exp(alpha * congestion));
    NS_LOG_LOGIC("m_incrementWnd: "<<m_incrementWnd<<" ; prev_wind: "<<tcb->m_cWnd<<" ; new: "<<(m_incrementWnd / (int)tcb->m_cWnd));

    NS_LOG_LOGIC ("Congestion level: " << congestion);
    NS_LOG_LOGIC ("Increment Window: " << m_incrementWnd);
}


void TcpAdaptiveReno::CongestionAvoidance(Ptr<TcpSocketState> tcb , uint32_t segmentsAcked){

  /*
  
      base_window = USE NEW RENO IMPLEMENTATION
      m_probeWnd = max(m_probeWnd + m_incrementWnd / current_window,  0)
      current_window = base_window + m_probeWnd

  */

    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    if (segmentsAcked > 0){
        EstimateIncWnd(tcb);
        // base_window = USE NEW RENO IMPLEMENTATION
        double adder = static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get ();
        adder = std::max (1.0, adder);
        m_baseWnd += static_cast<uint32_t> (adder);

        // change probe window
        m_probeWnd = std::max((double) (m_probeWnd + m_incrementWnd / (int)tcb->m_cWnd.Get()), (double) 0);
      
        NS_LOG_LOGIC("Before "<<tcb->m_cWnd<< " ; base "<<m_baseWnd<<" ; probe "<<m_probeWnd);
        tcb->m_cWnd = m_baseWnd + m_probeWnd;
        // NS_LOG_UNCOND("Congestion window changed");

        NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<
                    " ssthresh " << tcb->m_ssThresh);
    }

}

uint32_t TcpAdaptiveReno::GetSsThresh(Ptr<const TcpSocketState> tcb , uint32_t bytesInFlight){

  /*
  
      c = EstimateCongestionLevel() // congestion level
      RETURN:
      new_window = current_window / (1 + c)

  */
    m_prevConjestionRtt = m_conjestionRtt; // a loss event has occured. so set the previous conjestion RTT
    m_jPacketLossRtt = m_currentRtt; // this will now contain the RTT of previous packet or jth loss event
  
    double congestion = EstimateCongestionLevel();
    // NS_LOG_UNCOND("SSThresh called");

    uint32_t ssthresh = std::max(2 * tcb->m_segmentSize , (uint32_t) (tcb->m_cWnd / (1.0 + congestion)));

  // reset calculations
    m_baseWnd = ssthresh;
    m_probeWnd = 0;
  
    NS_LOG_LOGIC("new ssthresh : "<<ssthresh<<" ; old conj Rtt : "<<m_prevConjestionRtt<<" ; new conj Rtt : "<<m_conjestionRtt<<" ; cong : "<<congestion);
  
    return ssthresh;

}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork()
{
  return CreateObject<TcpAdaptiveReno> (*this);
}

} // namespace ns3