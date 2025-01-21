### Rotation of the camera 360 degrees around transport vehicles

> Commands:
- ```/veh``` - create a vehicle and start rotating the camera.
- ```/spawn``` - spawns a player in front of the vehicle while the camera rotates.

> Functions:
- ```cam360veh_ClearModelsInArray()``` - Clear the array with models to input new models.
- ```cam360veh_SetModelsInArray(modelid)``` - Add a model (if not cleared, it appends to defaults).

> Example:
```pawn
public OnGameModeInit()
{
    cam360veh_ClearModelsInArray(); //Clear default models

    cam360veh_SetModelsInArray(411);
    cam360veh_SetModelsInArray(420);
    cam360veh_SetModelsInArray(450);
    cam360veh_SetModelsInArray(550);
    return 1;
}
```
