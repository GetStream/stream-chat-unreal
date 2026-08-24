#!/usr/bin/env bash
# Resolve an Unreal Engine install for $1 (e.g. "5.8"), point ue4cli at it, and
# switch the .uproject/.uplugin to that engine's variant.
#
# `ue4 setroot` only *warns* when handed a path that isn't an engine, so a wrong
# root used to sail through this step and resurface much later as an opaque
# ue4cli traceback about a missing Engine/Build/Build.version. Resolve the root
# here and fail immediately with a path list instead.
set -euo pipefail

VERSION="${1:?usage: select-engine.sh <engine-version>}"

# Git Bash sees C:\UE as /c/UE; test existence with the converted path but hand
# ue4cli the native one, which is what it stores in its config.
to_bash_path() {
    case "$1" in
        [A-Za-z]:\\*|[A-Za-z]:/*)
            printf '/%s/%s\n' "$(printf '%s' "${1:0:1}" | tr '[:upper:]' '[:lower:]')" "$(printf '%s' "${1:3}" | tr '\\' '/')"
            ;;
        *) printf '%s\n' "$1" ;;
    esac
}

candidates=()
# An explicit override always wins, so a runner with a non-standard layout can
# be fixed by setting UE_ROOT (or the per-version UE_ROOT_5_8) on the runner or
# as a repository variable, without editing this script.
version_var="UE_ROOT_${VERSION//./_}"
[[ -n "${!version_var:-}" ]] && candidates+=("${!version_var}")
[[ -n "${UE_ROOT:-}" ]] && candidates+=("$UE_ROOT")

case "$(uname -s)" in
    Darwin)
        candidates+=(
            "/Users/Shared/Epic Games/UE_${VERSION}"
            "/Applications/Epic Games/UE_${VERSION}"
        )
        ;;
    *)
        candidates+=(
            "C:\\UE\\UE_${VERSION}"
            "C:\\Program Files\\Epic Games\\UE_${VERSION}"
            "D:\\UE\\UE_${VERSION}"
            "D:\\Program Files\\Epic Games\\UE_${VERSION}"
            "C:\\Epic Games\\UE_${VERSION}"
        )
        ;;
esac

root=""
for candidate in "${candidates[@]}"; do
    if [[ -f "$(to_bash_path "$candidate")/Engine/Build/Build.version" ]]; then
        root="$candidate"
        break
    fi
done

if [[ -z "$root" ]]; then
    echo "::error title=Unreal Engine ${VERSION} not found::No engine install for UE ${VERSION} on this runner. Looked in the paths below; set UE_ROOT_${VERSION//./_} or UE_ROOT to override." >&2
    printf '  %s\n' "${candidates[@]}" >&2
    exit 1
fi

echo "Using Unreal Engine ${VERSION} at: ${root}"
ue4 setroot "$root"
just set-engine "$VERSION"

# Let later steps in the job reuse the resolved root. UE_ROOT is the native path
# ue4cli wants; UE_ROOT_BASH is the same path in a form bash can quote and use,
# which matters on Windows where the engine may live under "C:\Program Files".
{
    echo "UE_ROOT=${root}"
    echo "UE_ROOT_BASH=$(to_bash_path "$root")"
} >> "$GITHUB_ENV"
