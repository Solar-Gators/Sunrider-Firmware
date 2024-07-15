/*
 * CustomBMS.hpp
 *
 * Created on: April 4, 2024
 * Author: Matthew Shen & Yash Bhat
 */

#ifndef SOLARGATORSBSP_DATAMODULES_INC_CUSTOMBMS_HPP_
#define SOLARGATORSBSP_DATAMODULES_INC_CUSTOMBMS_HPP_

#include <DataModule.hpp>

namespace SolarGators::DataModules {

class CustomBMSRx0 final: public DataModule {
public:
    CustomBMSRx0(uint32_t can_id);
    ~CustomBMSRx0() {};

    void ToByteArray(uint8_t* buff) const;
    void FromByteArray(uint8_t* buff);
    #ifdef IS_TELEMETRY
    void PostTelemetry(PythonScripts* scripts);
    #endif

    float GetPackVoltage() const;
    float GetAvgCellVoltage() const;
    float GetHighCellVoltage() const;
    float GetLowCellVoltage() const;

    static constexpr uint8_t Size = 8;
protected:
    uint16_t pack_voltage_;
    uint16_t avg_cell_voltage_;
    uint16_t high_cell_voltage_;
    uint16_t low_cell_voltage_;
};

class CustomBMSRx1 final: public DataModule {
public:
    CustomBMSRx1(uint32_t can_id);
    ~CustomBMSRx1() {};

    void ToByteArray(uint8_t* buff) const;
    void FromByteArray(uint8_t* buff);
    #ifdef IS_TELEMETRY
    void PostTelemetry(PythonScripts* scripts);
    #endif

    float GetPackCurrent() const;
    float GetIntegralCurrent() const;
    float GetAveragePower() const;
    uint8_t GetHighCellVoltageID() const;
    uint8_t GetLowCellVoltageID() const;

    static constexpr uint8_t Size = 8;
protected:
    int16_t pack_current_;
    int16_t integral_current_;
    int16_t average_power_;
    uint8_t high_cell_voltage_id_;
    uint8_t low_cell_voltage_id_;
};

class CustomBMSRx2 final: public DataModule {
public:
    CustomBMSRx2(uint32_t can_id);
    ~CustomBMSRx2() {};

    void ToByteArray(uint8_t* buff) const;
    void FromByteArray(uint8_t* buff);
    #ifdef IS_TELEMETRY
    void PostTelemetry(PythonScripts* scripts);
    #endif

    float GetHighTemp() const;
    float GetLowTemp() const;
    uint8_t GetHighTempCellID() const;
    uint8_t GetLowTempCellID() const;
    uint16_t GetInternalTemp() const;

    static constexpr uint8_t Size = 8;
protected:
    uint16_t high_temp_;
    uint16_t low_temp_;
    uint8_t high_temp_cell_id_;
    uint8_t low_temp_cell_id_;
    uint16_t internal_temp_;
};

class CustomBMSRx3 final: public DataModule {
public:
    CustomBMSRx3(uint32_t can_id);
    ~CustomBMSRx3() {};

    void ToByteArray(uint8_t* buff) const;
    void FromByteArray(uint8_t* buff);
    #ifdef IS_TELEMETRY
    void PostTelemetry(PythonScripts* scripts);
    #endif

    uint8_t GetFaultFlags() const;
    bool GetLowCellVoltageFault() const;
    bool GetHighCellVoltageFault() const;
    bool GetHighDischargeCurrentFault() const;
    bool GetHighChargeCurrentFault() const;
    bool GetHighTempFault() const;
    bool GetThermistorDisconnectedFault() const;
    bool GetCurrentSensorDisconnectedFault() const;
    bool GetKillSwitchPressedFault() const;
    uint8_t GetStatusFlags() const;
    uint16_t GetPackSoC() const;

    static constexpr uint8_t Size = 5;
protected:
    uint8_t fault_flags_;
    uint8_t status_flags_;
    uint16_t pack_soc_;
};

// Additional classes for BMSSecondaryFrame0 to BMSSecondaryFrame3 can be defined here following the same pattern.

}

#endif /* CUSTOMBMS_HPP_ */
