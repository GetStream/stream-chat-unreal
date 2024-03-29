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
    for f in $(rg -t cpp -t h -t cs -g '!ThirdParty' --files-without-match -F "Copyright 2022 Stream.IO, Inc. All Rights Reserved."); do dos2unix $f; sed -i '' '1s;^;// Copyright 2022 Stream.IO, Inc. All Rights Reserved.\n\n;' $f; done

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
        "./Plugins/StreamChat/StreamChat.uplugin.4.27",
        "./Plugins/StreamChat/StreamChat.uplugin.5.0",
        "./Plugins/StreamChat/StreamChat.uplugin.5.1",
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
