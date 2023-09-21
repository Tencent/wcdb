BEGIN {
    FS = ":"
}

$1 ~ /^[ \t]*version[ \t]*$/ {
    version = $2
    gsub(/^[ \t]+/, "", version)
    gsub(/[ \t]+&/, "", version)
    gsub(/\+/, "-", version)
    gsub(/\$\{date\}/, date, version)
    gsub(/\$\{git\.short_commit_id\}/, rev_short, version)
    print version
    exit
}
