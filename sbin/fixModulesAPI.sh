#!/bin/bash

if [ "$1" == "-h" ]; then
	echo " -h show this page"
	echo " -rb remove backup files"
	exit;
fi

if [ "$1" == "-rb" ]; then
	echo "Remove bak up files";
	rm ./local/modules/*.module.inc.backup;
	exit;
fi;

for file in `find ./local/modules/ -name '*.module.inc'`
do
	echo $file
	echo "created backup file $file.backup"

	echo

	cp $file $file.backup

	sed -i -r 's/function\s+(user|admin)(Action|Index|Popup)\([^\)]+\)/function \1\2()/' $file
	sed -i -r 's/\$(app|db|user|request|layout|response|objects|tree)(->|\[)/$this->\1\2/' $file
	sed -i -r 's/\$currentSid/$this->tree->current-id/' $file
	sed -i -r 's/\$this->tree->leafs\[\$this->tree->current->id\]/$this->tree->current/' $file
done

if [ "$1" != "-rb" ]; then
	echo "if you want delete backup files use flag -rb"
fi;
