#pragma config(StandardModel, "POC")
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

const int AUTONLENGTH = 15;

const int tempRightSpeed = 0;
const int tempLeftSpeed = 1;
const int tempRightDistance = 2;
const int tempLeftDistance = 3;
const int tempHangingArmDistance = 4;
const int tempHangingArmSpeed = 5;
const int tempIntakeSpeed = 6;
const int tempBallLift = 7;

const int dataMovementCode = 0;
const int dataRightSpeed = 1;
const int dataLeftSpeed = 2;
const int dataRightDistance = 3;
const int dataLeftDistance = 4;
const int dataHangingArmDistance = 5;

const int codeWait = 000;
const int codeForward = 100;
const int codeBackward = 200;
const int codeLeftTurn = 300;
const int codeRightTurn = 400;
const int codeHangingArm = 10;
const int codeIntake = 20;
const int codeBallLift = 40;
const int codePlus = 1;
const int codeMinus = 2;

float timeRemaining;
string display;
bool firstRun = true;
int temp[8];
int data[20*AUTONLENGTH][6];
#include "lcdFunctions.h"
#include "functions.h"
#include "driverControl.h"

/*
+---------+----------------------+
| Timer # |         Usage        |
+---------+----------------------+
|    T1   |      Time Limits     |
+---------+----------------------+
|    T2   | Autonomous Timeouts  |
+---------+----------------------+
|    T3   |    Driver Timeouts   |
+---------+----------------------+
|    T4   |         Other        |
+---------+----------------------+


temp[]
+---------------+----------------+---------------+---------------+----------------------+-------------------+---------------+---------------+
| Column [x][0] |  Column [x][1] | Column [x][2] | Column [x][3] | Column [x][4]        | Column [x][5]     | Column [x][6] | Column [x][7] |
+---------------+----------------+---------------+---------------+----------------------+-------------------+---------------+---------------+
|  Right Speed  | Left Speed     | Right Distance| Left Distance | Hanging Arm Distance | Hanging Arm Speed | Intake        | Ball Lift     |
+---------------+----------------+---------------+---------------+----------------------+-------------------+---------------+---------------+

data[][]
+---------------+---------------+---------------+----------------+---------------+---------------+
| Column [x][0] | Column [x][1] | Column [x][2] | Column [x][3]  | Column [x][4] | Column [x][5] |
+---------------+---------------+---------------+----------------+---------------+---------------+
| Movement Code |  Right Speed  | Left Speed    | Right Distance | Left Distance | Arm Distance  |
+---------------+---------------+---------------+----------------+---------------+---------------+


+-------+----------------+
| Code  |      Type      |
+-------+----------------+
|  0##  |     Nothing    |
+-------+----------------+
|  1##  |     Forward    |
+-------+----------------+
|  2##  |    Backward    |
+-------+----------------+
|  3##  |    Left Turn   |
+-------+----------------+
|  4##  |   Right Turn   |
+-------+----------------+
|  #0#  |    Nothing     |
+-------+----------------+
|  #1#  |  Hanging Arm   |
+-------+----------------+
|  #2#  |     Intake     |
+-------+----------------+
|  #4#  |    Ball Lift   |
+-------+----------------+
|  ##0  |    Nothing     |
+-------+----------------+
|  ##1  |     In/Up      |
+-------+----------------+
|  ##2  |    Out/Down    |
+-------+----------------+
*/

