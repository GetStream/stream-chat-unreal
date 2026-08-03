set dotenv-load := false
set shell := ["bash", "-uc"]

# List recipes
default:
  just --list

# Build the entire project
build-project:
    ue4 build --NoPCH -NoSharedPCH -DisableUnity

# Build the StreamChat plugin
build:
    cd Plugins/StreamChat && ue4 build --NoPCH -NoSharedPCH -DisableUnity

# Run unit tests
test:
    ue4 test StreamChat -- -nosound -LogCmds="global off, LogAutomationCommandLine on, LogAutomationController on" | tee >(grep -q "EXIT CODE: 0")

# Convert svg assets to pngs
convert-svg:
    for f in $(find Plugins/StreamChat/Content -name '*.svg'); do echo "$f"; inkscape "$f" -o "${f%.svg}.png" & done
# On mac you need to install inkscape: brew install inkscape
# And symlink the CLI: ln -s /Applications/Inkscape.app/Contents/MacOS/inkscape /usr/local/bin/inkscape

# Format .cpp/.h files using clang-format
format:
    git ls-files '*.cpp' '*.h' | xargs clang-format -i

# Add copyright notice to all cs, cpp and h files
fix-copyright:
    for f in $(rg -t cpp -t h -t cs -g '!ThirdParty' --files-without-match -F "Copyright 2026 Stream.IO, Inc. All Rights Reserved."); do dos2unix $f; sed -i '' '1s;^;// Copyright 2026 Stream.IO, Inc. All Rights Reserved.\n\n;' $f; done

bump-version version:
    #!/usr/bin/env node
    const validSemVer = /^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$/.test("{{version}}")
    if (!validSemVer) {
        console.error("Invalid semver: {{version}}");
        process.exit(1);
    }

    console.log("Bumping to version v{{version}}");
    const fs = require('fs');
    for (const fileName of [
        "./Plugins/StreamChat/StreamChat.uplugin",
        "./Plugins/StreamChat/StreamChat.uplugin.5.7",
        "./Plugins/StreamChat/StreamChat.uplugin.5.8",
    ]) {
        const data = fs.readFileSync(fileName);
        const file = JSON.parse(data);

        // Bump version
        file.Version += 1;

        // Set version string
        file.VersionName = "{{version}}";

        fs.writeFile(fileName, JSON.stringify(file, null, 4), function writeJSON(err) {
            if (err) return console.error(err);
            console.log("Writing to " + fileName);
        });
    }

create-release-branch version: (bump-version version)
    echo "Preparing release {{version}}"

    git config --global user.name 'github-actions'
    git config --global user.email 'release@getstream.io'
    git checkout -q -b "release-{{version}}"
    git commit -am "Release: {{version}}"
    git push -q -u origin "release-{{version}}"

    echo "Done!"

# Switch uproject and uplugin files for different engine versions
set-engine version:
    cp StreamChatSample.uproject.{{version}} StreamChatSample.uproject
    cp Plugins/StreamChat/StreamChat.uplugin.{{version}} Plugins/StreamChat/StreamChat.uplugin

# ---------------------------------------------------------------------------
# Running the demos
# ---------------------------------------------------------------------------

