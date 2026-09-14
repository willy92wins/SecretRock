modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SecretRock_RestorePlaced, 2000, false);
    }

    override void OnMissionFinish()
    {
        SecretRock_Persistence.SetShutdown();
        super.OnMissionFinish();
    }

    void SecretRock_RestorePlaced()
    {
        SecretRock_Persistence.RestoreAll();
    }
};
