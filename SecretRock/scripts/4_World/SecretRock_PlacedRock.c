class SecretRock_PlacedRock : House
{
    static void RestoreEntity(EntityAI ent)
    {
        if (!ent)
        {
            return;
        }

        int idx = ent.GetHiddenSelectionIndex("zbytek");
        if (idx < 0)
        {
            idx = 0;
        }

        string t = ent.GetType();
        string placedTex;
        string placedMat;
        if (t == "Land_LF_VanillaMonolith2")
        {
            placedTex = "lf_vanilla_monolith2\\data\\concrete_co.paa";
            placedMat = "lf_vanilla_monolith2\\data\\concrete.rvmat";
        }
        else if (t == "Land_LF_Monolith4Secure")
        {
            placedTex = "lf_monolith4secure\\data\\concrete_co.paa";
            placedMat = "lf_monolith4secure\\data\\concrete.rvmat";
        }
        else
        {
            TStringArray tex = ent.GetHiddenSelectionsTextures();
            TStringArray mats = ent.GetHiddenSelectionsMaterials();
            if (tex && idx < tex.Count())
            {
                placedTex = tex.Get(idx);
            }
            if (mats && idx < mats.Count())
            {
                placedMat = mats.Get(idx);
            }
        }

        if (placedTex != "")
        {
            ent.SetObjectTexture(idx, placedTex);
        }
        if (placedMat != "")
        {
            ent.SetObjectMaterial(idx, placedMat);
        }
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

    void SecretRock_RestorePlacedVisuals()
    {
        RestoreEntity(this);
    }

    override void EEInit()
    {
        super.EEInit();
        SecretRock_RestorePlacedVisuals();
    }
};

class Land_LF_VanillaMonolith2 : SecretRock_PlacedRock
{
};

class Land_LF_Monolith4Secure : SecretRock_PlacedRock
{
};
