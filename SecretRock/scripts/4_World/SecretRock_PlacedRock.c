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
            return;
        }

        TStringArray tex = ent.GetHiddenSelectionsTextures();
        TStringArray mats = ent.GetHiddenSelectionsMaterials();
        string placedTex;
        string placedMat;
        if (tex && idx < tex.Count())
        {
            placedTex = tex.Get(idx);
        }
        if (mats && idx < mats.Count())
        {
            placedMat = mats.Get(idx);
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
