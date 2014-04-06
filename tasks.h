#pragma systemFile

task liftArmLimiter()
{
	while(true)
	{
		lift.location = liftEncoder();
		if(SensorBoolean[liftDown])
		{
			writeDebugStreamLine("INFO: Lift Arm Stopped at Low at \"liftArmLimiter\"");
			if(motor[liftTopRight] < 0)
			{
				stopLiftArm();
				writeDebugStreamLine("INFO: Lift Arm Still Moving Down, Jamming at \"liftArmLimiter\"");
			}
			liftArmEncoderReset();
			lift.isDown = true;
		}
		else if(lift.location > lift.maxHeight)
		{
			writeDebugStreamLine("INFO: Lift Arm Stopped at High at \"liftArmLimiter\"");
			if(motor[liftTopRight] > 0)
			{
				stopLiftArm();
				writeDebugStreamLine("INFO: Lift Arm Still Moving Up, Jamming at \"liftArmLimiter\"");
			}
			startLiftArm(-127);
			lift.isMax = true;
		}
		if(lift.isMax && liftEncoder() < lift.maxHeight)
		{
			lift.isMax = false;
		}
		if(lift.isDown && !SensorBoolean[liftDown])
		{
			lift.isDown = false;
		}
		wait1Msec(10);
	}
}

task liftArmToHangPos()
{
	if(liftEncoder() < lift.maxHeight)
	{
		while(liftEncoder() < lift.maxHeight)
		{
			startLiftArm(127);
			wait10Msec(10);
		}
	}
	else if(liftEncoder() > lift.maxHeight)
	{
		writeDebugStreamLine("ERROR: Lift Arm Higher Than Expected at \"liftArmToHangPos\"");
	}
}

task liftArmToDown()
{
	writeDebugStreamLine("INFO: \"liftArmToDown\" Task Started");
	liftBtnUpLastState = 1;
	liftBtnDownLastState = 1;
	while(!liftArmButtonEvent() && !SensorBoolean[liftDown])
	{
		startLiftArm(-127);
		wait10Msec(10);
	}
	stopLiftArm();
	writeDebugStreamLine("INFO: \"liftArmToDown\" Task Done");
}

task liftArmPID()
{
	writeDebugStreamLine("INFO: \"liftArmPID\" Task Started");
	int lastRequestedLocation;
	while(true)
	{
		if(lift.isauton && lift.isPIDon)
		{
				lift.error = lift.requestedLocation - liftEncoder();
				startLiftArm(clamp((lift.error*lift.kp) + ((lift.error - lift.lastError)*lift.kd)));
				lift.lastError = lift.error;
				wait10Msec(10);
		}
		else if(!lift.isauton && lift.isPIDon)
		{
			lift.requestedLocation = liftEncoder();
			while(lift.isPIDon)
			{
				lift.error = lift.requestedLocation - liftEncoder();
				startLiftArm(clamp((lift.error*lift.kp) + ((lift.error - lift.lastError)*lift.kd)));
				lift.lastError = lift.error;
				wait10Msec(10);
			}
		}
		wait10Msec(10);
	}
}
