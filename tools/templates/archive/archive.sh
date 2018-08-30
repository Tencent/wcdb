#!/usr/make

# prepare source
working_directory=`pwd`
codesnippets_zip="$working_directory"/codesnippets.zip
xctemplates_zip="$working_directory"/xctemplates.zip

cd "$working_directory"/../codesnippets && zip -r "$codesnippets_zip" * && cd "$working_directory"
cd "$working_directory"/../xctemplates && zip -r "$xctemplates_zip" * && cd "$working_directory"
