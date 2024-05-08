/*
 * CustomBMS.cpp
 *
 * Created on: April 4, 2024
 * Author: Matthew Shen & Yash Bhat
 */

#include "CustomBMS.hpp"


namespace SolarGators::DataModules {

// BMSFrame0 Implementation
CustomBMSRx0::CustomBMSRx0(uint32_t can_id) : DataModule(can_id, 0, this->Size, 0, false) {}

void CustomBMSRx0::ToByteArray(uint8_t* buff) const {
    buff[0] = pack_voltage_ >> 8;
    buff[1] = pack_voltage_ & 0xFF;
    buff[2] = avg_cell_voltage_ >> 8;
    buff[3] = avg_cell_voltage_ & 0xFF;
    buff[4] = high_cell_voltage_ >> 8;
    buff[5] = high_cell_voltage_ & 0xFF;
    buff[6] = low_cell_voltage_ >> 8;
    buff[7] = low_cell_voltage_ & 0xFF;
}

void CustomBMSRx0::FromByteArray(uint8_t* buff) {
    // Voltage is in V * 1e-1
    pack_voltage_ = (static_cast<uint16_t>(buff[1]) << 8) | buff[0];
    // Voltage in mV for remaining values
    avg_cell_voltage_ = (static_cast<uint16_t>(buff[3]) << 8) | buff[2];
    high_cell_voltage_ = (static_cast<uint16_t>(buff[5]) << 8) | buff[4];
    low_cell_voltage_ = (static_cast<uint16_t>(buff[7]) << 8) | buff[6];
}

uint16_t CustomBMSRx0::GetPackVoltage() const {
    return pack_voltage_;
}

uint16_t CustomBMSRx0::GetAvgCellVoltage() const {
    return avg_cell_voltage_;
}

uint16_t CustomBMSRx0::GetHighCellVoltage() const {
    return high_cell_voltage_;
}

uint16_t CustomBMSRx0::GetLowCellVoltage() const {
    return low_cell_voltage_;
}

#ifdef IS_TELEMETRY
  void CustomBMSRx0::PostTelemetry(PythonScripts* scripts){
	PythonHttp http;
	http.init();
	http.addData("low_cell_volt_", GetLowCellVoltage());
	http.addData("high_cell_volt_", GetHighCellVoltage());
	http.addData("avg_cell_volt_", GetAvgCellVoltage());
	http.addData("pack_sum_volt_", GetPackVoltage());
	scripts->send("bms/rx0", http.getParameters());
	http.flush();
  }
#endif
// BMSFrame1 Implementation
CustomBMSRx1::CustomBMSRx1(uint32_t can_id) : DataModule(can_id, 0, this->Size, 0, false) {}

void CustomBMSRx1::ToByteArray(uint8_t* buff) const {
    buff[0] = pack_current_ >> 8;
    buff[1] = pack_current_ & 0xFF;
    buff[2] = integral_current_ >> 8;
    buff[3] = integral_current_ & 0xFF;
    buff[4] = average_power_ >> 8;
    buff[5] = average_power_ & 0xFF;
    buff[6] = high_cell_voltage_id_;
    buff[7] = low_cell_voltage_id_;
}

void CustomBMSRx1::FromByteArray(uint8_t* buff) {
    pack_current_ = (static_cast<int16_t>(buff[1]) << 8) | buff[0];
    integral_current_ = (static_cast<int16_t>(buff[3]) << 8) | buff[2];
    average_power_ = (static_cast<int16_t>(buff[5]) << 8) | buff[4];
    high_cell_voltage_id_ = buff[6];
    low_cell_voltage_id_ = buff[7];
}

int16_t CustomBMSRx1::GetPackCurrent() const {
    //Current is in A * 1e-2
    return pack_current_;
}

int16_t CustomBMSRx1::GetIntegralCurrent() const {
    //Charge is in uAh
    return integral_current_;
}

int16_t CustomBMSRx1::GetAveragePower() const {
    //Power in W
    return average_power_;
}

uint8_t CustomBMSRx1::GetHighCellVoltageID() const {
    return high_cell_voltage_id_;
}

uint8_t CustomBMSRx1::GetLowCellVoltageID() const {
    return low_cell_voltage_id_;
}
#ifdef IS_TELEMETRY
    void CustomBMSRx1::PostTelemetry(PythonScripts* scripts){
	PythonHttp http;
	http.init();
	http.addData("pack_current_", GetPackCurrent());
	scripts->send("bms/rx2", http.getParameters());
	http.flush();
    }
}
#endif
