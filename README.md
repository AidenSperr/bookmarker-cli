# bookmarker-cli (bm)

Bookmark directories from anywhere in your PowerShell terminal, then jump
straight back to them with a short name
`cd ..\..\..\Projects\whatever-it-was-called`.

## Install

```powershell
irm https://raw.githubusercontent.com/AidenSperr/bookmarker-cli/master/install.ps1 | iex
```

Restart your terminal (or run `. $PROFILE`) afterward. This downloads a
prebuilt `bm.exe` from the latest release, writes the PowerShell wrapper
functions to `~\.bm-bookmarks\Bookmarks.ps1`, and adds a line to your
`$PROFILE` that loads them.

## Usage

| Command               | What it does                                    |
|-----------------------|-------------------------------------------------|
| `bma <name>`          | Bookmark the current directory as `<name>`      |
| `bma <name> <path>`   | Bookmark a specific path as `<name>`            |
| `bm <name>`           | Jump to a bookmark                              |
| `bm`                  | List bookmarks, then prompt for one to jump to  |
| `bml`                 | List all bookmarks, no prompt                   |
| `bmrm <name>`         | Remove a bookmark                               |

**NOTE:** Bookmarking a name that already exists updates it instead of creating a
duplicate. Bookmarks are stored as tab-separated text at `~\.bookmarks`,
so they're easy to look at or manually edit if you ever need to.

## Building from source

Requires a C compiler. [WinLibs GCC](https://winlibs.com/) is a simple
option:

```powershell
winget install --id BrechtSanders.WinLibs.POSIX.UCRT
git clone https://github.com/<owner>/<repo>.git
cd <repo>
gcc bm.c -o bm.exe -Wall -Wextra
```

Then either dot-source `Bookmarks.ps1` from your `$PROFILE` directly, or
point `install.ps1` at your local build instead of downloading a release.

## License

MIT — see [LICENSE](LICENSE).