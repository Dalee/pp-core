#!/bin/bash

if [ -d local ] && [ -d libpp ]; then [ ]; else echo 'I didn`t see a svn root. Sorry'; exit 1; fi;

greplist() {
	echo $(grep -R -P -l --exclude-dir=.svn '(user|admin)(Index|Action|Popup)\s*\([^\s].+\)' --exclude=*.backup local/*); # '
}

showlist() {
	for i in $(greplist); do echo $i; done;
}

case "$1" in
	-h|help)
		echo "$0 help: to look at this page"
		echo "$0 clean: to remove backup files"
		echo "$0 revert: to try to revert all known changes (take care while use it)"
		exit 1;
		;;

	-rb|clean)
		echo "Removing backup files";
		rm ./local/modules/*.module.inc.backup 2>/dev/null;
		rm ./local/modules/*.module.inc.backup.* 2>/dev/null;
		exit 1;
		;;

	revert|cleanup)
		echo "Reverting all changes";
		svn -R revert local/*
		rm ./local/modules/*.module.inc.backup 2>/dev/null;
		rm ./local/modules/*.module.inc.backup.* 2>/dev/null;
		exit 1;
		;;

	list)
		showlist
		exit 0;
		;;
esac;


if [ -z "$1" ]; then
	files=$(greplist)
else
	files=$(ls "$1")
fi;

for file in $files;
do
	echo "Processing $file"
	if (cp $file $file.backup); then
		echo "Backup file $file.backup created"
		echo
	else
		echo "Can't create backup file. Skipped..."
		echo
		continue;
	fi

	sed -i -r 's/function\s+(user|admin)(Action|Index|Popup)\([^\)]+\)/function \1\2()/' $file;
	sed -i -r 's/parent::(user|admin)(Action|Index|Popup)\([^\)]+\)/parent::\1\2()/g' $file;
	sed -i -r 's/\$currentCid\s*(!=\s*-1|<\s*0)/$this->objects->hasCurrent()/g' $file;
	sed -i -r 's/\$currentSid\s*!=\s*\$rootId/$this->tree->current->isRoot()/g' $file;
	sed -i -r 's/\$tree->leafs\[\s*\$currentSid\s*\]/$this->tree->current/g' $file;
	sed -i -r 's/\$(app|db|user|request|layout|response|objects|tree)(->|\[|,|\))/$this->\1\2/g' $file;
	sed -i -r 's/\$currentSid/$this->tree->current->id/' $file;
	sed -i -r 's/\$currentCid/$current = $this->objects->getCurrent(); \/\* fixme \*\/ $currentCid = $current["id"]/' $file;
	sed -i -r 's/\$currentCtype/$this->objects->getCurrentType()/' $file;
	sed -i -r 's/BASEPATH\s*\.\s*(["'"'"'])\/var\//BASEPATH . \1\/site\/var\//g' $file;
	sed -i -r 's/\$pathId/$this->tree->pathId/g' $file;
	sed -i -r 's/\$urlFile/$this->request->getFile()/g' $file;
	sed -i -r 's/function\s+([a-z0-9]+)\s*\(([^\)]*)\$this->([a-z0-9]+)([^\)]*)\)/function \1(\2 $\3 \4)/ig' $file;
	for k in {1..9}; do
		sed -i -r 's/function\s(.*)\$this->([\w\d]+?)/function \1\$\2/ig' $file;
	done;

	# ancient names and other praetorian stuff
	sed -i -r 's/(RT|NL)AbstractModule/PXModule/g' $file;
	sed -i -r 's/(RT|NL)(Request|Response)::/PXRegistry::get\2()->/g' $file;

	diff -y --left-column --suppress-common-lines -W 180 $file.backup $file
done

echo "Also look at:"
grep -R -P -B 1 -A 4 --exclude-dir=.svn --exclude=*.backup '(Subsys_JsHttpRequest_Js|\$(heap|references|leaf->children|error|request|response))' local/*


echo
echo "if you want delete backup files use 'clean' command"
exit 0;
