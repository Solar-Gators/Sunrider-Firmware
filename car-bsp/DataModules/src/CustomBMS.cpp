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
	//pack current exists, needs to move to rx1 frame though
	http.addData("pack_current_", GetPackCurrent());
	http.addData("integral_current_", GetIntegralCurrent());
	http.addData("avg_power_", GetAveragePower());
	http.addData("high_cell_voltage_id_", GetHighCellVoltageID());
	http.addData("low_cell_voltage_id_", GetLowCellVoltageID());
	scripts->send("bms/rx1", http.getParameters());
	http.flush();
    }

#endif

//BMSFrame2: Implementation
CustomBMSRx2::CustomBMSRx2(uint32_t can_id) : DataModule(can_id, 0, this->Size, 0, false) {}

void CustomBMSRx2::ToByteArray(uint8_t* buff) const {
    buff[0] = high_temp_ >> 8;
    buff[1] = high_temp_ & 0xFF;
    buff[2] = low_temp_ >> 8;
    buff[3] = low_temp_ & 0xFF;
    buff[4] = high_temp_cell_id_;
    buff[5] = low_temp_cell_id_;
    buff[6] = internal_temp_ >> 8;
    buff[7] = internal_temp_ & 0xFF;
}

void CustomBMSRx2::FromByteArray(uint8_t* buff) {
    high_temp_ = (static_cast<uint16_t>(buff[0]) << 8) | buff[1];
    low_temp_ = (static_cast<uint16_t>(buff[2]) << 8) | buff[3];
    high_temp_cell_id_ = buff[4];
    low_temp_cell_id_ = buff[5];
    internal_temp_ = (static_cast<uint16_t>(buff[6]) << 8) | buff[7];
}
//temp in C/100
uint16_t CustomBMSRx2::GetHighTemp() const{
	return high_temp_;
}
//temp in C/100
uint16_t CustomBMSRx2::GetLowTemp() const {
	return low_temp_;
}

uint8_t CustomBMSRx2::GetHighTempCellID() const {
	return high_temp_cell_id_;
}

uint8_t CustomBMSRx2::GetLowTempCellID() const {
	return low_temp_cell_id_;
}

uint16_t CustomBMSRx2::GetInternalTemp() const {
	return internal_temp_;
}

#ifdef IS_TELEMETRY
    void CustomBMSRx2::PostTelemetry(PythonScripts* scripts){
	PythonHttp http;
	http.init();
	http.addData("high_temp_", GetHighTemp());
	http.addData("high_temp_id_", GetHighTempCellID());
	http.addData("low_temp_", GetLowTemp());
	http.addData("low_temp_id_", GetLowTempCellID());
	http.addData("internal_temp_", GetInternalTemp());
	scripts->send("bms/rx2", http.getParameters());
}
#endif


//BMSFrame3: Implementation
CustomBMSRx3::CustomBMSRx3(uint32_t can_id) : DataModule(can_id, 0, this->Size, 0, false) {}

void CustomBMSRx3::FromByteArray(uint8_t* buff){
    fault_flags_ = buff[0];
    status_flags_ = buff[1];
    pack_soc_ = (static_cast<uint16_t>(buff[3]) << 8) | buff[4];
}

bool CustomBMSRx3::GetLowCellVoltageFault() const {
    return (fault_flags_ && 0b1);
}

bool CustomBMSRx3::GetHighCellVoltageFault() const {
    return (fault_flags_ && 0b10);
}

bool CustomBMSRx3::GetHighDischargeCurrentFault() const {
    return (fault_flags_ && 0b100);
}

bool CustomBMSRx3::GetHighChargeCurrentFault() const {
    return (fault_flags_ && 0b1000);
}

bool CustomBMSRx3::GetHighTempFault() const {
    return (fault_flags_ && 0b10000);
} 

bool CustomBMSRx3::GetThermistorDisconnectedFault() const {
    return (fault_flags_ && 0b100000);
}

bool CustomBMSRx3::GetCurrentSensorDisconnectedFault() const {
    return (fault_flags_ && 0b1000000);
}

bool CustomBMSRx3::GetKillSwitchPressedFault() const {
    return (fault_flags_ && 0b10000000);
}

