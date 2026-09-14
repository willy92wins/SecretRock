class SecretRock_PlacedEntry
{
    string classname;
    float px;
    float py;
    float pz;
    float ox;
    float oy;
    float oz;
    bool doorOpen;
};

class SecretRock_PlacedFile
{
    ref array<ref SecretRock_PlacedEntry> entries = new array<ref SecretRock_PlacedEntry>;
};

class SecretRock_Persistence
{
    static const string DIR = "$profile:SecretRock";
    static const string PATH = "$profile:SecretRock/placed.json";

    static bool IsOurRockType(string t)
    {
        return t == "Land_LF_VanillaMonolith2" || t == "Land_LF_Monolith4Secure";
    }

    static bool IsOurRock(Object obj)
    {
        if (!obj)
            return false;
        return IsOurRockType(obj.GetType());
    }

    static void EnsureDir()
    {
        if (!FileExist(DIR))
            MakeDirectory(DIR);
    }

    static void RegisterPlaced(EntityAI placed)
    {
        if (!placed)
            return;

        SecretRock_PlacedFile data = Load();
        SecretRock_PlacedEntry e = new SecretRock_PlacedEntry();
        e.classname = placed.GetType();
        vector p = placed.GetPosition();
        vector o = placed.GetOrientation();
        e.px = p[0];
        e.py = p[1];
        e.pz = p[2];
        e.ox = o[0];
        e.oy = o[1];
        e.oz = o[2];
        e.doorOpen = false;
        data.entries.Insert(e);
        Save(data);
    }

    static void OnDoorChanged(Building building)
    {
        if (!IsOurRock(building))
            return;

        SecretRock_PlacedFile data = Load();
        vector p = building.GetPosition();
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            vector ep = Vector(e.px, e.py, e.pz);
            if (e.classname == building.GetType() && vector.Distance(ep, p) < 0.75)
            {
                e.doorOpen = building.IsDoorOpen(0);
                Save(data);
                return;
            }
        }
    }

    static Building FindExisting(string classname, vector pos)
    {
        array<Object> nearby = new array<Object>;
        array<CargoBase> proxy = new array<CargoBase>;
        g_Game.GetObjectsAtPosition3D(pos, 0.75, nearby, proxy);
        int i;
        for (i = 0; i < nearby.Count(); i++)
        {
            Object obj = nearby.Get(i);
            if (!obj)
                continue;
            if (obj.GetType() != classname)
                continue;
            Building found = Building.Cast(obj);
            if (found)
                return found;
        }
        return null;
    }

    static void ApplyDoor(Building b, bool doorOpen)
    {
        if (!b)
            return;
        if (doorOpen)
        {
            if (!b.IsDoorOpen(0))
                b.OpenDoor(0);
        }
        else if (b.IsDoorOpen(0))
        {
            b.CloseDoor(0);
        }
    }

    static void RestoreAll()
    {
        SecretRock_PlacedFile data = Load();
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (!IsOurRockType(e.classname))
                continue;

            vector pos = Vector(e.px, e.py, e.pz);
            vector ori = Vector(e.ox, e.oy, e.oz);
            Building existing = FindExisting(e.classname, pos);
            if (existing)
            {
                SecretRock_PlacedRock.RestoreEntity(existing);
                ApplyDoor(existing, e.doorOpen);
                continue;
            }

            Building b = Building.Cast(g_Game.CreateObjectEx(e.classname, pos, ECE_CREATEPHYSICS));
            if (!b)
                continue;
            b.SetPosition(pos);
            b.SetOrientation(ori);
            b.Update();
            SecretRock_PlacedRock.RestoreEntity(b);
            ApplyDoor(b, e.doorOpen);
        }
    }

    static SecretRock_PlacedFile Load()
    {
        SecretRock_PlacedFile data = new SecretRock_PlacedFile();
        if (!FileExist(PATH))
            return data;

        string err;
        if (!JsonFileLoader<SecretRock_PlacedFile>.LoadFile(PATH, data, err))
        {
            Print("[SecretRock] placed.json load failed: " + err);
            data = new SecretRock_PlacedFile();
        }
        if (!data)
            data = new SecretRock_PlacedFile();
        if (!data.entries)
            data.entries = new array<ref SecretRock_PlacedEntry>;
        return data;
    }

    static void Save(SecretRock_PlacedFile data)
    {
        EnsureDir();
        string err;
        if (!JsonFileLoader<SecretRock_PlacedFile>.SaveFile(PATH, data, err))
            Print("[SecretRock] placed.json save failed: " + err);
    }
};