# Locate an Unreal Engine install. Override with: UE_ROOT=/path/to/UE_5.8 just demo
_engine:
    #!/usr/bin/env bash
    set -euo pipefail
    if [[ -n "${UE_ROOT:-}" ]]; then
        if [[ ! -x "$UE_ROOT/Engine/Build/BatchFiles/Mac/Build.sh" && ! -f "$UE_ROOT/Engine/Build/BatchFiles/Build.bat" ]]; then
            echo "UE_ROOT is set to '$UE_ROOT' but that does not look like an engine install." >&2
            exit 1
        fi
        echo "$UE_ROOT"; exit 0
    fi
    # Ask the Epic launcher where it installed the engine
    for manifest in \
        "$HOME/Library/Application Support/Epic/UnrealEngineLauncher/LauncherInstalled.dat" \
        "/c/ProgramData/Epic/UnrealEngineLauncher/LauncherInstalled.dat" \
        "/mnt/c/ProgramData/Epic/UnrealEngineLauncher/LauncherInstalled.dat"; do
        if [[ -f "$manifest" ]]; then
            # Pick the highest engine version, not the alphabetically-last install path
            found=$(python3 -c 'import json,re,sys; d=json.load(open(sys.argv[1])); ue=[((int(m.group(1)),int(m.group(2))), i["InstallLocation"]) for i in d.get("InstallationList",[]) for m in [re.fullmatch(r"UE_(\d+)\.(\d+)", i.get("AppName",""))] if m]; print(sorted(ue)[-1][1] if ue else "")' "$manifest" 2>/dev/null || true)
            if [[ -n "$found" && -d "$found" ]]; then echo "$found"; exit 0; fi
        fi
    done
    # Fall back to the default install locations, newest first
    for candidate in "/Users/Shared/Epic Games/UE_5.8" "/Users/Shared/Epic Games/UE_5.7" \
                     "/c/Program Files/Epic Games/UE_5.8" "/c/Program Files/Epic Games/UE_5.7"; do
        if [[ -d "$candidate" ]]; then echo "$candidate"; exit 0; fi
    done
    echo "Could not find an Unreal Engine install. Set UE_ROOT, e.g.:" >&2
    echo "  UE_ROOT='/Users/Shared/Epic Games/UE_5.8' just demo" >&2
    exit 1

# Print the detected engine path
engine:
    @just _engine

# Compile the editor target. Needed once before running a demo, and after any C++ change.
build-editor:
    #!/usr/bin/env bash
    set -euo pipefail
    UE="$(just _engine)"
    echo "==> Using engine: $UE"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        "$UE/Engine/Build/BatchFiles/Mac/Build.sh" StreamChatSampleEditor Mac Development -project="$PWD/StreamChatSample.uproject" -waitmutex
    elif [[ "$OSTYPE" == "linux"* ]]; then
        "$UE/Engine/Build/BatchFiles/Linux/Build.sh" StreamChatSampleEditor Linux Development -project="$PWD/StreamChatSample.uproject" -waitmutex
    else
        "$UE/Engine/Build/BatchFiles/Build.bat" StreamChatSampleEditor Win64 Development -project="$PWD/StreamChatSample.uproject" -waitmutex
    fi

# Run the Team Chat demo in a standalone game window. This is the one to start with.
demo: (run "team-chat")

# Run a demo: team-chat | in-game-chat | jumpy-lion | tutorial
run sample="team-chat":
    #!/usr/bin/env bash
    set -euo pipefail
    case "{{sample}}" in
      team-chat)    MAP=/Game/TeamChatSample/Maps/TeamChatSample ;;
      in-game-chat) MAP=/Game/InGameChatSample/Maps/InGameChatSample ;;
      jumpy-lion)   MAP=/Game/JumpyLion/Maps/JumpyLion ;;
      tutorial)     MAP=/Game/Tutorial/Maps/Tutorial ;;
      *) echo "Unknown demo '{{sample}}'. Choose: team-chat | in-game-chat | jumpy-lion | tutorial" >&2; exit 1 ;;
    esac
    UE="$(just _engine)"
    echo "==> Running {{sample}} ($MAP)"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        EDITOR="$UE/Engine/Binaries/Mac/UnrealEditor"
    elif [[ "$OSTYPE" == "linux"* ]]; then
        EDITOR="$UE/Engine/Binaries/Linux/UnrealEditor"
    else
        EDITOR="$UE/Engine/Binaries/Win64/UnrealEditor.exe"
    fi
    "$EDITOR" "$PWD/StreamChatSample.uproject" "$MAP" -game -windowed -ResX=1280 -ResY=800

# Open the project in the Unreal Editor
edit:
    #!/usr/bin/env bash
    set -euo pipefail
    UE="$(just _engine)"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        open -a "$UE/Engine/Binaries/Mac/UnrealEditor.app" --args "$PWD/StreamChatSample.uproject"
    elif [[ "$OSTYPE" == "linux"* ]]; then
        "$UE/Engine/Binaries/Linux/UnrealEditor" "$PWD/StreamChatSample.uproject"
    else
        "$UE/Engine/Binaries/Win64/UnrealEditor.exe" "$PWD/StreamChatSample.uproject"
    fi
