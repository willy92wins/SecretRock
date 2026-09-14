class SecretRock_KitBaseDeployable : DeployableContainer_Base
{
    void SecretRock_DeferredDelete()
    {
        g_Game.ObjectDelete(this);
    }

    void SecretRock_RestorePlacedDeferred(EntityAI placed)
    {
        SecretRock_PlacedRock.RestoreEntity(placed);
    }

    string SecretRock_GetSpawnClassname()
    {
        return "";
    }

    string GetDeployedClassname()
    {
        return SecretRock_GetSpawnClassname();
    }

    vector GetDeployPositionOffset()
    {
        return "0 0 0";
    }

    vector GetDeployOrientationOffset()
    {
        return "0 0 0";
    }

    override bool IsBasebuildingKit()
    {
        return true;
    }

    override bool IsDeployable()
    {
        return true;
    }

    override bool CanDisplayCargo()
    {
        return false;
    }

    override bool CanBePlaced(Man player, vector position)
    {
        return true;
    }

    override bool DoPlacingHeightCheck()
    {
        return false;
    }

    override string GetDeploySoundset()
    {
        return "placeBarbedWire_SoundSet";
    }

    override string GetLoopDeploySoundset()
    {
        return "";
    }

    override void SetActions()
    {
        super.SetActions();
        RemoveAction(ActionPlaceObject);
        AddAction(ActionTogglePlaceObject);
        AddAction(SecretRock_ActionPlaceGeneric);
    }

    override void OnPlacementCancelled(Man player)
    {
        super.OnPlacementCancelled(player);
        SetTakeable(true);
        SetIsBeingPlaced(false);
    }

    override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
    {
        super.OnPlacementComplete(player, position, orientation);

        #ifdef SERVER
        string spawnClass = SecretRock_GetSpawnClassname();
        if (spawnClass == "")
        {
            Print("[SecretRock] Empty spawn classname on " + GetType());
            return;
        }

        EntityAI placed = EntityAI.Cast(g_Game.CreateObjectEx(spawnClass, position, ECE_CREATEPHYSICS));
        if (placed)
        {
            placed.SetPosition(position);
            placed.SetOrientation(orientation);
            placed.Update();
            SecretRock_PlacedRock.RestoreEntity(placed);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SecretRock_RestorePlacedDeferred, 250, false, placed);
            SecretRock_Persistence.RegisterPlaced(placed);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SecretRock_DeferredDelete, 500, false);
        }
        else
        {
            Print("[SecretRock] Failed to create " + spawnClass + " — kit preserved.");
            PlayerBase pb = PlayerBase.Cast(player);
            if (pb)
            {
                pb.MessageStatus("Placement failed. Kit preserved.");
            }
        }
        #endif
    }
};
