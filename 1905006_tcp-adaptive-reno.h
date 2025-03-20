#ifndef TCP_ADAPTIVERENO_H
#define TCP_ADAPTIVERENO_H

#include "tcp-congestion-ops.h"
#include "tcp-westwood-plus.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"

namespace ns3{

class Packet;
class TcpHeader;
class Time;
class EventId;

// inherit TCPWestWood
class TcpAdaptiveReno:public TcpWestwoodPlus{
public:
    // Get typeid(static)

    static TypeId GetTypeId(void);
    TcpAdaptiveReno(void);


    TcpAdaptiveReno(const TcpAdaptiveReno& sock);
    virtual ~TcpAdaptiveReno(void);
    
    //Filter type (None or Tustin)
   

    enum FilterType{
        NONE,
        TUSTIN
    };

    virtual uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb , uint32_t bytesInFlight);
    virtual void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked , const Time& rtt);
    virtual Ptr<TcpCongestionOps> Fork();

private:
    // void UpdateAckedSegments (int acked);
    void EstimateBW(const Time& rtt, Ptr<TcpSocketState> tcb);
    double EstimateCongestionLevel();
    void EstimateIncWnd(Ptr<TcpSocketState> tcb);

protected:
    virtual void CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
    

    Time                   m_minimumRtt;                 //!< Minimum RTT
    Time                   m_currentRtt;             //!< Current RTT
    Time                   m_jPacketLossRtt;            //!< RTT of j packet loss
    Time                   m_conjestionRtt;                //!< Conjestion RTT (j th event)
    Time                   m_prevConjestionRtt;            //!< Previous Conjestion RTT (j-1 th event)

    // Window calculations
    int32_t                m_incrementWnd;                 //!< Increment Window
    uint32_t               m_baseWnd;                //!< Base Window
    int32_t                m_probeWnd;               //!< Probe Window 
};

} // namespace ns3

#endif /* TCP_ADAPTIVE_RENO_H */