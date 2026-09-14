class CfgPatches
{
    class SecretRock
    {
        units[] = {"SecretRock_VanillaMonolith2_Kit", "SecretRock_Monolith4Secure_Kit"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "lf_vanilla_monolith2", "lf_monolith4secure"};
    };
};

class CfgMods
{
    class SecretRock
    {
        dir = "SecretRock";
        picture = "";
        action = "";
        hideName = 0;
        hidePicture = 0;
        name = "SecretRock";
        prefix = "SecretRock";
        author = "La Frontera";
        version = "1.0";
        type = "mod";
        dependencies[] = {"World", "Mission"};
        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"SecretRock/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SecretRock/scripts/5_Mission"};
            };
        };
    };
};

class CfgVehicles
{
    class Inventory_Base;
    class SecretRock_VanillaMonolith2_Kit : Inventory_Base
    {
        scope = 2;
        displayName = "Monolith 2 kit";
        descriptionShort = "Places the vanilla monolith shelter. Spawn only.";
        model = "\SecretRock\data\kits\lf_kit_box.p3d";
        weight = 5000;
        itemSize[] = {5, 3};
        rotationFlags = 2;
        itemBehaviour = 2;
        canBeDigged = 0;
        carveNavmesh = 1;
        physLayer = "item_small";
        hiddenSelections[] = {"zbytek"};
        hiddenSelectionsTextures[] = {""};
        hiddenSelectionsMaterials[] = {""};
    };
    class SecretRock_Monolith4Secure_Kit : Inventory_Base
    {
        scope = 2;
        displayName = "Monolith 4 kit";
        descriptionShort = "Places the Monolith 4 shelter. Spawn only.";
        model = "\SecretRock\data\kits\lf_kit_box.p3d";
        weight = 5000;
        itemSize[] = {5, 3};
        rotationFlags = 2;
        itemBehaviour = 2;
        canBeDigged = 0;
        carveNavmesh = 1;
        physLayer = "item_small";
        hiddenSelections[] = {"zbytek"};
        hiddenSelectionsTextures[] = {""};
        hiddenSelectionsMaterials[] = {""};
    };
};
