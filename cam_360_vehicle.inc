/*

	Author: Saibot

*/

#if defined _cam_360_vehicle_included
	#endinput
#endif
#define _cam_360_vehicle_included


const
	CAM_360_VEH_MIN_VEHICLE_MODELID	= 400,
	CAM_360_VEH_MAX_VEHICLE_MODELID	= 611;

static gCam360VehDefaultModelSpawn[] =
{
	400,
	401,
	411,
	418,
	421,
	423,
	424,
	434
};

static
	gCam360VehPlayerTimer[MAX_PLAYERS],
	Float:gCam360VehPlayerCoordX[MAX_PLAYERS],
	Float:gCam360VehPlayerCoordY[MAX_PLAYERS],
	Float:gCam360VehPlayerCoordZ[MAX_PLAYERS],
	IteratorArray:Cam360VehiclesID[MAX_PLAYERS]<MAX_VEHICLES>,
	Iterator:Cam360VehiclesModels<CAM_360_VEH_MAX_VEHICLE_MODELID + 1>;

public OnGameModeInit()
{
	Iter_Init(Cam360VehiclesID);

	for (new i; i < sizeof(gCam360VehDefaultModelSpawn); i++)
	{
		cam360veh_SetModelsInArray(gCam360VehDefaultModelSpawn[i]);
	}
#if defined camp360veh_OnGameModeInit
		return camp360veh_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit camp360veh_OnGameModeInit
#if defined camp360veh_OnGameModeInit
	forward camp360veh_OnGameModeInit();
#endif

public OnPlayerDisconnect(playerid, reason)
{
	if (cam360veh_IsValidTimer(playerid))
	{
		cam360veh_DestroyVehicle(playerid);
		cam360veh_KillTimer(playerid);
	}
#if defined camp360veh_OnPlayerDisconnect
	return camp360veh_OnPlayerDisconnect(playerid, reason);
#else
	return 1;
#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect camp360veh_OnPlayerDisconnect
#if defined    camp360veh_OnPlayerDisconnect
	forward camp360veh_OnPlayerDisconnect(playerid, reason);
#endif

public OnPlayerSpawn(playerid)
{
	if (cam360veh_IsValidTimer(playerid))
	{
		cam360veh_DestroyVehicle(playerid);
		cam360veh_KillTimer(playerid);
	}
#if defined camp360veh_OnPlayerSpawn
	return camp360veh_OnPlayerSpawn(playerid);
#else
	return 1;
#endif
}
#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn camp360veh_OnPlayerSpawn
#if defined    camp360veh_OnPlayerSpawn
	forward camp360veh_OnPlayerSpawn(playerid);
#endif

CMD:spawn(playerid, params[])
{
	if (!cam360veh_IsValidTimer(playerid))
	{
		return 0;
	}
	cam360veh_KillTimer(playerid);

	new
		Float:x,
		Float:y,
		Float:z;
	
	GetPlayerCameraPos(playerid, x, y, z);

	new Float:angle = atan2(
		gCam360VehPlayerCoordY[playerid] - y,
		gCam360VehPlayerCoordX[playerid] - x) - 90.0;

	SetPlayerPos(playerid, x, y, gCam360VehPlayerCoordZ[playerid]);
	SetPlayerFacingAngle(playerid, angle);
	SetCameraBehindPlayer(playerid);
	return 1;
}

CMD:veh(playerid, params[])
{
	if (cam360veh_IsValidTimer(playerid))
	{
		return 0;
	}
	cam360veh_DestroyVehicle(playerid);

	const
		Float:PI = 3.141592653589793,
		Float:DISTANCE_BETWEEN_VEHICLE = 8.0;

	new max_count = cam360veh_GetCountVehicle();

	new
		Float:x,
		Float:y,
		Float:z,
		Float:angle,
		Float:veh_x,
		Float:veh_y,
		Float:rad_angle,
		vehicleid,
		index,
		Float:angle_step = 360.0 / float(max_count);

	GetPlayerPos(playerid, x, y, z);

	gCam360VehPlayerCoordX[playerid] = x;
	gCam360VehPlayerCoordY[playerid] = y;
	gCam360VehPlayerCoordZ[playerid] = z;

	foreach (Cam360VehiclesModels, modelid)
	{
		angle = angle_step * float(index++);
		rad_angle = angle * (PI / 180.0);
		veh_x = x + floatcos(rad_angle) * DISTANCE_BETWEEN_VEHICLE;
		veh_y = y + floatsin(rad_angle) * DISTANCE_BETWEEN_VEHICLE;

		vehicleid = CreateVehicle(
			modelid,
			veh_x,
			veh_y,
			z,
			angle - 90.0,
			-1,
			-1,
			-1
		);
		Iter_Add(Cam360VehiclesID[playerid], vehicleid);
	}
	gCam360VehPlayerTimer[playerid] = SetTimerEx(
		"@_OnTimerCam360VehicleFly",
		25,
		false,
		"dffff",
		playerid,
		x,
		y,
		z,
		0
	);
	return 1;
}

@_OnTimerCam360VehicleFly(playerid, Float:x, Float:y, Float:z, angle);
@_OnTimerCam360VehicleFly(playerid, Float:x, Float:y, Float:z, angle)
{
	const
		CAMERA_SPEED = 1,
		Float:CAMERA_RADIUS = 19.0,
		Float:CAMERA_HEIGHT = 5.0;
	
	angle = (angle + CAMERA_SPEED) % 360;

	new
		Float:cam_x = CAMERA_RADIUS * floatcos(angle, degrees),
		Float:cam_y = CAMERA_RADIUS * floatsin(angle, degrees);

	SetPlayerCameraPos(playerid, x + cam_x, y + cam_y, z + CAMERA_HEIGHT);
	SetPlayerCameraLookAt(playerid, x, y, z);

	gCam360VehPlayerTimer[playerid] = SetTimerEx(
		"@_OnTimerCam360VehicleFly",
		25,
		false,
		"dffff",
		playerid,
		x,
		y,
		z,
		angle
	);
	return 1;
}

/*

	functions can be called only inside
	
*/

static cam360veh_IsValidTimer(playerid)
{
	return gCam360VehPlayerTimer[playerid] != 0;
}

static cam360veh_KillTimer(playerid)
{
	KillTimer(gCam360VehPlayerTimer[playerid]);
	gCam360VehPlayerTimer[playerid] = 0;
	return 1;
}

static cam360veh_DestroyVehicle(playerid)
{
	foreach (Cam360VehiclesID[playerid], vehicleid)
	{
		DestroyVehicle(vehicleid);
		Iter_SafeRemove(Cam360VehiclesID[playerid], vehicleid, vehicleid);
	}
	return 1;
}

static cam360veh_GetCountVehicle()
{
	return Iter_Count(Cam360VehiclesModels);
}


/*

	functions can be called from outside

*/

stock cam360veh_ClearModelsInArray()
{
	Iter_Clear(Cam360VehiclesModels);
	return 1;
}

stock cam360veh_SetModelsInArray(modelid)
{
	if (!(CAM_360_VEH_MIN_VEHICLE_MODELID <= modelid <= CAM_360_VEH_MAX_VEHICLE_MODELID))
	{
		return 0;
	}
	Iter_Add(Cam360VehiclesModels, modelid);
	return 1;
}