class SecretRock_PlacedRock : House
{
    string m_SecretRockPersistId;

    // Land_LF_* visual faces are one p3d selection named zbytek.
    // Never SetObjectTexture / SetObjectMaterial on that selection: it
    // replaces native rock_monolith + concrete materials with a single
    // slot (and House ignores the restore anyway).
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

class Land_LF_VanillaMonolith2 : SecretRock_PlacedRock
{
};

class Land_LF_Monolith4Secure : SecretRock_PlacedRock
{
};
