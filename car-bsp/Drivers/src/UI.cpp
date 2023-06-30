/*
 * UI.cpp
 *
 *  Created on: Jan 12, 2022
 *      Author: John Carr
 */

#include <UI.hpp>


namespace SolarGators {
namespace Drivers {

void InfoSquare::Draw(ILI9341& disp)
{
  disp.SetTextSize(TextSize);
  // Draw the Border
  disp.DrawRect(x, y, DataSqW, DataSqH, UI::color_neutral_);
  // Draw the Title
  disp.DrawText(x+TextPaddX, y+TitlePaddY, title.c_str(), UI::color_neutral_);
  etl::string<5> tmp(" N/A ");
  UpdateValue(disp, tmp);
}

void InfoSquare::UpdateValue(ILI9341& disp, etl::string<5>& val)
{
  disp.SetTextSize(TextSize);
  disp.DrawText(x+TextPaddX, TextHeight*TextSize+TitlePaddY+DataPaddY+y, val.c_str(), UI::color_neutral_);
}

UI::UI(ILI9341& display):disp(display)
{
  // Set Screen Orientation
  disp.SetRotation(1);
  // Set Background Color
  disp.ClearScreen(0x0000);
  // Setup info squares
  etl::string<5> titles[] = {"Vsup","Psup","Voltg","Pmain"};
  for (size_t i = 0; i < first_row_.size(); ++i)
  {
    first_row_[i].title = titles[i];
    first_row_[i].x = i*InfoSquare::DataSqW;
    first_row_[i].y = 0;
    first_row_[i].Draw(disp);
  }
  DrawSpeed();
  DrawTripCodes();
}

UI::~UI()
{

}

void UI::UpdateSquare(uint8_t num, etl::string<5>& val)
{
  if(num < 4)
  {
    first_row_[num].UpdateValue(disp, val);
  }
}

void UI::DrawSpeed()
{
  // Draw Speed
  disp.SetTextSize(3);
  const char* str2 = "Speed";
  disp.DrawText(115, 54, str2, color_neutral_);
  disp.SetTextSize(4);
  UpdateSpeed(99.9);
}

void UI::UpdateSpeed(float speed)
{
  // Draw Speed
  disp.SetTextSize(3);
  etl::string<4> s_speed;
  etl::to_string(speed, s_speed, etl::format_spec().precision(1).width(4).fill(0), false);
  disp.DrawText(112+9, 82, s_speed.c_str(), color_neutral_);
}

void UI::UpdateBMSTrip(SolarGators::DataModules::OrionBMSRx4* bmsCodes) {
	// BMS Trip Codes
	const char* bms_faults_messages[] = {
		"Internal Cell Communication",
		"Cell Balancing Stuck Off",
		"Weak Cell",
		"Low Cell Voltage",
		"Cell Open Wiring",
		"Current Sensor",
		"Cell Voltage Over 5v",
		"Cell Bank",
		"Weak Pack",
		"Fan Monitor",
		//"Thermistor",
		"Can Communication",
		"Redundant Power Supply",
		"High Voltage Isolation",
		"Invalid Input Supply Voltage",
		"Charge En Relay",
		"Discharge En Relay",
		"Charger Safety Relay",
		"Internal Hardware",
		"Internal Heatsink Thermistor",
		"Internal Logic",
		"Highest Cell Voltage Too High",
		"Lowest Cell Voltage Too Low",
		"Pack Too Hot"
	};

	bool bms_faults_values[24] = {
		bmsCodes->isInternalCellCommunicationFault(),
		bmsCodes->isCellBalancingStuckOffFault(),
		bmsCodes->isWeakCellFault(),
		bmsCodes->isLowCellVoltageFault(),
		bmsCodes->isCellOpenWiringFault(),
		bmsCodes->isCurrentSensorFault(),
		bmsCodes->isCellVoltageOver5vFault(),
		bmsCodes->isCellBankFault(),
		bmsCodes->isWeakPackFault(),
		bmsCodes->isFanMonitorFault(),
		//bmsCodes->isThermistorFault(),
		bmsCodes->isCanCommunicationFault(),
		bmsCodes->isRedundantPowerSupplyFault(),
		bmsCodes->isHighVoltageIsolationFault(),
		bmsCodes->isInvalidInputSupplyVoltageFault(),
		bmsCodes->isChargeenableRelayFault(),
		bmsCodes->isDischargeenableRelayFault(),
		bmsCodes->isChargerSafetyRelayFault(),
		bmsCodes->isInternalHardwareFault(),
		bmsCodes->isInternalHeatsinkThermistorFault(),
		bmsCodes->isInternalLogicFault(),
		bmsCodes->isHighestCellVoltageTooHighFault(),
		bmsCodes->isLowestCellVoltageTooLowFault(),
		bmsCodes->isPackTooHotFault()
	};

	disp.SetTextSize(2);

	//Get the first trip in the list
	etl::string<40> bms_fault = nullptr;
	for (int i = 0; i < 24; i++) {
		if (bms_faults_values[i]) {
			bms_fault = bms_faults_messages[i];
			break;
		}
	}


    if(bms_fault.size() != 0)
    {
        disp.DrawText(0, 140, bms_fault.c_str(), color_fail_);
    }
    else {
    	etl::string<40> ok = "OK";
    	disp.DrawText(0, 140, ok.c_str(), color_ok_);
    }

}

void UI::UpdateMitsubaTrip(SolarGators::DataModules::MitsubaRx2* mitsubaCodes) {
	// Mitsuba Trip Codes
	const char* mitsuba_faults_messages[] = {
	  "AD Sensor Error",
	  "Motor Curr Sensor U Error",
	  "Motor Curr Sensor W Error",
	  "Fet Therm Error",
	  "Batt Volt Sensor Error",
	  "Batt Curr Sensor Error",
	  "Batt Curr Sensor Adj Error",
	  "Motor Curr Sensor Adj Error",
	  "Accel Pos Error",
	  "Cont Volt Sensor Error",
	  "Power System Error",
	  "Over Curr Error",
	  "Over Volt Error",
	  "Over Curr Limit",
	  "Motor System Error",
	  "Motor Lock",
	  "Hall Sensor Short",
	  "Hall Sensor Open"
	};
	bool mitsuba_faults_values[18] = {
			mitsubaCodes->GetAdSensorError(),
			mitsubaCodes->GetMotorSensorUError(),
			mitsubaCodes->GetMotorCurrSensorWError(),
			mitsubaCodes->GetFetThermError(),
			mitsubaCodes->GetBattVoltSensorError(),
			mitsubaCodes->GetBattCurrSensorError(),
			mitsubaCodes->GetBattCurrSensorAdjError(),
			mitsubaCodes->GetMotorCurrSensorAdjError(),
			mitsubaCodes->GetAccelPosError(),
			mitsubaCodes->GetContVoltSensorError(),
			mitsubaCodes->GetPowerSystemError(),
			mitsubaCodes->GetOverCurrError(),
			mitsubaCodes->GetOverVoltError(),
			mitsubaCodes->GetOverCurrLimit(),
			mitsubaCodes->GetMotorSystemError(),
			mitsubaCodes->GetMotorLock(),
			mitsubaCodes->GetHallSensorShort()
	};
	disp.SetTextSize(2);
	//Get the first trip in the list
	const char* mitsuba_fault = nullptr;
	for (int i = 0; i < 18; i++) {
		if (mitsuba_faults_values[i]) {
			mitsuba_fault = mitsuba_faults_messages[i];
			break;
		}
	}

	if(mitsuba_fault != nullptr) {
		disp.DrawText(0, 200, mitsuba_fault, color_fail_);
	}
	else {
		disp.DrawText(0, 200, "OK", color_ok_);
	}
}

void UI::DrawTripCodes()
{
    disp.SetTextSize(2);
    disp.DrawText(0, 120, "BMS Status: ", color_neutral_);
    disp.DrawText(0, 180, "MC Status: ", color_neutral_);
}

} /* namespace Drivers */
} /* namespace SolarGators */
