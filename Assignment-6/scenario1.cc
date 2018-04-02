/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
 *
 */

//
// Network topology
//
//           10Mb/s, 10ms       10Mb/s, 10ms
//       n0-----------------n1-----------------n2
//
//
// - Tracing of queues and packet receptions to file 
//   "tcp-large-transfer.tr"
// - pcap traces also generated in the following files
//   "tcp-large-transfer-$n-$i.pcap" where n and i represent node and interface
// numbers respectively
//  Usage (e.g.): ./waf --run tcp-large-transfer

#include <iostream>
#include <fstream>
#include <string>
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/traffic-control-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpLargeTransfer");

// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 2000000;
// static uint32_t currentTxBytes = 0;
// Perform series of 1040 byte writes (this is a multiple of 26 since
// we want to detect data splicing in the output stream)
static const uint32_t writeSize = 1040;
uint8_t data[writeSize];

// These are for starting the writing process, and handling the sending 
// socket's notification upcalls (events).  These two together more or less
// implement a sending "Application", although not a proper ns3::Application
// subclass.

void StartFlow (Ptr<Socket>, Ipv4Address, uint16_t);
void WriteUntilBufferFull (Ptr<Socket>, uint32_t);

// static void 
// CwndTracer (uint32_t oldval, uint32_t newval)
// {
//   NS_LOG_INFO ("Moving cwnd from " << oldval << " to " << newval);
// }

int main (int argc, char *argv[])
{
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  //  LogComponentEnable("TcpL4Protocol", LOG_LEVEL_ALL);
  //  LogComponentEnable("TcpSocketImpl", LOG_LEVEL_ALL);
  //  LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
  //  LogComponentEnable("TcpLargeTransfer", LOG_LEVEL_ALL);
 std::string redLinkDataRate = "1.5Mbps";
  std::string redLinkDelay = "20ms";
 TrafficControlHelper tchPfifo;
  uint16_t handle = tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
  tchPfifo.AddInternalQueues (handle, 3, "ns3::DropTailQueue", "MaxPackets", UintegerValue (1000));

  TrafficControlHelper tchRed;
  tchRed.SetRootQueueDisc ("ns3::RedQueueDisc", "LinkBandwidth", StringValue (redLinkDataRate),
                           "LinkDelay", StringValue (redLinkDelay));
  Config::SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (5));
  Config::SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (15));
  Config::SetDefault ("ns3::RedQueueDisc::QueueLimit", UintegerValue (1000));
  CommandLine cmd;
  cmd.Parse (argc, argv);

  // initialize the tx buffer.
  for(uint32_t i = 0; i < writeSize; ++i)
    {
      char m = toascii (97 + i % 26);
      data[i] = m;
    }

  // Here, we will explicitly create three nodes.  The first container contains
  // nodes 0 and 1 from the diagram above, and the second one contains nodes
  // 1 and 2.  This reflects the channel connectivity, and will be used to
  // install the network interfaces and connect them with a channel.
  NodeContainer c;
  c.Create (10);

  NodeContainer n0n1=NodeContainer(c.Get(0),c.Get(1));
  NodeContainer n1n7=NodeContainer(c.Get(1),c.Get(7));
  NodeContainer n1n8=NodeContainer(c.Get(1),c.Get(8));
  NodeContainer n1n9=NodeContainer(c.Get(1),c.Get(9));
  NodeContainer n1n2=NodeContainer(c.Get(1),c.Get(2));
  NodeContainer n2n4=NodeContainer(c.Get(2),c.Get(4));
  NodeContainer n2n5=NodeContainer(c.Get(2),c.Get(5));
  NodeContainer n2n6=NodeContainer(c.Get(2),c.Get(6));
  NodeContainer n2n3=NodeContainer(c.Get(2),c.Get(3));

  // We create the channels first without any IP addressing information
  // First make and configure the helper, so that it will put the appropriate
  // attributes on the network interfaces and channels we are about to install.
  PointToPointHelper p;
   p.SetDeviceAttribute ("DataRate", StringValue("1Mbps"));
   p.SetQueue("ns3::DropTailQueue");
  p.SetChannelAttribute ("Delay", StringValue("2ms"));

  PointToPointHelper p1;
   p1.SetDeviceAttribute ("DataRate", StringValue("100Kbps"));
   p1.SetQueue("ns3::DropTailQueue");
  p1.SetChannelAttribute ("Delay", StringValue("10ms"));
  
  // And then install devices and channels connecting our topology.
  NetDeviceContainer dev0 = p.Install (n0n1);
  NetDeviceContainer dev1 = p.Install (n1n7);
  NetDeviceContainer dev2 = p.Install (n1n8);
  NetDeviceContainer dev3 = p.Install (n1n9); 
  NetDeviceContainer dev4 = p1.Install (n1n2);
  NetDeviceContainer dev5 = p.Install (n2n4);
  NetDeviceContainer dev6 = p.Install (n2n5);
  NetDeviceContainer dev7 = p.Install (n2n6);
  NetDeviceContainer dev8 = p.Install (n2n3);

  // Now add ip/tcp stack to all nodes.
  InternetStackHelper internet;
  internet.InstallAll ();
  tchPfifo.Install (dev0);
