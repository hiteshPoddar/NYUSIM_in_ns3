/*
*	Copyright (c) 2023 New York University and NYU WIRELESS
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this
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
 * This example shows how to configure the NYUSIM channel model classes to
 * compute the SNR between two nodes.
 * The simulation involves two static nodes which are placed at a certain
 * distance from each other and communicates through a wireless channel at
 * 2 GHz with a bandwidth of 18 MHz. The default propagation environment is
 * 3D-urban macro (UMa) and it can be configured changing the value of the
 * string "scenario".
 * Each node hosts a SimpleNetDevice and has an antenna array with 4 elements.
 */

#include "ns3/nyu-channel-condition-model.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/lte-spectrum-value-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/simple-net-device.h"
#include "ns3/spectrum-signal-parameters.h"
#include "ns3/nyu-channel-model.h"
#include "ns3/nyu-propagation-loss-model.h"
#include "ns3/nyu-spectrum-propagation-loss-model.h"
#include "ns3/uniform-planar-array.h"

#include <fstream>

NS_LOG_COMPONENT_DEFINE("NYUChannelExample");

using namespace ns3;

static Ptr<NYUPropagationLossModel>
    m_propagationLossModel; //!< the PropagationLossModel object
static Ptr<NYUSpectrumPropagationLossModel>
    m_spectrumLossModel; //!< the SpectrumPropagationLossModel object

/**
 * \brief A structure that holds the parameters for the
 * ComputeSnr function. In this way the problem with the limited
 * number of parameters of method Schedule is avoided.
 */
struct ComputeSnrParams
{
    Ptr<MobilityModel> txMob;        //!< the tx mobility model
    Ptr<MobilityModel> rxMob;        //!< the rx mobility model
    double txPow;                    //!< the tx power in dBm
    double noiseFigure;              //!< the noise figure in dB
    Ptr<PhasedArrayModel> txAntenna; //!< the tx antenna array
    Ptr<PhasedArrayModel> rxAntenna; //!< the rx antenna array
};

/**
 * Perform the beamforming using the DFT beamforming method
 * \param thisDevice the device performing the beamforming
 * \param thisAntenna the antenna object associated to thisDevice
 * \param otherDevice the device towards which point the beam
 */
static void
DoBeamforming(Ptr<NetDevice> thisDevice,
              Ptr<PhasedArrayModel> thisAntenna,
              Ptr<NetDevice> otherDevice)
{
    // retrieve the position of the two devices
    Vector aPos = thisDevice->GetNode()->GetObject<MobilityModel>()->GetPosition();
    Vector bPos = otherDevice->GetNode()->GetObject<MobilityModel>()->GetPosition();

    // compute the azimuth and the elevation angles
    Angles completeAngle(bPos, aPos);
    double hAngleRadian = completeAngle.GetAzimuth();

    double vAngleRadian = completeAngle.GetInclination(); // the elevation angle

    // retrieve the number of antenna elements and resize the vector
    uint64_t totNoArrayElements = thisAntenna->GetNumberOfElements();
    PhasedArrayModel::ComplexVector antennaWeights(totNoArrayElements);

    // the total power is divided equally among the antenna elements
    double power = 1.0 / sqrt(totNoArrayElements);

    // compute the antenna weights
    const double sinVAngleRadian = sin(vAngleRadian);
    const double cosVAngleRadian = cos(vAngleRadian);
    const double sinHAngleRadian = sin(hAngleRadian);
    const double cosHAngleRadian = cos(hAngleRadian);

    for (uint64_t ind = 0; ind < totNoArrayElements; ind++)
    {
        Vector loc = thisAntenna->GetElementLocation(ind);
        double phase = -2 * M_PI *
                       (sinVAngleRadian * cosHAngleRadian * loc.x +
                        sinVAngleRadian * sinHAngleRadian * loc.y + cosVAngleRadian * loc.z);
        antennaWeights[ind] = exp(std::complex<double>(0, phase)) * power;
    }

    // store the antenna weights
    thisAntenna->SetBeamformingVector(antennaWeights);
}

