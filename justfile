set dotenv-load := false

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
    -ue4 test StreamChat

# Convert svg assets to pngs
# On mac you need to install inkscape: brew install inkscape
# And symlink the CLI: ln -s /Applications/Inkscape.app/Contents/MacOS/inkscape /usr/local/bin/inkscape
convert-svg:
    for f in $(find Plugins/StreamChat -name '*.svg'); do echo "$f"; inkscape "$f" --export-dpi=192 -o "${f%.svg}.png" & done

# Format .cpp/.h files using clang-format
format:
    git ls-files '*.cpp' '*.h' | xargs clang-format -i

check-copyright:
    rg -t cpp -t h --files-without-match Copyright