# SecretRock

DayZ kits that place two sliding-door monoliths (La Frontera). Compilable tree only.

Git: public GitHub `https://github.com/willy92wins/SecretRock` (this folder only; `P:\SecretRock_dev` is not in the repo).

| Folder | `$PBOPREFIX$` | Units |
|---|---|---|
| `lf_vanilla_monolith2/` | `lf_vanilla_monolith2` | `Land_LF_VanillaMonolith2` |
| `lf_monolith4secure/` | `lf_monolith4secure` | `Land_LF_Monolith4Secure` |
| `SecretRock/` | `SecretRock` | `SecretRock_VanillaMonolith2_Kit`, `SecretRock_Monolith4Secure_Kit` |

Spawn a kit, place it (hologram of the rock), look at `door1` to open/close (slide). No craft, no pack.

## Compile

Pack **each** addon folder:

```bat
AddonBuilder.exe P:\SecretRock\lf_vanilla_monolith2 P:\Mods\@SecretRock\Addons -prefix=lf_vanilla_monolith2 -temp=P:\temp\SecretRock\lf_vanilla_monolith2 -include=P:\SecretRock\include.lst -binarizeFullLogs -clear
AddonBuilder.exe P:\SecretRock\lf_monolith4secure P:\Mods\@SecretRock\Addons -prefix=lf_monolith4secure -temp=P:\temp\SecretRock\lf_monolith4secure -include=P:\SecretRock\include.lst -binarizeFullLogs -clear
AddonBuilder.exe P:\SecretRock\SecretRock P:\Mods\@SecretRock\Addons -prefix=SecretRock -temp=P:\temp\SecretRock\SecretRock -include=P:\SecretRock\include.lst -binarizeFullLogs -clear
copy /Y P:\SecretRock\mod.cpp P:\Mods\@SecretRock\mod.cpp
```

Wrapper: `P:\SecretRock_dev\tools\build.ps1`.
