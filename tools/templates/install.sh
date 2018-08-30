#!/usr/make

# prepare source
codesnippets_zip=codesnippets.zip
xctemplates_zip=xctemplates.zip

local_codesnippets_zip=`pwd`/archive/$codesnippets_zip
local_xctemplates_zip=`pwd`/archive/$xctemplates_zip

if ! git rev-parse --git-dir > /dev/null 2>&1; then
    # fetch remote zip    
    remote_url="https://raw.githubusercontent.com/Tencent/wcdb/master/tools/templates/archive"
    temp_directory=`mktemp -d`
    
    remote_codesnippets_zip=$remote_url/$codesnippets_zip
    local_codesnippets_zip=$temp_directory/$codesnippets_zip
    curl $remote_codesnippets_zip -o $local_codesnippets_zip -s
    echo "fetch $remote_codesnippets_zip to $local_codesnippets_zip"
    
    remote_xctemplates_zip=$remote_url/$xctemplates_zip
    local_xctemplates_zip=$temp_directory/$xctemplates_zip
    curl $remote_xctemplates_zip -o $local_xctemplates_zip -s
    echo "fetch $remote_xctemplates_zip to $local_xctemplates_zip"    
fi

templates=~/Library/Developer/Xcode/Templates

target_templates="$templates"/File\ Templates/
target_codesnippets=~/Library/Developer/Xcode/UserData/CodeSnippets/

# handle unexpected issue
if [ -f "$templates" ] ;
then
    rm "$templates"
fi;
if [ -h "$target_templates" ] ;
then
    rm "$target_templates"
fi;
rm -f "$target_codesnippets"/WCDB.*
rm -fr "$target_templates"/WCDB

install() {
    source_zip="$1"
    target="$2"
    mkdir -p "$target"
    unzip -o -q "$source_zip" -d "$target"
}

# install code templates
install "$local_xctemplates_zip" "$target_templates"
echo "Code templates is installed at $target_templates"

# install codesnippets
install "$local_codesnippets_zip" "$target_codesnippets"
echo "Code snippets is installed at $target_codesnippets"