/**
 * Compute the average SNR
 * \param params A structure that holds the parameters that are needed to perform calculations in
 * ComputeSnr
 */
static void
ComputeSnr(const ComputeSnrParams& params)
{
    // Create the tx PSD using the LteSpectrumValueHelper
    // 100 RBs corresponds to 18 MHz (1 RB = 180 kHz)
    // EARFCN 100 corresponds to 2125.00 MHz
    std::vector<int> activeRbs0(100);
    for (int i = 0; i < 100; i++)
    {
        activeRbs0[i] = i;
    }
    Ptr<SpectrumValue> txPsd =
        LteSpectrumValueHelper::CreateTxPowerSpectralDensity(2100, 100, params.txPow, activeRbs0);
    Ptr<SpectrumSignalParameters> txParams = Create<SpectrumSignalParameters>();
    txParams->psd = txPsd->Copy();
    NS_LOG_DEBUG("Average tx power " << 10 * log10(Sum(*txPsd) * 180e3) << " dB");

    // create the noise PSD
    Ptr<SpectrumValue> noisePsd =
        LteSpectrumValueHelper::CreateNoisePowerSpectralDensity(2100, 100, params.noiseFigure);
    NS_LOG_DEBUG("Average noise power " << 10 * log10(Sum(*noisePsd) * 180e3) << " dB");

    // apply the pathloss
    double propagationGainDb = m_propagationLossModel->CalcRxPower(0, params.txMob, params.rxMob);
    NS_LOG_DEBUG("Pathloss " << -propagationGainDb << " dB");
    double propagationGainLinear = std::pow(10.0, (propagationGainDb) / 10.0);
    *(txParams->psd) *= propagationGainLinear;

    NS_ASSERT_MSG(params.txAntenna, "params.txAntenna is nullptr!");
    NS_ASSERT_MSG(params.rxAntenna, "params.rxAntenna is nullptr!");

    // apply the fast fading and the beamforming gain
    Ptr<SpectrumValue> rxPsd = m_spectrumLossModel->CalcRxPowerSpectralDensity(txParams,
                                                                               params.txMob,
                                                                               params.rxMob,
                                                                               params.txAntenna,
                                                                               params.rxAntenna);
    NS_LOG_DEBUG("Average rx power " << 10 * log10(Sum(*rxPsd) * 180e3) << " dB");

    // compute the SNR
    NS_LOG_DEBUG("Average SNR " << 10 * log10(Sum(*rxPsd) / Sum(*noisePsd)) << " dB");

    // print the SNR and pathloss values in the snr-trace.txt file
    std::ofstream f;
    f.open("snr-trace.txt", std::ios::out | std::ios::app);
    f << Simulator::Now().GetSeconds() << " " << 10 * log10(Sum(*rxPsd) / Sum(*noisePsd)) << " "
      << propagationGainDb << std::endl;
    f.close();
}

