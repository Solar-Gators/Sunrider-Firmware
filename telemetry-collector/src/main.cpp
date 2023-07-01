#include <map>
#include <iostream>
#include <sstream>
#include <csignal>
#include "DataLinkRecive.hpp"
#include "PythonScripts.hpp"
#include "NetworkReceive.hpp"
#include "DataModule.hpp"
#include "Mitsuba.hpp"
#include "Mitsuba.hpp"
#include "OrionBMS.hpp"
#include "PowerBoard.hpp"
#include "DataModuleInfo.hpp"
#include "Mppt.hpp"
#include "GPS.hpp"
#include "crc16.hpp"
#include "Logger.hpp"

SolarGators::DataModules::GPS GPS_Rx_0;

SolarGators::DataModules::MitsubaRx0 MitsubaRx0(SolarGators::DataModuleInfo::MOTORRX0_RL_MSG_ID, 0);
SolarGators::DataModules::MitsubaRx1 MitsubaRx1(SolarGators::DataModuleInfo::MOTORRX1_RL_MSG_ID, 0);
SolarGators::DataModules::MitsubaRx2 MitsubaRx2(SolarGators::DataModuleInfo::MOTORRX2_RL_MSG_ID, 0);

SolarGators::DataModules::OrionBMSRx0 OrionBMSRx0(SolarGators::DataModuleInfo::BMS_RX0_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx1 OrionBMSRx1(SolarGators::DataModuleInfo::BMS_RX1_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx2 OrionBMSRx2(SolarGators::DataModuleInfo::BMS_RX2_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx3 OrionBMSRx3(SolarGators::DataModuleInfo::BMS_RX3_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx4 OrionBMSRx4(SolarGators::DataModuleInfo::BMS_RX4_MSG_ID, 0);
SolarGators::DataModules::OrionBMSRx5 OrionBMSRx5(SolarGators::DataModuleInfo::BMS_RX5_MSG_ID, 0);

//registering mppt data modules
//there are several more modules than this, but i only want to register essential info from each
SolarGators::DataModules::Mpptx0 MPPT0_Rx_0(SolarGators::DataModuleInfo::MPPT0_RX0_MSG_ID);
SolarGators::DataModules::Mpptx0 MPPT1_Rx_0(SolarGators::DataModuleInfo::MPPT1_RX0_MSG_ID);
SolarGators::DataModules::Mpptx0 MPPT2_Rx_0(SolarGators::DataModuleInfo::MPPT2_RX0_MSG_ID);

SolarGators::DataModules::Mpptx1 MPPT0_Rx_1(SolarGators::DataModuleInfo::MPPT0_RX1_MSG_ID);
SolarGators::DataModules::Mpptx1 MPPT1_Rx_1(SolarGators::DataModuleInfo::MPPT1_RX1_MSG_ID);
SolarGators::DataModules::Mpptx1 MPPT2_Rx_1(SolarGators::DataModuleInfo::MPPT2_RX1_MSG_ID);

SolarGators::DataModules::Mpptx5 MPPT0_Rx_5(SolarGators::DataModuleInfo::MPPT0_RX5_MSG_ID);
SolarGators::DataModules::Mpptx5 MPPT1_Rx_5(SolarGators::DataModuleInfo::MPPT1_RX5_MSG_ID);
SolarGators::DataModules::Mpptx5 MPPT2_Rx_5(SolarGators::DataModuleInfo::MPPT2_RX5_MSG_ID);
//SolarGators::DataModules::PowerBoard PBoard(SolarGators::DataModuleInfo::POWER_BOARD_ID, 0);

// SolarGators::DataModules::Flight Flight(SolarGators::DataModuleInfo::FRONT_LIGHTS_ID, 0);


void signalHandler( int signum ) {
   exit(0);
}


int main(int argc, char *argv[]) {
    signal(SIGTERM, signalHandler);

    PythonScripts scripts;
    scripts.init();

    DataLinkReceive dataLink;
    dataLink.init();

    std::map<int, SolarGators::DataModules::DataModule*> modules;


    // Motor Drivers
    modules.insert(std::make_pair(MitsubaRx0.can_id_, &MitsubaRx0));
    modules.insert(std::make_pair(MitsubaRx1.can_id_, &MitsubaRx1));
    modules.insert(std::make_pair(MitsubaRx2.can_id_, &MitsubaRx2));

    // BMS
    modules.insert(std::make_pair(OrionBMSRx0.can_id_, &OrionBMSRx0));
    modules.insert(std::make_pair(OrionBMSRx1.can_id_, &OrionBMSRx1));
    modules.insert(std::make_pair(OrionBMSRx2.can_id_, &OrionBMSRx2));
    modules.insert(std::make_pair(OrionBMSRx3.can_id_, &OrionBMSRx3));
    modules.insert(std::make_pair(OrionBMSRx4.can_id_, &OrionBMSRx4));
    modules.insert(std::make_pair(OrionBMSRx5.can_id_, &OrionBMSRx5));

    // MPPTs
    modules.insert(std::make_pair(MPPT0_Rx_0.can_id_, &MPPT0_Rx_0));
    modules.insert(std::make_pair(MPPT1_Rx_0.can_id_, &MPPT1_Rx_0));
    modules.insert(std::make_pair(MPPT2_Rx_0.can_id_, &MPPT2_Rx_0));

    modules.insert(std::make_pair(MPPT0_Rx_1.can_id_, &MPPT0_Rx_1));
    modules.insert(std::make_pair(MPPT1_Rx_1.can_id_, &MPPT1_Rx_1));
    modules.insert(std::make_pair(MPPT2_Rx_1.can_id_, &MPPT2_Rx_1));

    modules.insert(std::make_pair(MPPT0_Rx_5.can_id_, &MPPT0_Rx_5));
    modules.insert(std::make_pair(MPPT1_Rx_5.can_id_, &MPPT1_Rx_5));
    modules.insert(std::make_pair(MPPT2_Rx_5.can_id_, &MPPT2_Rx_5));

    // GPS
    modules.insert(std::make_pair(GPS_Rx_0.can_id_, &GPS_Rx_0));


    // modules.insert(stdd::make_pair(Flight.can_id_, &Flight));

    //need review here?
    //modules.insert(std::make_pair(PBoard.can_id_, &PBoard));

    NetworkReceive network;
    while (1) {
        u_int8_t data = scripts.readUART();
        if (!dataLink.read(data)) {
            continue;
        }
        network.fromByteArray(dataLink.buffer);
        uint16_t crc = SolarGators::Helpers::crc16(network.data, network.size);
        if (crc != network.crc) {
            Logger::info("Error: CRC did not match, skipping.\n");
            continue;
        }

        //Get module
        if (modules.count(network.can_id) > 0) {
            SolarGators::DataModules::DataModule* rx_module = (*modules.find(network.can_id)).second;
            rx_module->FromByteArray(network.data);
            rx_module->PostTelemetry(&scripts);
        }
        dataLink.flush();
    }

    return 0;
}
