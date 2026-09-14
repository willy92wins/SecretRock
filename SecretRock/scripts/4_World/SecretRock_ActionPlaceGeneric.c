class SecretRock_ActionPlaceGenericCB : ActionPlaceObjectCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT);
    }
};

class SecretRock_ActionPlaceGeneric : ActionPlaceObject
{
    void SecretRock_ActionPlaceGeneric()
    {
        m_CallbackClass = SecretRock_ActionPlaceGenericCB;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_FullBody = true;
    }

    override bool HasProgress()
    {
        return true;
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

    override void OnEndClient(ActionData action_data)
    {
        super.OnEndClient(action_data);

        PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);
        if (!poActionData || !poActionData.m_MainItem || !poActionData.m_Player)
            return;

        if (poActionData.m_AlreadyPlaced)
            return;

        EntityAI inHands = poActionData.m_Player.GetEntityInHands();
        if (inHands != poActionData.m_MainItem)
        {
            poActionData.m_Player.PredictiveTakeEntityToHands(poActionData.m_MainItem);
        }
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
            poActionData.m_MainItem.SetTakeable(true);

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