int
main(int argc, char* argv[])
{
    double frequency = 2125.0e6;  // operating frequency in Hz (corresponds to EARFCN 2100)
    double txPow = 49.0;          // tx power in dBm
    double noiseFigure = 9.0;     // noise figure in dB
    double distance = 10.0;       // distance between tx and rx nodes in meters
    uint32_t simTime = 1000;      // simulation time in milliseconds
    uint32_t timeRes = 10;        // time resolution in milliseconds
    std::string scenario = "Uma"; // NYUSIM propagation scenario

    Config::SetDefault("ns3::NYUChannelModel::UpdatePeriod",
                       TimeValue(MilliSeconds(1))); // update the channel at each iteration
    Config::SetDefault("ns3::NYUChannelConditionModel::UpdatePeriod",
                       TimeValue(MilliSeconds(0.0))); // do not update the channel condition

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    // create and configure the factories for the channel condition and propagation loss models
    ObjectFactory propagationLossModelFactory;
    ObjectFactory channelConditionModelFactory;
    if (scenario == "Rma")
    {
        propagationLossModelFactory.SetTypeId(NYURmaPropagationLossModel::GetTypeId());
        channelConditionModelFactory.SetTypeId(NYURmaChannelConditionModel::GetTypeId());
    }
    else if (scenario == "Uma")
    {
        propagationLossModelFactory.SetTypeId(NYUUmaPropagationLossModel::GetTypeId());
        channelConditionModelFactory.SetTypeId(NYUUmaChannelConditionModel::GetTypeId());
    }
    else if (scenario == "Umi")
    {
        propagationLossModelFactory.SetTypeId(
            NYUUmiPropagationLossModel::GetTypeId());
        channelConditionModelFactory.SetTypeId(
            NYUUmiChannelConditionModel::GetTypeId());
    }
    else if (scenario == "InH")
    {
        propagationLossModelFactory.SetTypeId(
            NYUInHPropagationLossModel::GetTypeId());
        channelConditionModelFactory.SetTypeId(
            NYUInHChannelConditionModel::GetTypeId());
    }
    else if (scenario == "InF")
    {
        propagationLossModelFactory.SetTypeId(
            NYUInFPropagationLossModel::GetTypeId());
        channelConditionModelFactory.SetTypeId(
            NYUInFChannelConditionModel::GetTypeId());
    }
    else
    {
        NS_FATAL_ERROR("Unknown scenario");
    }

    // create the propagation loss model
    m_propagationLossModel = propagationLossModelFactory.Create<NYUPropagationLossModel>();
    m_propagationLossModel->SetAttribute("Frequency", DoubleValue(frequency));
    m_propagationLossModel->SetAttribute("ShadowingEnabled", BooleanValue(false));

    // create the spectrum propagation loss model
    m_spectrumLossModel = CreateObject<NYUSpectrumPropagationLossModel>();
    m_spectrumLossModel->SetChannelModelAttribute("Frequency", DoubleValue(frequency));
    m_spectrumLossModel->SetChannelModelAttribute("Scenario", StringValue(scenario));

    // create the channel condition model and associate it with the spectrum and
    // propagation loss model
    Ptr<ChannelConditionModel> condModel =
        channelConditionModelFactory.Create<NYUChannelConditionModel>();
    m_spectrumLossModel->SetChannelModelAttribute("ChannelConditionModel", PointerValue(condModel));
    m_propagationLossModel->SetChannelConditionModel(condModel);

    // create the tx and rx nodes
    NodeContainer nodes;
    nodes.Create(2);

    // create the tx and rx devices
    Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice>();
    Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice>();

    // associate the nodes and the devices
    nodes.Get(0)->AddDevice(txDev);
    txDev->SetNode(nodes.Get(0));
    nodes.Get(1)->AddDevice(rxDev);
    rxDev->SetNode(nodes.Get(1));

    // create the tx and rx mobility models, set the positions
    Ptr<MobilityModel> txMob = CreateObject<ConstantPositionMobilityModel>();
    txMob->SetPosition(Vector(0.0, 0.0, 10.0));
    Ptr<MobilityModel> rxMob = CreateObject<ConstantPositionMobilityModel>();
    rxMob->SetPosition(Vector(distance, 0.0, 1.6));

    // assign the mobility models to the nodes
    nodes.Get(0)->AggregateObject(txMob);
    nodes.Get(1)->AggregateObject(rxMob);

    // create the antenna objects and set their dimensions
    Ptr<PhasedArrayModel> txAntenna =
        CreateObjectWithAttributes<UniformPlanarArray>("NumColumns",
                                                       UintegerValue(2),
                                                       "NumRows",
                                                       UintegerValue(2));
    Ptr<PhasedArrayModel> rxAntenna =
        CreateObjectWithAttributes<UniformPlanarArray>("NumColumns",
                                                       UintegerValue(2),
                                                       "NumRows",
                                                       UintegerValue(2));

    // set the beamforming vectors
    DoBeamforming(txDev, txAntenna, rxDev);
    DoBeamforming(rxDev, rxAntenna, txDev);

    for (int i = 0; i < floor(simTime / timeRes); i++)
    {
        ComputeSnrParams params{txMob, rxMob, txPow, noiseFigure, txAntenna, rxAntenna};
        Simulator::Schedule(MilliSeconds(timeRes * i), &ComputeSnr, params);
    }

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
