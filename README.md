# depkg

Fast PS4 PKG extractor. Files are extracted in parallel, one thread per CPU core.

## Usage

    depkg FILE.pkg FOLDER

Without `FOLDER`, the files are extracted in the current folder.
`depkg FILE.pkg --check-type` only reports the kind of PKG as exit code.

| Exit code | Meaning |
|---|---|
| 0 | Extraction complete |
| 1 | The PKG could not be read or extracted |
| 101 / 102 / 103 | `--check-type`: base game / update / DLC |

`install_pkg` and `batch_install` (sh / bat) install PKGs into the folders used by ShadPS4.
Edit `gamesDir` and `addonsDir` in `install_pkg` before using them.

## Build

    git clone --recurse-submodules URL
    cd depkg/extractor/build
    bash lconf
    bash cryptobuild
    bash lbuild

`bash lpack` then creates `depkg.AppImage` on Linux. Requires clang, CMake and zlib.

## Credits and license

depkg is derived from [shadPS4Plus](https://github.com/AzaharPlus/shadPS4Plus), a standalone
extractor based on the [shadPS4](https://github.com/shadps4-emu/shadPS4) emulator, and it has been
modified: parallel extraction, faster decryption, 64-bit file sizes, error checks with exit codes
and a fix for the decryption of `npbind.dat`.

Licensed under GPL-2.0-or-later, see `LICENSE`. Files state their own license in their header,
and the license texts are in `LICENSES`.
