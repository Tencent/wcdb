#!/usr/bin/env bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: xcframework.sh --scheme <SCHEME> [options]

Options:
  --scheme <SCHEME>           Target scheme to build. Required. (WCDB | WCDBCpp | WCDBObjc | WCDBSwift)
  --configuration <CONFIG>    Build configuration. Default: Release.
  --project <PATH>            Path to the .xcodeproj (relative to this script or absolute). Default: src/WCDB.xcodeproj
  --output <DIR>              Output directory (relative to this script or absolute). Default: <script_dir>/build
  --platforms <LIST...>       Platforms to build: ios ios-simulator macos macos-catalyst all. Default: all
  --no-xcframework            Skip xcframework creation.
  --xcbeautify                Pipe build output through xcbeautify.
  -h, --help                  Show this help message.
EOF
}

ScriptDir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
Root=$(cd "$ScriptDir/../.." && pwd)

scheme=""
configuration="Release"
project="src/WCDB.xcodeproj"
output="$ScriptDir/build"
platforms=()
no_xcframework=0
use_xcbeautify=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        --scheme)
            [[ $# -lt 2 ]] && { echo "Missing value for --scheme" >&2; usage; exit 1; }
            scheme="$2"
            shift 2
            ;;
        --configuration)
            [[ $# -lt 2 ]] && { echo "Missing value for --configuration" >&2; usage; exit 1; }
            configuration="$2"
            shift 2
            ;;
        --project)
            [[ $# -lt 2 ]] && { echo "Missing value for --project" >&2; usage; exit 1; }
            project="$2"
            shift 2
            ;;
        --output)
            [[ $# -lt 2 ]] && { echo "Missing value for --output" >&2; usage; exit 1; }
            output="$2"
            shift 2
            ;;
        --platforms)
            platforms=()
            shift
            while [[ $# -gt 0 && "$1" != --* ]]; do
                platforms+=("$1")
                shift
            done
            ;;
        --no-xcframework)
            no_xcframework=1
            shift
            ;;
        --xcbeautify)
            use_xcbeautify=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage
            exit 1
            ;;
    esac
done

if [[ -z "$scheme" ]]; then
    echo "Error: --scheme is required." >&2
    usage
    exit 1
fi

case "$scheme" in
    WCDB|WCDBCpp|WCDBObjc|WCDBSwift) ;;
    *)
        echo "Unknown scheme: $scheme" >&2
        exit 1
        ;;
esac

if [[ ${#platforms[@]} -eq 0 ]]; then
    platforms=("all")
fi

resolve_path() {
    local path="$1"
    if [[ "$path" == /* ]]; then
        printf "%s" "$path"
    else
        printf "%s/%s" "$Root" "$path"
    fi
}

get_destination() {
    case "$1" in
        ios) echo "generic/platform=iOS,name=Any iOS Device" ;;
        ios-simulator) echo "generic/platform=iOS Simulator" ;;
        macos) echo "generic/platform=macOS,name=Any Mac" ;;
        macos-catalyst) echo "generic/platform=macOS,variant=Mac Catalyst,name=Any Mac" ;;
        *)
            echo "Invalid platform: $1" >&2
            exit 1
            ;;
    esac
}

get_extra_args() {
    case "$1" in
        WCDBSwift) echo "BUILD_LIBRARY_FOR_DISTRIBUTION=YES" ;;
        *) echo "" ;;
    esac
}

is_valid_platform() {
    case "$1" in
        ios|ios-simulator|macos|macos-catalyst|all) return 0 ;;
        *) return 1 ;;
    esac
}

for platform in "${platforms[@]}"; do
    if ! is_valid_platform "$platform"; then
        echo "Invalid platform: $platform" >&2
        exit 1
    fi
done

if [[ $use_xcbeautify -eq 1 ]] && ! command -v xcbeautify >/dev/null 2>&1; then
    echo "xcbeautify not found in PATH." >&2
    exit 1
fi

project_file=$(resolve_path "$project")
build_out_dir=$(resolve_path "$output")
archives_dir="$build_out_dir/archives"
xcframeworks_dir="$build_out_dir/xcframeworks"

run_command() {
    local cmd=("$@")
    printf "\n[cmd] %s\n\n" "${cmd[*]}"
    "${cmd[@]}"
}

create_xcframework() {
    local output_dir="$1"
    local framework_name="$2"

    local cmd=(xcodebuild -create-xcframework)
    local archive
    for archive in "${archives[@]}"; do
        cmd+=(-archive "$archive" -framework "${framework_name}.framework")
    done

    mkdir -p "$output_dir"
    local output_path="$output_dir/${framework_name}.xcframework"
    if [[ -d "$output_path" ]]; then
        printf "[*] Removing existing xcframework: %s\n" "$output_path"
        rm -rf "$output_path"
    fi

    cmd+=(-output "$output_path")
    run_command "${cmd[@]}"
    printf "\n✅ Created XCFramework: %s\n" "$output_path"
}

start_time=$(date +%s)

printf "[*] SCRIPT_DIR: %s\n" "$ScriptDir"
printf "[*] REPO_ROOT_DIR: %s\n" "$Root"
printf "[*] XCODE_PROJECT_FILE: %s\n" "$project_file"
printf "[*] BUILD_OUT_DIR: %s\n" "$build_out_dir"
printf "[*] BUILD_ARCHIVES_DIR: %s\n\n" "$archives_dir"

mkdir -p "$archives_dir"
archives=()

default_platform_order=(ios ios-simulator macos macos-catalyst)
contains_all=0
for platform in "${platforms[@]}"; do
    if [[ "$platform" == "all" ]]; then
        contains_all=1
        break
    fi
done

if [[ $contains_all -eq 1 ]]; then
    build_platforms=("${default_platform_order[@]}")
else
    build_platforms=("${platforms[@]}")
fi

for platform in "${build_platforms[@]}"; do
    destination=$(get_destination "$platform")
    printf "\n🚧 Building %s (%s)...\n" "$scheme" "$platform"

    archive_base="${archives_dir}/${scheme}-${platform}"
    archive_path="${archive_base}.xcarchive"
    rm -rf "$archive_path"

    cmd=(xcodebuild archive
        -project "$project_file"
        -scheme "$scheme"
        -configuration "$configuration"
        -destination "$destination"
        -archivePath "$archive_base"
        SKIP_INSTALL=NO)

    extra_arg=$(get_extra_args "$scheme")
    if [[ -n "$extra_arg" ]]; then
        cmd+=("$extra_arg")
    fi

    if [[ $use_xcbeautify -eq 1 ]]; then
        printf "\n[cmd] %s\n\n" "${cmd[*]}"
        if ! "${cmd[@]}" | xcbeautify; then
            echo "Command failed: ${cmd[*]}" >&2
            exit 1
        fi
    else
        run_command "${cmd[@]}"
    fi

    archives+=("$archive_path")
done

if [[ $no_xcframework -eq 1 ]]; then
    printf "\n⚙️ Skipped XCFramework creation (--no-xcframework).\n"
else
    if [[ ${#archives[@]} -eq 0 ]]; then
        echo "No archives created. Aborting." >&2
        exit 1
    fi
    printf "\n🧩 Creating XCFramework for %s ...\n" "$scheme"
    mkdir -p "$xcframeworks_dir"
    create_xcframework "$xcframeworks_dir" "$scheme"
fi

end_time=$(date +%s)
elapsed=$((end_time - start_time))
mins=$((elapsed / 60))
secs=$((elapsed % 60))

printf "\n⏱ Total elapsed time: %d min %d sec (%d s)\n" "$mins" "$secs" "$elapsed"

