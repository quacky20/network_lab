#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;
using namespace std;

int main(){
	NodeContainer nodes;
	nodes.Create(3);
	
	// PointToPointHelper p2p;

	// p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	// p2p.SetChannelAttribute("Delay", StringValue("5ms"));
	
	PointToPointHelper p2p1;
	PointToPointHelper p2p2;
	p2p1.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	p2p2.SetChannelAttribute("Delay", StringValue("5ms"));
	p2p1.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
	p2p2.SetChannelAttribute("Delay", StringValue("5ms"));
	
	p2p1.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));
	p2p2.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));

	NodeContainer n0n1 = NodeContainer(nodes.Get(0), nodes.Get(1));
	NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));

	NetDeviceContainer d0d1 = p2p1.Install(n0n1);
	NetDeviceContainer d1d2 = p2p2.Install(n1n2);

	InternetStackHelper internet;
	internet.Install(nodes);

	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i1 = address.Assign(d0d1);
	address.SetBase("10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = address.Assign(d1d2);
	
	// checking ip addresses of each node
	cout << "Node0 : \t\t" << i0i1.GetAddress(0) << endl;
	cout << "Node1 (Link 1): \t" << i0i1.GetAddress(1) << endl;
	cout << "Node1 (Link 2): \t" << i1i2.GetAddress(0) << endl;
	cout << "Node2 : \t\t" << i1i2.GetAddress(1) << endl;

	uint16_t port = 9;
	
	// server node
	UdpEchoServerHelper server(port);
	ApplicationContainer serverApp = server.Install(nodes.Get(2));
	serverApp.Start(Seconds(1.0));
	serverApp.Stop(Seconds(10.0));

	// client node
	UdpEchoClientHelper client(i1i2.GetAddress(1), port);
	client.SetAttribute("MaxPackets", UintegerValue(100));
	client.SetAttribute("Interval", TimeValue(Seconds(0.01)));
	client.SetAttribute("PacketSize", UintegerValue(512));
	
	ApplicationContainer clientApp = client.Install(nodes.Get(0));
	clientApp.Start(Seconds(2.0));
	clientApp.Stop(Seconds(10.0));
        
        
        // routing table
        Ipv4GlobalRoutingHelper::PopulateRoutingTables();
        
	// Ipv4GlobalRoutingHelper g;
	// Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(&std::cout);
	// g.PrintRoutingTableAllAt(Seconds(3.0), stream);


	// logging
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO); 
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
	
	FlowMonitorHelper flowHelper;
	Ptr<FlowMonitor> flowMonitor = flowHelper.InstallAll();
	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	flowMonitor->SerializeToXmlFile("lab8_4.xml", false, true);
	Simulator::Destroy();

	return 0;
}
