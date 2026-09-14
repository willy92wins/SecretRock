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
    int version;
    ref array<ref SecretRock_PlacedEntry> entries = new array<ref SecretRock_PlacedEntry>;
};

class SecretRock_Persistence
{
    static const string DIR = "$profile:SecretRock";
    static const string PATH = "$profile:SecretRock/placed.json";
    static const string PATH_TMP = "$profile:SecretRock/placed.json.tmp";
    static const string PATH_BAK = "$profile:SecretRock/placed.json.bak";
    static const string PATH_BAD = "$profile:SecretRock/placed.json.bad";
    static const float MATCH_M = 0.75;
    static const int FORMAT_VERSION = 1;
    static const int RESTORE_RETRY_MAX = 3;

    // JsonFileLoader<T>.LoadFile/SaveFile: P:\scripts\3_game\tools\jsonfileloader.c:7-66
    // FileExist / CopyFile / DeleteFile / MakeDirectory: P:\scripts\1_core\proto\ensystem.c:397,525,528,531
    // DayZ has no rename; dest delete+copy is not atomic.

    protected static bool s_SaveInhibited;
    protected static bool s_Shutdown;
    protected static int s_IdSeq;
    protected static int s_RestoreAttempts;
    protected static bool s_RestoreRetry;

    static void SetShutdown()
    {
        s_Shutdown = true;
    }