task main()
{
	displayLCDCenteredString(0, "Autonomus");
	displayLCDCenteredString(1, "Recorder V1.0");
	wait1Msec(2000);
	clearLCD();
	displayLCDCenteredString(0, "Start");
	displayLCDCenteredString(1, "\/");
	waitPress(centerButton);
	waitRelease();
	clearLCD();
	displayLCDCenteredString(0, "Time Remaining");
	ClearTimer(T1);
	int i = -1;
	while(time1[T1] < 1000*AUTONLENGTH)
	{
		i++;
		clearLCDLine(1);
		timeRemaining = AUTONLENGTH-(time1[T1]*0.001);
		sprintf(display, "%1.1f%c", timeRemaining);
		displayLCDCenteredString(1, display);
		baseEncoderReset();
		liftArmEncoderReset();
		driverControl(false);
		temp[tempRightSpeed] = motor[backRight]; //Right Speed
		temp[tempLeftSpeed] = motor[backLeft]; //Left Speed
		temp[tempRightDistance] = nMotorEncoder[backRight]; //Right Distance
		temp[tempLeftDistance] = nMotorEncoder[backLeft]; //Left Distance
		temp[tempHangingArmDistance] = SensorValue[liftArmEncoder]; //Lift Encoder
		temp[tempHangingArmSpeed] = motor[liftTopRight]; //Lift Speed
		temp[tempIntakeSpeed] = motor[intakeRight]; //Intake Speed
		temp[tempBallLift] = SensorValue[ballLift]; //Ball Lift State
		if(abs(temp[0] - temp[2]) < 11) //Straight
		{
			if(temp[0] > 0) //Forward
			{
				data[i][dataRightSpeed] = (temp[tempRightSpeed] + temp[tempLeftSpeed])/2; //Right Speed
				data[i][dataRightDistance] = (temp[tempRightDistance] + temp[tempLeftDistance])/2; //Right Distance
				data[i][dataLeftSpeed] = data[i][dataRightSpeed]; //Left Speed
				data[i][dataLeftDistance] = data[i][dataRightDistance]; //Left Distance
				data[i][dataMovementCode] = codeForward; //Forward Code
			}
			else if(temp[0] < 0) //Backward
			{
				data[i][dataRightSpeed] = (temp[tempRightSpeed] + temp[tempLeftSpeed])/2; //Right Speed
				data[i][dataRightDistance] = (temp[tempRightDistance] + temp[tempLeftDistance])/2; //Right Distance
				data[i][dataLeftSpeed] = data[i][dataRightSpeed]; //Left Speed
				data[i][dataLeftDistance] = data[i][dataRightDistance]; //Left Distance
				data[i][dataMovementCode] = codeBackward; //Backward Code
			}
			else //Not moving
			{
				data[i][dataRightSpeed] = 0;
				data[i][dataRightDistance] = temp[tempRightDistance];
				data[i][dataLeftSpeed] = 0;
				data[i][dataLeftDistance] = temp[tempLeftDistance];
				data[i][dataMovementCode] = codeWait; //Wait Code
			}
		}
		else if((temp[tempRightSpeed] < temp[tempLeftSpeed] && temp[tempRightSpeed] > 0) || (temp[tempRightSpeed] > temp[tempLeftSpeed] && temp[tempRightSpeed] < 0)) //Right Turn
		{
			data[i][dataRightSpeed] = temp[tempRightSpeed];
			data[i][dataLeftSpeed] = temp[tempLeftSpeed];
			data[i][dataRightDistance] = temp[tempRightDistance];
			data[i][dataLeftDistance] = temp[tempLeftDistance];
			data[i][dataMovementCode] = codeRightTurn;
		}
		else if((temp[tempRightSpeed] > temp[tempLeftSpeed] && temp[tempRightSpeed] > 0) || (temp[tempRightSpeed] < temp[tempLeftSpeed] && temp[tempRightSpeed] < 0)) //Left Turn
		{
			data[i][dataRightSpeed] = temp[tempRightSpeed];
			data[i][dataLeftSpeed] = temp[tempLeftSpeed];
			data[i][dataRightDistance] = temp[tempRightDistance];
			data[i][dataLeftDistance] = temp[tempLeftDistance];
			data[i][dataMovementCode] = codeLeftTurn;
		}
		if(temp[tempHangingArmSpeed] != 0)
		{
			data[i][dataMovementCode] += codeHangingArm;
			if(temp[tempHangingArmSpeed] > 0)
			{
				data[i][dataMovementCode] += codePlus;
			}
			else
			{
				data[i][dataMovementCode] += codeMinus;
			}
		}
		if(tempIntakeSpeed != 0)
		{
			data[i][dataMovementCode] += codeIntake;
			if(temp[tempIntakeSpeed] > 0)
			{
				data[i][dataMovementCode] += codePlus;
			}
			else
			{
				data[i][dataMovementCode] += codeMinus;
			}
		}
		if(temp[tempBallLift] != 0)
		{
			data[i][dataMovementCode] += codeBallLift;
			data[i][dataMovementCode] += codePlus;
		}
		data[i][dataHangingArmDistance] = temp[tempHangingArmDistance];
		if(!firstRun)
		{
			if((data[i][dataMovementCode]/10) == (data[i-1][dataMovementCode]/10))
			{
				i--;
				data[i][dataMovementCode] = data[i+1][dataMovementCode];
				data[i][dataRightSpeed] = (data[i+1][dataRightSpeed] + data[i][dataRightSpeed]) / 2;
				data[i][dataLeftSpeed] = (data[i+1][dataLeftSpeed] + data[i][dataLeftSpeed]) / 2;
			}
			data[i][dataRightDistance] += data[i+1][dataRightDistance];
			data[i][dataLeftDistance] += data[i+1][dataLeftDistance];
			data[i][dataHangingArmDistance] += data[i+1][dataHangingArmDistance];
		}
		else
		{
			firstRun = false;
		}
	}

	for(int i=1;i<AUTONLENGTH*20;i++)
	{
		data[i][dataRightDistance] = 127;
		data[i][dataMovementCode] = codeForward+1;
		data[i][dataRightSpeed] = 63;
		if((data[i][dataMovementCode]/10)*10 == codeForward)
		{
			writeDebugStreamLine("");
			writeDebugStream("forward(");
			writeDebugStream("%1.0f%c", data[i][dataRightSpeed]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataRightDistance]);
			writeDebugStream(");");
		}
		else if((data[i][dataMovementCode]/10)*10 == codeBackward)
		{
			writeDebugStreamLine("");
			writeDebugStream("backward(");
			writeDebugStream("%1.0f%c", data[i][dataRightSpeed]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataRightDistance]);
			writeDebugStream(");");
		}
		else if((data[i][dataMovementCode]/10)*10 == codeLeftTurn)
		{
			writeDebugStreamLine("");
			writeDebugStream("leftTurn(");
			writeDebugStream("%1.0f%c", data[i][dataRightSpeed]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataLeftSpeed]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataRightDistance]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataLeftDistance]);
			writeDebugStream(");");
		}
		else if((data[i][dataMovementCode]/10)*10 == codeRightTurn)
		{
			writeDebugStreamLine("");
			writeDebugStream("rightTurn(");
			writeDebugStream("%1.0f%c", data[i][dataRightSpeed]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataLeftSpeed]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataRightDistance]);
			writeDebugStream(", ");
			writeDebugStream("%1.0f%c", data[i][dataLeftDistance]);
			writeDebugStream(");");
		}
	}
}
