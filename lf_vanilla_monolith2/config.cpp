class CfgPatches
{
    class lf_vanilla_monolith2
    {
        units[] = {"Land_LF_VanillaMonolith2"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Rocks"};
    };
};
class CfgVehicles
{
    class HouseNoDestruct;
    class Land_LF_VanillaMonolith2: HouseNoDestruct
    {
        scope = 2;
        displayName = "Refugio vanilla Monolith 2";
        model = "lf_vanilla_monolith2\data\lf_vanilla_monolith2.p3d";
        autocenter = 0;
        class Doors
        {
            class Door1
            {
                displayName = "Entrada secreta";
                component = "door1";
                soundPos = "door1_action";
                animPeriod = 2.0;
                initPhase = 0;
                initOpened = 0;
                soundOpen = "doorMetalSmallOpen";
                soundClose = "doorMetalSmallClose";
                soundLocked = "doorMetalSmallRattle";
                soundOpenABit = "doorMetalSmallOpenABit";
            };
        };
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health { hitpoints = 10000; };
            };
            class GlobalArmor
            {
                class Projectile
                {
                    class Health { damage = 0; };
                    class Blood { damage = 0; };
                    class Shock { damage = 0; };
                };
                class Melee
                {
                    class Health { damage = 0; };
                    class Blood { damage = 0; };
                    class Shock { damage = 0; };
                };
            };
            class DamageZones
            {
                class Door1
                {
                    class Health { hitpoints = 10000; transferToGlobalCoef = 0; };
                    componentNames[] = {"door1"};
                    fatalInjuryCoef = -1;
                    class ArmorType
                    {
                        class Projectile { class Health { damage = 0; }; };
                        class Melee { class Health { damage = 0; }; };
                    };
                };
            };
        };
    };
};