    static bool ConsumeRestoreRetry()
    {
        bool retry = s_RestoreRetry;
        s_RestoreRetry = false;
        return retry;
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

    static int EnsureEntryIds(SecretRock_PlacedFile data)
    {
        int assigned = 0;
        if (!data || !data.entries)
            return 0;
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (!e)
                continue;
            if (e.id == "")
            {
                e.id = MakeId();
                assigned++;
            }
        }
        return assigned;
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

    static bool BuildingClaimed(array<Building> claimed, Building b)
    {
        if (!claimed || !b)
            return false;
        int i;
        for (i = 0; i < claimed.Count(); i++)
        {
            if (claimed.Get(i) == b)
                return true;
        }
        return false;
    }

    static void PersistAssignedIds(SecretRock_PlacedFile data, int assigned)
    {
        if (assigned <= 0 || s_SaveInhibited)
            return;
        Save(data);
    }

    static void RegisterPlaced(EntityAI placed)
    {
        if (!placed)
            return;

        SecretRock_PlacedFile data = Load();
        if (s_SaveInhibited)
        {
            Print("[SecretRock] RegisterPlaced skipped (save inhibited).");
            return;
        }

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

    static Building FindExisting(string classname, vector pos, array<Building> claimed)
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
            if (!found)
                continue;
            if (BuildingClaimed(claimed, found))
                continue;
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
        if (s_SaveInhibited)
        {
            Print("[SecretRock] RestoreAll skipped (placed.json unreadable; save inhibited).");
            return;
        }

        array<Building> claimed = new array<Building>;
        int failed = 0;
        int i;
        for (i = 0; i < data.entries.Count(); i++)
        {
            SecretRock_PlacedEntry e = data.entries.Get(i);
            if (!e || !IsOurRockType(e.classname))
            {
                string skipId = "";
                string skipClass = "";
                if (e)
                {
                    skipId = e.id;
                    skipClass = e.classname;
                }
                Print("[SecretRock] RestoreAll skip invalid entry id=" + skipId + " classname=" + skipClass);
                continue;
            }

            vector pos = Vector(e.px, e.py, e.pz);
            vector ori = Vector(e.ox, e.oy, e.oz);
            Building existing = FindExisting(e.classname, pos, claimed);
            if (existing)
            {
                BindId(existing, e.id);
                claimed.Insert(existing);
                SecretRock_PlacedRock.RestoreEntity(existing);
                ApplyDoor(existing, e.doorOpen);
                continue;
            }

            Building b = Building.Cast(g_Game.CreateObjectEx(e.classname, pos, ECE_CREATEPHYSICS));
            if (!b)
            {
                Print("[SecretRock] RestoreAll CreateObjectEx failed classname=" + e.classname + " pos=" + pos.ToString() + " id=" + e.id);
                failed++;
                continue;
            }
            b.SetPosition(pos);
            b.SetOrientation(ori);
            b.Update();
            BindId(b, e.id);
            claimed.Insert(b);
            SecretRock_PlacedRock.RestoreEntity(b);
            ApplyDoor(b, e.doorOpen);
        }

        if (failed > 0 && s_RestoreAttempts < RESTORE_RETRY_MAX)
        {
            s_RestoreAttempts++;
            s_RestoreRetry = true;
            Print("[SecretRock] RestoreAll will retry failed=" + failed.ToString() + " attempt=" + s_RestoreAttempts.ToString());
        }
        else
        {
            s_RestoreRetry = false;
            if (failed > 0)
                Print("[SecretRock] RestoreAll giving up failed=" + failed.ToString());
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
        if (data.version > FORMAT_VERSION)
        {
            err = "future sidecar version " + data.version.ToString();
            data = new SecretRock_PlacedFile();
            return false;
        }
        if (!data.entries)
            data.entries = new array<ref SecretRock_PlacedEntry>;
        if (data.version <= 0)
            data.version = FORMAT_VERSION;
        return true;
    }

    static bool PromoteVerified(string src, string dest)
    {
        if (!FileExist(src))
            return false;
        if (FileExist(dest))
            DeleteFile(dest);
        if (!CopyFile(src, dest))
            return false;
        SecretRock_PlacedFile verify;
        string verifyErr;
        if (!TryLoadPath(dest, verify, verifyErr))
        {
            Print("[SecretRock] promote verify failed dest=" + dest + " err=" + verifyErr);
            if (FileExist(dest))
                DeleteFile(dest);
            return false;
        }
        return true;
    }

    static SecretRock_PlacedFile Load()
    {
        SecretRock_PlacedFile data;
        string err;

        if (TryLoadPath(PATH, data, err))
        {
            s_SaveInhibited = false;
            PersistAssignedIds(data, EnsureEntryIds(data));
            return data;
        }

        if (FileExist(PATH))
        {
            if (err.IndexOf("future sidecar") != -1)
            {
                s_SaveInhibited = true;
                Print("[SecretRock] placed.json " + err + " — save inhibited (read-only reject).");
                data = new SecretRock_PlacedFile();
                data.entries = new array<ref SecretRock_PlacedEntry>;
                return data;
            }

            Print("[SecretRock] placed.json load failed: " + err);
            if (FileExist(PATH_BAD))
                DeleteFile(PATH_BAD);
            if (!CopyFile(PATH, PATH_BAD))
                Print("[SecretRock] could not copy placed.json to .bad");

            if (TryLoadPath(PATH_BAK, data, err))
            {
                Print("[SecretRock] placed.json recovered from .bak (dest left as .bad evidence).");
                s_SaveInhibited = false;
                int assignedBak = EnsureEntryIds(data);
                if (!PromoteVerified(PATH_BAK, PATH))
                    Print("[SecretRock] could not restore dest from .bak; in-memory bak kept, dest may be absent.");
                PersistAssignedIds(data, assignedBak);
                return data;
            }

            s_SaveInhibited = true;
            Print("[SecretRock] placed.json corrupt and .bak unusable — save inhibited until admin repair (replace placed.json from .bak/.tmp/.bad or delete it, then restart).");
            data = new SecretRock_PlacedFile();
            data.entries = new array<ref SecretRock_PlacedEntry>;
            return data;
        }

        if (TryLoadPath(PATH_TMP, data, err))
        {
            Print("[SecretRock] placed.json missing; promoting parseable .tmp");
            s_SaveInhibited = false;
            PersistAssignedIds(data, EnsureEntryIds(data));
            if (!FileExist(PATH))
                PromoteVerified(PATH_TMP, PATH);
            return data;
        }

        if (TryLoadPath(PATH_BAK, data, err))
        {
            Print("[SecretRock] placed.json missing; recovered from .bak");
            s_SaveInhibited = false;
            int assignedMissing = EnsureEntryIds(data);
            PromoteVerified(PATH_BAK, PATH);
            PersistAssignedIds(data, assignedMissing);
            return data;
        }

        s_SaveInhibited = false;
        data = new SecretRock_PlacedFile();
        data.version = FORMAT_VERSION;
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
        data.version = FORMAT_VERSION;

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
            return;
        }

        if (FileExist(PATH))
        {
            if (FileExist(PATH_BAK))
                DeleteFile(PATH_BAK);
            if (!CopyFile(PATH, PATH_BAK))
            {
                Print("[SecretRock] placed.json.bak copy failed — dest not replaced.");
                return;
            }
        }

        if (!PromoteVerified(PATH_TMP, PATH))
        {
            Print("[SecretRock] placed.json promote from .tmp failed; .tmp and .bak kept.");
            return;
        }
        DeleteFile(PATH_TMP);
    }
};
