modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SecretRock_RestorePlaced, 2000, false);
    }

    void SecretRock_RestorePlaced()
    {
        SecretRock_Persistence.RestoreAll();
    }
};
