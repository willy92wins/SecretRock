modded class Building
{
    override void OnDoorOpenFinish(DoorFinishParams params)
    {
        super.OnDoorOpenFinish(params);
        #ifdef SERVER
        SecretRock_Persistence.OnDoorChanged(this);
        #endif
    }

    override void OnDoorCloseFinish(DoorFinishParams params)
    {
        super.OnDoorCloseFinish(params);
        #ifdef SERVER
        SecretRock_Persistence.OnDoorChanged(this);
        #endif
    }
};