tchPfifo.Install (dev1);
tchPfifo.Install (dev2);
tchPfifo.Install (dev3);
tchRed.Install (dev4);
tchPfifo.Install (dev5);
tchPfifo.Install (dev6);
tchPfifo.Install (dev7);
tchPfifo.Install (dev8);


  // Later, we add IP addresses.
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.0.0", "255.255.255.0");
  Ipv4InterfaceContainer i01 = ipv4.Assign (dev0);
  
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i17 = ipv4.Assign (dev1);
  
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i18 = ipv4.Assign (dev2);
  
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i19 = ipv4.Assign (dev3);
  
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i12 = ipv4.Assign (dev4);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i24 = ipv4.Assign (dev5);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i25 = ipv4.Assign (dev6);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i26 = ipv4.Assign (dev7);

  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer i23 = ipv4.Assign (dev8);

  // and setup ip routing tables to get total ip-level connectivity.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  ///////////////////////////////////////////////////////////////////////////
  // Simulation 1
  //
  // Send 2000000 bytes over a connection to server port 50000 at time 0
  // Should observe SYN exchange, a lot of data segments and ACKS, and FIN 
  // exchange.  FIN exchange isn't quite compliant with TCP spec (see release
  // notes for more info)
  //
  ///////////////////////////////////////////////////////////////////////////

  uint16_t port = 50000;
  OnOffHelper source03 ("ns3::TcpSocketFactory",InetSocketAddress (i23.GetAddress(1), port));
  ApplicationContainer sourceApps03 = source03.Install (c.Get (0));
  sourceApps03.Start (Seconds (0.0));
  sourceApps03.Stop (Seconds (60.0));

  PacketSinkHelper sink03 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps03 = sink03.Install (c.Get(3));
  sinkApps03.Start (Seconds (0.0));
  sinkApps03.Stop (Seconds (60.0));

  OnOffHelper source74 ("ns3::UdpSocketFactory",InetSocketAddress (i24.GetAddress(1), port));
  ApplicationContainer sourceApps74 = source74.Install (c.Get (7));
  sourceApps74.Start (Seconds (0.0));
  sourceApps74.Stop (Seconds (60.0));

  PacketSinkHelper sink74 ("ns3::UdpSocketFactory",InetSocketAddress (Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps74 = sink74.Install (c.Get(4));
  sinkApps74.Start (Seconds (0.0));
  sinkApps74.Stop (Seconds (60.0));

  OnOffHelper source85 ("ns3::UdpSocketFactory",InetSocketAddress (i25.GetAddress(1), port));
  ApplicationContainer sourceApps85 = source85.Install (c.Get (8));
  sourceApps85.Start (Seconds (0.0));
  sourceApps85.Stop (Seconds (60.0));

  PacketSinkHelper sink85 ("ns3::UdpSocketFactory",InetSocketAddress (Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps85 = sink85.Install (c.Get(5));
  sinkApps85.Start (Seconds (0.0));
  sinkApps85.Stop (Seconds (60.0));

  OnOffHelper source96 ("ns3::UdpSocketFactory",InetSocketAddress (i26.GetAddress(1), port));
  ApplicationContainer sourceApps96 = source96.Install (c.Get (9));
  sourceApps96.Start (Seconds (0.0));
  sourceApps96.Stop (Seconds (60.0));

  PacketSinkHelper sink96 ("ns3::UdpSocketFactory",InetSocketAddress (Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps96 = sink96.Install (c.Get(6));
  sinkApps96.Start (Seconds (0.0));
  sinkApps96.Stop (Seconds (60.0));

  //Ask for ASCII and pcap traces of network traffic
  AsciiTraceHelper ascii;
  p.EnableAsciiAll (ascii.CreateFileStream ("default.tr"));
  p.EnablePcapAll ("default");



  AnimationInterface anim("animation-scenario1(without queue).xml");
  anim.SetConstantPosition(c.Get(0), 0.0, 10.0);
  anim.SetConstantPosition(c.Get(1), 20.0, 10.0);  
  anim.SetConstantPosition(c.Get(2), 40.0, 10.0);  
  anim.SetConstantPosition(c.Get(3), 60.0, 10.0);  
  anim.SetConstantPosition(c.Get(4), 30.0, 20.0);  
  anim.SetConstantPosition(c.Get(5), 40.0, 20.0);  
  anim.SetConstantPosition(c.Get(6), 50.0, 20.0);  
  anim.SetConstantPosition(c.Get(7), 10.0, 0.0);  
  anim.SetConstantPosition(c.Get(8), 20.0, 0.0);  
  anim.SetConstantPosition(c.Get(9), 30.0, 0.0);  

  Simulator::Stop (Seconds (1000));


  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  std::string flow_name ("flow-scenario1(without queue).xml");
  Simulator::Run ();
  monitor->SerializeToXmlFile (flow_name.c_str(), false, true);


  Simulator::Destroy ();
}
