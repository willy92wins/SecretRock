class SecretRock_PlacedEntry
{
    string id;
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
    static const string PATH_TMP = "$profile:SecretRock/placed.json.tmp";
    static const string PATH_BAK = "$profile:SecretRock/placed.json.bak";
    static const float MATCH_M = 0.75;

    protected static bool s_SaveInhibited;
    protected static bool s_Shutdown;
    protected static int s_IdSeq;

    static void SetShutdown()
    {
        s_Shutdown = true;
    }

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

    static string MakeId()
    {
        s_IdSeq++;
        int t = 0;
        if (g_Game)
            t = g_Game.GetTime();
        return "sr_" + t.ToString() + "_" + s_IdSeq.ToString() + "_" + Math.RandomInt(10000, 100000).ToString();
    }

    static void BindId(EntityAI ent, string persistId)
    {
        SecretRock_PlacedRock rock = SecretRock_PlacedRock.Cast(ent);
        if (!rock || persistId == "")
            return;
        rock.m_SecretRockPersistId = persistId;
    }

    static string IdOf(EntityAI ent)
    {
        SecretRock_PlacedRock rock = SecretRock_PlacedRock.Cast(ent);
        if (!rock)
            return "";
        return rock.m_SecretRockPersistId;
    }

    static void EnsureEntryIds(SecretRock_PlacedFile data)
    {
        if (!data || !data.entries)
            return;
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (!e)
                continue;
            if (e.id == "")
                e.id = MakeId();
        }
    }

    static int FindIndexById(SecretRock_PlacedFile data, string persistId)
    {
        if (!data || !data.entries || persistId == "")
            return -1;
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (e && e.id == persistId)
                return i;
        }
        return -1;
    }

    static int FindClosestIndex(SecretRock_PlacedFile data, string classname, vector pos)
    {
        if (!data || !data.entries)
            return -1;
        int best = -1;
        float bestDist = MATCH_M;
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (!e || e.classname != classname)
                continue;
            vector ep = Vector(e.px, e.py, e.pz);
            float d = vector.Distance(ep, pos);
            if (d < bestDist)
            {
                bestDist = d;
                best = i;
            }
        }
        return best;
    }

    static void RegisterPlaced(EntityAI placed)
    {
        if (!placed)
            return;

        SecretRock_PlacedFile data = Load();
        if (s_SaveInhibited)
            return;

        SecretRock_PlacedEntry e = new SecretRock_PlacedEntry();
        e.id = MakeId();
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
        BindId(placed, e.id);
        Save(data);
    }

    static void UnregisterEntity(EntityAI ent)
    {
        if (s_Shutdown || !ent)
            return;

        SecretRock_PlacedFile data = Load();
        if (s_SaveInhibited)
            return;

        int idx = FindIndexById(data, IdOf(ent));
        if (idx < 0)
            idx = FindClosestIndex(data, ent.GetType(), ent.GetPosition());
        if (idx < 0)
            return;

        array<ref SecretRock_PlacedEntry> kept = new array<ref SecretRock_PlacedEntry>;
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            if (i == idx)
                continue;
            kept.Insert(data.entries.Get(i));
        }
        data.entries = kept;
        Save(data);
    }

    static void OnDoorChanged(Building building)
    {
        if (!IsOurRock(building))
            return;

        SecretRock_PlacedFile data = Load();
        if (s_SaveInhibited)
            return;

        int idx = FindIndexById(data, IdOf(building));
        if (idx < 0)
            idx = FindClosestIndex(data, building.GetType(), building.GetPosition());
        if (idx < 0)
            return;

        SecretRock_PlacedEntry e = data.entries.Get(idx);
        e.doorOpen = building.IsDoorOpen(0);
        Save(data);
    }

    static Building FindExisting(string classname, vector pos)
    {
        array<Object> nearby = new array<Object>;
        array<CargoBase> proxy = new array<CargoBase>;
        g_Game.GetObjectsAtPosition3D(pos, MATCH_M, nearby, proxy);
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
        s_Shutdown = false;
        SecretRock_PlacedFile data = Load();
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (!e || !IsOurRockType(e.classname))
                continue;

            vector pos = Vector(e.px, e.py, e.pz);
            vector ori = Vector(e.ox, e.oy, e.oz);
            Building existing = FindExisting(e.classname, pos);
            if (existing)
            {
                BindId(existing, e.id);
                SecretRock_PlacedRock.RestoreEntity(existing);
                ApplyDoor(existing, e.doorOpen);
                continue;
            }

            Building b = Building.Cast(g_Game.CreateObjectEx(e.classname, pos, ECE_CREATEPHYSICS));
            if (!b)
            {
                Print("[SecretRock] RestoreAll CreateObjectEx failed classname=" + e.classname + " pos=" + pos.ToString() + " id=" + e.id);
                continue;
            }
            b.SetPosition(pos);
            b.SetOrientation(ori);
            b.Update();
            BindId(b, e.id);
            SecretRock_PlacedRock.RestoreEntity(b);
            ApplyDoor(b, e.doorOpen);
        }
    }

    static bool TryLoadPath(string path, out SecretRock_PlacedFile data, out string err)
    {
        data = new SecretRock_PlacedFile();
        err = "";
        if (!FileExist(path))
            return false;
        if (!JsonFileLoader<SecretRock_PlacedFile>.LoadFile(path, data, err))
        {
            data = new SecretRock_PlacedFile();
            return false;
        }
        if (!data)
            data = new SecretRock_PlacedFile();
        if (!data.entries)
            data.entries = new array<ref SecretRock_PlacedEntry>;
        return true;
    }

    static SecretRock_PlacedFile Load()
    {
        SecretRock_PlacedFile data;
        string err;

        if (TryLoadPath(PATH, data, err))
        {
            s_SaveInhibited = false;
            EnsureEntryIds(data);
            return data;
        }

        if (FileExist(PATH))
        {
            Print("[SecretRock] placed.json load failed: " + err);
            if (TryLoadPath(PATH_BAK, data, err))
            {
                Print("[SecretRock] placed.json recovered from .bak");
                s_SaveInhibited = false;
                EnsureEntryIds(data);
                return data;
            }
            s_SaveInhibited = true;
            Print("[SecretRock] placed.json corrupt and .bak unusable — save inhibited until admin repair (replace/delete placed.json then restart).");
            data = new SecretRock_PlacedFile();
            data.entries = new array<ref SecretRock_PlacedEntry>;
            return data;
        }

        if (TryLoadPath(PATH_TMP, data, err))
        {
            Print("[SecretRock] placed.json missing; promoting parseable .tmp");
            s_SaveInhibited = false;
            EnsureEntryIds(data);
            Save(data);
            return data;
        }

        if (TryLoadPath(PATH_BAK, data, err))
        {
            Print("[SecretRock] placed.json missing; recovered from .bak");
            s_SaveInhibited = false;
            EnsureEntryIds(data);
            return data;
        }

        s_SaveInhibited = false;
        data = new SecretRock_PlacedFile();
        data.entries = new array<ref SecretRock_PlacedEntry>;
        return data;
    }

    static void Save(SecretRock_PlacedFile data)
    {
        if (s_SaveInhibited)
        {
            Print("[SecretRock] placed.json save skipped (inhibited after corrupt load).");
            return;
        }
        if (!data)
            return;
        if (!data.entries)
            data.entries = new array<ref SecretRock_PlacedEntry>;

        EnsureDir();
        string err;
        if (!JsonFileLoader<SecretRock_PlacedFile>.SaveFile(PATH_TMP, data, err))
        {
            Print("[SecretRock] placed.json.tmp save failed: " + err);
            return;
        }
        if (!FileExist(PATH_TMP))
        {
            Print("[SecretRock] placed.json.tmp missing after write.");
            return;
        }

        SecretRock_PlacedFile verify;
        string verifyErr;
        if (!TryLoadPath(PATH_TMP, verify, verifyErr))
        {
            Print("[SecretRock] placed.json.tmp read-back failed: " + verifyErr);
            DeleteFile(PATH_TMP);
            return;
        }

        if (FileExist(PATH))
        {
            if (FileExist(PATH_BAK))
                DeleteFile(PATH_BAK);
            if (!CopyFile(PATH, PATH_BAK))
                Print("[SecretRock] placed.json.bak copy failed; continuing promote.");
        }

        if (FileExist(PATH))
            DeleteFile(PATH);
        if (!CopyFile(PATH_TMP, PATH))
        {
            Print("[SecretRock] placed.json promote from .tmp failed.");
            return;
        }
        DeleteFile(PATH_TMP);
    }
};
