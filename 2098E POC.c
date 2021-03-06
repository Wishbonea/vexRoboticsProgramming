#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    powerExpander,  sensorAnalog)
#pragma config(Sensor, in4,    lineFarRight,   sensorLineFollower)
#pragma config(Sensor, in5,    lineFarLeft,    sensorLineFollower)
#pragma config(Sensor, in6,    lineRight,      sensorLineFollower)
#pragma config(Sensor, in7,    lineMiddle,     sensorLineFollower)
#pragma config(Sensor, in8,    lineLeft,       sensorLineFollower)
#pragma config(Sensor, dgtl1,  liftArmEncoder, sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  liftDown,       sensorTouch)
#pragma config(Sensor, dgtl5,  middleRightBackup, sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  middleLeftBackup, sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  isRedAlliance,  sensorTouch)
#pragma config(Sensor, dgtl10, isHangingZone,  sensorTouch)
#pragma config(Sensor, dgtl11, isAutonTwo,     sensorTouch)
#pragma config(Sensor, dgtl12, isIMEFailure,   sensorTouch)
#pragma config(Sensor, I2C_1,  frontRightIME,  sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  backRightIME,   sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_3,  backLeftIME,    sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_4,  frontLeftIME,   sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           frontRight,    tmotorVex393HighSpeed, openLoop, reversed, encoder, encoderPort, I2C_1, 1000)
#pragma config(Motor,  port2,           backRight,     tmotorVex393HighSpeed, openLoop, reversed, encoder, encoderPort, I2C_2, 1000)
#pragma config(Motor,  port3,           intakeRight,   tmotorVex393HighSpeed, openLoop)
#pragma config(Motor,  port4,           liftTopRight,  tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port5,           liftBottomRight, tmotorVex393, openLoop)
#pragma config(Motor,  port6,           liftBottomLeft, tmotorVex393, openLoop)
#pragma config(Motor,  port7,           liftTopLeft,   tmotorVex393, openLoop, reversed)
#pragma config(Motor,  port8,           intakeLeft,    tmotorVex393HighSpeed, openLoop, reversed)
#pragma config(Motor,  port9,           frontLeft,     tmotorVex393HighSpeed, openLoop, encoder, encoderPort, I2C_4, 0)
#pragma config(Motor,  port10,          backLeft,      tmotorVex393HighSpeed, openLoop, reversed, encoder, encoderPort, I2C_3, 0)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "globalVariables.h"
#include "lcdFunctions.h"
#include "functions.h"
#include "autonomousFunctions.h"
#include "PIDTasks.h"
#include "tasks.h"
#include "autonomousSelector.h"
#include "autonomous.h"
#include "driverControl.h"
#include "programmingSkills.h"


#pragma platform(VEX)
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)
#include "Vex_Competition_Includes.c"

void pre_auton()
{
	writeDebugStreamLine("INFO: Pre_Auton Started");
	if(SensorBoolean[isIMEFailure])
	{
		writeDebugStreamLine("INFO: IME Failure, Switching to QE");
	}
	if(bResetFromWatchdogTimeout)
	{
		writeDebugStreamLine("ERROR: Watchdogtimeout Caused Restart");
	}
	clearLCD();
	bLCDBacklight = true;
	displayLCDCenteredString(0, "Auton Select?");
	if((!(nVexRCReceiveState & vrCompetitionSwitch) || bIfiRobotDisabled  || nLCDButtons == leftButton || nLCDButtons == centerButton || nLCDButtons == rightButton) && (!bResetFromWatchdogTimeout  || nLCDButtons == leftButton || nLCDButtons == centerButton || nLCDButtons == rightButton))
	{
		waitRelease();
		writeDebugStreamLine("INFO: Starting Battery Check");
		batteryCheck();
		writeDebugStreamLine("INFO: Battery Check Done");
		writeDebugStreamLine("INFO: Starting Autonomous Selector");
		//autonNumber = autonomousSelector();
		writeDebugStreamLine("INFO: Autonomous Selector Returned %d", autonNumber);
	}
	resetEveryThing();
	liftValues();
	bStopTasksBetweenModes = true;
	bLCDBacklight = false;
	writeDebugStreamLine("INFO: Pre_Auton Done");
}

task autonomous()
{
	writeDebugStreamLine("INFO: Autonomous Started");
	StartTask(liftArmPID);
	baseEncoderReset();
	autonNumber = autonEncode(SensorBoolean[isRedAlliance], !SensorBoolean[isRedAlliance], SensorBoolean[isHangingZone], !SensorBoolean[isHangingZone], (SensorValue[isAutonTwo] + 1));
	auton(autonNumber);
	writeDebugStreamLine("INFO: Autonomous Done");
}

task usercontrol()
{
	lift.requestedLocation = liftEncoder();
	StartTask(liftArmPID);
	StartTask(liftControl);
	writeDebugStreamLine("INFO: Driver_Control Started");
	driverControl(true);
}
