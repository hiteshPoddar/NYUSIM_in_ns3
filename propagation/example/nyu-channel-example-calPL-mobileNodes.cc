/*
*	Copyright (c) 2023 New York University and NYU WIRELESS
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
*	software and associated documentation files (the “Software”), to deal in the Software
*	without restriction, including without limitation the rights to use, copy, modify,
*	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
*	persons to whom the Software is furnished to do so, subject to the following conditions:
*	
*	The above copyright notice and this permission notice shall be included in all copies
*	or substantial portions of the Software. Users are encouraged to cite NYU WIRELESS 
*	publications regarding this work.
*	
*	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
*	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
*	BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
*	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
*	IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*	THE SOFTWARE.
*
* Author: Hitesh Poddar <hiteshp@nyu.edu>
*     
*/

/**
* This example shows how to configure the NYU channel model classes to
* compute the RxPower between two nodes. The Tx node is stationary whereas Rx is mobile.
* The default propagation environment is 3D-urban macro (UMi) and it can be configured changing the value of the
* string "scenario".
*/

#include "ns3/core-module.h"
#include <fstream>
#include "ns3/net-device.h"
#include "ns3/simple-net-device.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/mobility-model.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/channel-condition-model.h"
#include "ns3/nyu-propagation-loss-model.h"
#include <iomanip>
#include <ns3/mobility-model.h>

NS_LOG_COMPONENT_DEFINE ("NYUChannelExample");

using namespace ns3;

static Ptr<NYUPropagationLossModel> m_propagationLossModel; //!< the PropagationLossModel object

/** Compute the Rx Power for a CW signal without any noise figure,BW, Antenna Gains of Rx and Tx in consideration */
/* RxPwr = TxPwr - PL */
static void
CreateFile(std::string filename)
{
  std::ofstream f;
  f.open (filename, std::ios::out | std::ios::app);
  f << "Time" << std::setw(15) << "Rx Power" << std::endl;
  f.close();
}

static void
ComputeRxPwr (std::string filename, Ptr<MobilityModel> txMob,Ptr<MobilityModel> rxMob, double txPow)
{
  // apply the pathloss
  double RxPwr = m_propagationLossModel->CalcRxPower(txPow, txMob, rxMob);
  NS_LOG_DEBUG ("RxPwr " << RxPwr << " dBm");
  std::ofstream f;
  f.open (filename, std::ios::out | std::ios::app);
  // print the rxpower values in the PL-trace.txt file
  f << Simulator::Now ().GetSeconds () << std::setw(15) << RxPwr << std::endl;
  f.close ();
}

static void
ChangeRxPos(double distance2d, int i, Ptr<MobilityModel> rxMob)
{
  rxMob->SetPosition(Vector (distance2d*(i+1),0.0,1.6));
}

int
main (int argc, char *argv[])
{
  std::string filename = "rxpwr-trace-mobility.txt";
  double frequency = 28e9; // operating frequency in Hz (corresponds to 28 GHz)
  double txPow = 10.0; // tx power in dBm
  double distance = 1.0; // distance between tx and rx nodes in meters
  uint32_t simTime = 4500; // simulation time in milliseconds
  uint32_t timeRes = 1; // time resolution in milliseconds
  std::string scenario = "Umi"; // NYU propagation scenario
  
  //Config::SetDefault ("ns3::NYUChannelConditionModel::UpdatePeriod", TimeValue(MilliSeconds (1.0))); // update the channel condition every millisecond

  RngSeedManager::SetSeed(1);
  RngSeedManager::SetRun(1);

  // create and configure the factories for the channel condition and propagation loss models
  ObjectFactory propagationLossModelFactory;
  ObjectFactory channelConditionModelFactory;
  if (scenario == "Umi")
  {
    propagationLossModelFactory.SetTypeId (NYUUmiPropagationLossModel::GetTypeId ());
    channelConditionModelFactory.SetTypeId (NYUUmiChannelConditionModel::GetTypeId ());
  }
  else if (scenario == "Uma")
  {
    propagationLossModelFactory.SetTypeId (NYUUmaPropagationLossModel::GetTypeId ());
    channelConditionModelFactory.SetTypeId (NYUUmaChannelConditionModel::GetTypeId ());
  }
  else if (scenario == "Rma")
  {
    propagationLossModelFactory.SetTypeId (NYURmaPropagationLossModel::GetTypeId ());
    channelConditionModelFactory.SetTypeId (NYURmaChannelConditionModel::GetTypeId ());
  }
  else if(scenario == "InH")
  {
    propagationLossModelFactory.SetTypeId (NYUInHPropagationLossModel::GetTypeId ());
    channelConditionModelFactory.SetTypeId (NYUInHChannelConditionModel::GetTypeId ());
  }
  else if(scenario == "InF")
  {
    propagationLossModelFactory.SetTypeId (NYUInFPropagationLossModel::GetTypeId ());
    channelConditionModelFactory.SetTypeId (NYUInFChannelConditionModel::GetTypeId ());
  }
  else
  {
    NS_FATAL_ERROR ("Unknown scenario");
  }

  // create the propagation loss model and set attributes for the NYU Channel Model.
  m_propagationLossModel = propagationLossModelFactory.Create<NYUPropagationLossModel> ();
  m_propagationLossModel->SetAttribute ("Frequency", DoubleValue (frequency));
  m_propagationLossModel->SetAttribute ("O2ILosstype", StringValue ("High Loss"));
  m_propagationLossModel->SetAttribute ("FoliageLossEnabled", BooleanValue (false));
  m_propagationLossModel->SetAttribute ("FoliageLoss", DoubleValue (0.1));
 
  // create the tx and rx nodes
  NodeContainer nodes;
  nodes.Create (2);

  // create the tx and rx devices
  Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice> ();
  Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice> ();

  // create the tx and rx mobility models, set the positions
  Ptr<MobilityModel> txMob = CreateObject<ConstantPositionMobilityModel> ();
  txMob->SetPosition (Vector (0.0,0.0,10.0));
  Ptr<MobilityModel> rxMob = CreateObject<ConstantPositionMobilityModel> ();
  rxMob->SetPosition (Vector (distance,0.0,1.6));

  // associate the nodes and the devices
  nodes.Get (0)->AddDevice (txDev);
  txDev->SetNode (nodes.Get (0));
  nodes.Get (1)->AddDevice (rxDev);
  rxDev->SetNode (nodes.Get (1));

  // assign the mobility models to the nodes
  nodes.Get (0)->AggregateObject (txMob);
  nodes.Get (1)->AggregateObject (rxMob);

  // Create a File to write the RxPower during simulation
  CreateFile(filename);

  // this loop contains the event to simulate
  for (int i = 0; i < floor (simTime / timeRes); i++)
  {
    Simulator::Schedule (MilliSeconds (timeRes*i), &ChangeRxPos, distance, i,rxMob);
    Simulator::Schedule (MilliSeconds (timeRes*i), &ComputeRxPwr,filename,txMob, rxMob, txPow);
  }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
