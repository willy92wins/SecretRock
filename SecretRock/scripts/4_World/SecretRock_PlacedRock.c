class SecretRock_PlacedRock : House
{
    string m_SecretRockPersistId;

    // Land_LF_* keep native face materials. Never SetObjectTexture /
    // SetObjectMaterial on a whole-mesh selection (do not add zbytek).
    static bool SecretRock_TargetHasSelection(ActionTarget target, string want)
    {
        if (!target || want == "")
        {
            return false;
        }

        Object obj = target.GetObject();
        if (!obj)
        {
            return false;
        }

        TStringArray names = new TStringArray;
        obj.GetActionComponentNameList(target.GetComponentIndex(), names);
        int i;
        for (i = 0; i < names.Count(); i++)
        {
            if (names.Get(i) == want)
            {
                return true;
            }
        }

        return false;
    }

    // Open: sliding leaf, or the static cubes/panel beside the door.
    // Close: static panel/cubes (door1 faces that are not door1_leaf).
    static bool SecretRock_IsOpenTarget(ActionTarget target)
    {
        if (SecretRock_TargetHasSelection(target, "door1_leaf"))
        {
            return true;
        }

        if (SecretRock_TargetHasSelection(target, "door1_panel"))
        {
            return true;
        }

        return false;
    }

    static bool SecretRock_IsCloseTarget(ActionTarget target)
    {
        if (SecretRock_TargetHasSelection(target, "door1_panel"))
        {
            return true;
        }

        if (SecretRock_TargetHasSelection(target, "door1") && !SecretRock_TargetHasSelection(target, "door1_leaf"))
        {
            return true;
        }

        return false;
    }

    static void RestoreEntity(EntityAI ent)
    {
    }

    static void RestoreNearby(string classname, vector pos)
    {
        if (classname == "")
        {
            return;
        }

        array<Object> nearby = new array<Object>;
        array<CargoBase> proxy = new array<CargoBase>;
        g_Game.GetObjectsAtPosition3D(pos, 1.5, nearby, proxy);
        int i;
        for (i = 0; i < nearby.Count(); i++)
        {
            Object obj = nearby.Get(i);
            if (!obj)
            {
                continue;
            }
            if (obj.GetType() != classname)
            {
                continue;
            }
            EntityAI ent = EntityAI.Cast(obj);
            RestoreEntity(ent);
        }
    }

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        #ifdef SERVER
        SecretRock_Persistence.UnregisterEntity(this);
        #endif
    }
};

class Land_LF_VanillaMonolith1 : SecretRock_PlacedRock
{
};

class Land_LF_VanillaMonolith2 : SecretRock_PlacedRock
{
};

class Land_LF_Monolith4Secure : SecretRock_PlacedRock
{
};
