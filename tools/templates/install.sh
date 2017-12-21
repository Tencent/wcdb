#!/usr/make

#xctemplate
install_xctemplates() {
    templates=~/Library/Developer/Xcode/Templates
    file_templates=~/Library/Developer/Xcode/Templates/File\ Templates
    source_templates=`pwd`/xctemplates/

    if [ -f $templates ] ;
    then
        rm $templates
    fi;
    mkdir -p "$file_templates"
    cp -R $source_templates "$file_templates"
}

install_codesnippets() {
    source_codesnippets=`pwd`/codesnippets/*.codesnippet
    codesnippets=~/Library/Developer/Xcode/UserData/CodeSnippets/
    cp $source_codesnippets $codesnippets
}

install_xctemplates

install_codesnippets