# SecretRock

DayZ addon source for La Frontera. Compilable tree only.

Git remote: public GitHub `https://github.com/willy92wins/SecretRock` (this folder only; `P:\SecretRock_dev` is not in the repo).

Layout of `P:` (legacy sibling folders, same as LFSecure / LFQuad3):

- `P:\SecretRock` — this tree (what AddonBuilder packs)
- `P:\SecretRock_dev` — Blender, renders, docs, tools (not in this git repo)

## Addons in this tree

Shipped as two PBOs (no scripts). Names taken from `$PBOPREFIX$` / `CfgPatches`:

| Folder | `$PBOPREFIX$` | `CfgPatches` | `CfgVehicles` unit |
|---|---|---|---|
| `lf_vanilla_monolith2/` | `lf_vanilla_monolith2` | `lf_vanilla_monolith2` | `Land_LF_VanillaMonolith2` |
| `lf_monolith4secure/` | `lf_monolith4secure` | `lf_monolith4secure` | `Land_LF_Monolith4Secure` |

Product contract is **not** in this repo yet.

## How to compile

`P:\` must be mounted (`subst P: "C:\Users\guill\OneDrive\Documentos\DayZ Projects"`). Pack **each** addon folder, not this root:

```bat
AddonBuilder.exe P:\SecretRock\lf_vanilla_monolith2 P:\Mods\@SecretRock\Addons -prefix=lf_vanilla_monolith2 -temp=P:\temp\SecretRock\lf_vanilla_monolith2 -include=P:\SecretRock\include.lst -binarizeFullLogs -clear
AddonBuilder.exe P:\SecretRock\lf_monolith4secure P:\Mods\@SecretRock\Addons -prefix=lf_monolith4secure -temp=P:\temp\SecretRock\lf_monolith4secure -include=P:\SecretRock\include.lst -binarizeFullLogs -clear
copy /Y P:\SecretRock\mod.cpp P:\Mods\@SecretRock\mod.cpp
```

Wrapper: `P:\SecretRock_dev\tools\build.ps1`. After pack, verify PBO entries (not AddonBuilder exit code). Staging off `P:\` may be required if binarize parses a broken foreign `config.cpp` on this drive.

Archive of record: `C:\Users\guill\Downloads\Secretrock_LF.rar`. Hashes: `P:\SecretRock_dev\sources\SOURCES.md`.
