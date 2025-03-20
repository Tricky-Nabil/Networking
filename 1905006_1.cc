/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

#include<bits/stdc++.h>

using namespace ns3;
using std::endl;

int packet_Sent = 0;

std::map<Address , int> received_Size;

void onOffHelper_for_Tx(Ptr<const Packet> packet){
    //std::cout<<"Hiii";
    packet_Sent++;
}

void sinkHelper_for_Rx(Ptr<const Packet> packet , const Address &addr){
    received_Size[addr] += packet->GetSize();
    //std::cout<<"Byeee";
}

int main(int argc , char* argv[]){
    const int tx_range = 5 , packet_size = 1024;
    int node_num = 20 , flow_num = 10 , packet_num = 100 , coverage_area = 1 * tx_range , graph = -1;
    uint64_t data_Rate = (packet_num * packet_size * 8);
    std::string thorough_put_file_name = "throughput.dat";
    std::string delivery_ratio_file_name = "deliveryratio.dat";

    CommandLine cmd(__FILE__);
    cmd.AddValue("node_num" , "Number of nodes" , node_num);
    cmd.AddValue("flow_num" , "Number of flows" , flow_num);
    cmd.AddValue("packet_num" , "Number of packets" , packet_num);
    cmd.AddValue("coverage_area" , "Coverage Area" , coverage_area);
    cmd.AddValue("graph" , "Attribute to be plotted" , graph);
    cmd.AddValue("tpfile" , "name of throughput file" , thorough_put_file_name);
    cmd.AddValue("drfile" , "name of dilivery ratio file", delivery_ratio_file_name);
    cmd.Parse(argc , argv);

    Time::SetResolution(Time::NS);

    int servers = node_num / 2;
    int receivers = node_num - servers;
    flow_num = std::max(servers , flow_num);
    coverage_area = coverage_area * tx_range;

    //LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);

    NodeContainer access_point_nodes;
    access_point_nodes.Create(2);

    PointToPointHelper p2pHelper;
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
    //p2pHelper.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer access_point_devices;
    access_point_devices = p2pHelper.Install(access_point_nodes);

    NodeContainer server_nodes , receiver_nodes;
    server_nodes.Create(servers);
    receiver_nodes.Create(receivers);
    
    NodeContainer server_apNodes = access_point_nodes.Get(0);
    NodeContainer receiver_apNodes = access_point_nodes.Get(1);
    // NodeContainer server_apNodes , receiver_apNodes;
    // server_apNodes.Add(access_point_nodes.Get(0));
    // receiver_apNodes.Add(access_point_nodes.Get(1));

    YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default();
    channel1.AddPropagationLoss("ns3::RangePropagationLossModel" , "MaxRange" , DoubleValue(coverage_area));
    YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default();
    channel2.AddPropagationLoss("ns3::RangePropagationLossModel" , "MaxRange" , DoubleValue(coverage_area));

    YansWifiPhyHelper phy1;
    YansWifiPhyHelper phy2;
    phy1.SetChannel(channel1.Create());
    phy2.SetChannel(channel2.Create());

    WifiMacHelper mac1;
    WifiMacHelper mac2;
    Ssid ssid1 = Ssid("ns-3-ssid");
    Ssid ssid2 = Ssid("ns-3-ssid");
    mac1.SetType("ns3::StaWifiMac" , "Ssid" , SsidValue(ssid1) , "ActiveProbing" , BooleanValue(false));
    mac2.SetType("ns3::StaWifiMac" , "Ssid" , SsidValue(ssid2) , "ActiveProbing" , BooleanValue(false));

    WifiHelper  wifi1;

    NetDeviceContainer staDevices1 , staDevices2;
    staDevices1 = wifi1.Install(phy1 , mac1 , server_nodes);
    staDevices2 = wifi1.Install(phy2 , mac2 , receiver_nodes);

    mac1.SetType("ns3::ApWifiMac" , "Ssid" , SsidValue(ssid1));
    mac2.SetType("ns3::ApWifiMac" , "Ssid" , SsidValue(ssid2));

    NetDeviceContainer access_point_devices_1 , access_point_devices_2;
    access_point_devices_1 = wifi1.Install(phy1 , mac1 , server_apNodes);
    access_point_devices_2 = wifi1.Install(phy2 , mac2 , receiver_apNodes);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(1.0),
                                  "DeltaY",
                                  DoubleValue(1.0),
                                  "GridWidth",
                                  UintegerValue(10),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(server_apNodes);
    mobility.Install(server_nodes);
    mobility.Install(receiver_apNodes);
    mobility.Install(receiver_nodes);

    InternetStackHelper stack;
    stack.Install(server_nodes);
    stack.Install(access_point_nodes);
    stack.Install(receiver_nodes);

    Ipv4AddressHelper address;
    
    address.SetBase("10.1.1.0" , "255.255.255.0");
    Ipv4InterfaceContainer access_point_nodes_interfaces;
    access_point_nodes_interfaces = address.Assign(access_point_devices);

    address.SetBase("10.1.2.0" , "255.255.255.0");
    Ipv4InterfaceContainer server_nodes_interfaces , access_point_nodes_interfaces1;
    server_nodes_interfaces = address.Assign(staDevices1);
    access_point_nodes_interfaces1 = address.Assign(access_point_devices_1);

    address.SetBase("10.1.3.0" , "255.255.255.0");
    Ipv4InterfaceContainer receiver_nodes_interfaces , access_point_nodes_interfaces2;
    receiver_nodes_interfaces = address.Assign(staDevices2);
    access_point_nodes_interfaces2 = address.Assign(access_point_devices_2);

    // UdpEchoServerHelper echoServer(9);

    // ApplicationContainer serverApps = echoServer.Install(receiver_nodes);
    // serverApps.Start(Seconds(1.0));
    // serverApps.Stop(Seconds(10.0));

    // std::vector<UdpEchoClientHelper> echoClients;
    // for(int i = 0 ; i < receivers ; i++){
    //     UdpEchoClientHelper echoClient(receiver_nodes_interfaces.GetAddress(i) , 9);
    //     echoClient.SetAttribute("Interval" , TimeValue(Seconds(1.0/packet_num)));
    //     echoClient.SetAttribute("PacketSize" , UintegerValue(1024));
    //     echoClients.push_back(echoClient);
    // }

    //int flow_per_nodes = flow_num / servers;
    for(int i = 0 ; i < servers ; i++){
        for(int j = 0 ; j <= (flow_num / servers) ; j++){
            PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory" , InetSocketAddress(receiver_nodes_interfaces.GetAddress(i) , 9+j));
            ApplicationContainer sinkApps = packetSinkHelper.Install(receiver_nodes.Get(i));
            sinkApps.Start(Seconds(0.0));
            sinkApps.Stop(Seconds(10.0));

            Ptr<PacketSink> packetSink = StaticCast<PacketSink>(sinkApps.Get(0));
            packetSink->TraceConnectWithoutContext("Rx" , MakeCallback(&sinkHelper_for_Rx));

        }
    } 

    for(int i = 0 ; i < servers ; i++){
        for(int j = 0 ; j < (flow_num/servers) ; j++){
        
            OnOffHelper onOffHelper("ns3::TcpSocketFactory" , InetSocketAddress(receiver_nodes_interfaces.GetAddress(i) , 9+j));
            onOffHelper.SetAttribute("PacketSize" , UintegerValue(1024));
            onOffHelper.SetAttribute("DataRate" , DataRateValue(DataRate(data_Rate)));

            ApplicationContainer sendApps = onOffHelper.Install(server_nodes.Get(i));
            sendApps.Start(Seconds(0.0));
            sendApps.Stop(Seconds(10.0));

            //std::cout<<"oooo"<<endl;

            Ptr<OnOffApplication> senders = StaticCast<OnOffApplication>(sendApps.Get(0));
            senders->TraceConnectWithoutContext("Tx" , MakeCallback(&onOffHelper_for_Tx));
        }
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Simulator::Destroy();

    int received_total = 0 , received_packet = 0;
    for(auto i : received_Size){
        received_packet += i.second / 1024;
        received_total += i.second;
    }

    double thorough_put = received_total * 8/1e6 /10;
    double delivery_ratio = received_packet / (double)packet_Sent;
    NS_LOG_UNCOND("Throughput : " << thorough_put);
    NS_LOG_UNCOND("Delivery Ratio : " << delivery_ratio);
    
    if(graph != -1){
        std::ofstream thorough_put_file(thorough_put_file_name , std::ios::app);
        std::ofstream delivery_ratio_file(delivery_ratio_file_name , std::ios::app);

        if(graph == 0){
            thorough_put_file << node_num << "\t" << thorough_put <<endl;
            delivery_ratio_file << node_num << "\t" << delivery_ratio <<endl;
        }
        else if(graph == 1){
            thorough_put_file << flow_num << "\t" << thorough_put << endl;
            delivery_ratio_file << flow_num << "\t" <<delivery_ratio << endl;
        }
        else if(graph == 2){
            thorough_put_file << packet_num << "\t" << thorough_put << endl;
            delivery_ratio_file << packet_num << "\t" << delivery_ratio << endl;
        }
        else if(graph == 3){
            thorough_put_file << coverage_area << "\t" << thorough_put << endl;
            delivery_ratio_file << coverage_area << "\t" << delivery_ratio << endl;
        }
    }

    return 0;

}
