class SecretRock_ActionPlaceGeneric : ActionPlaceObject
{
    void SecretRock_ActionPlaceGeneric()
    {
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_FullBody = true;
    }

    override void SetupAnimation(ItemBase item)
    {
        if (!item)
            return;

        if (item.IsHeavyBehaviour())
        {
            m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_HEAVY;
        }
        else if (item.IsOneHandedBehaviour())
        {
            m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_1HD;
        }
        else if (item.IsTwoHandedBehaviour())
        {
            m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_2HD;
        }
        else
        {
            m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_2HD;
        }
    }

    override protected int GetStanceMask(PlayerBase player)
    {
        return DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }

    override void OnEndServer(ActionData action_data)
    {
        if (action_data.m_Player)
            action_data.m_Player.SetPerformedActionID(-1);

        PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);
        if (!poActionData || !poActionData.m_MainItem)
            return;

        if (!poActionData.m_AlreadyPlaced)
        {
            poActionData.m_MainItem.SetIsBeingPlaced(false);

            if (g_Game.IsMultiplayer())
            {
                poActionData.m_Player.PlacingCancelServer();
            }
            else
            {
                poActionData.m_Player.PlacingCancelLocal();
                poActionData.m_Player.PlacingCancelServer();
            }
        }

        if (poActionData.m_MainItem.GetLoopDeploySoundset() != string.Empty)
            poActionData.m_MainItem.StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
    }
};
