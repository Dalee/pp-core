#!/bin/bash
for file in `find ./ -regextype egrep -regex '.*\.(php|inc|tmpl)$' 2>/dev/null`
do
	dName=`dirname "$file"`
	bName=`basename "$file"`

	if [ -f "$dName/.svn/text-base/$bName.svn-base" ]
	then
		if [ ! -f "$dName/.svn/prop-base/$bName.svn-base" ] || [ ! `grep 'svn:mime-type' "$dName/.svn/prop-base/$bName.svn-base"` ]
		then
			ext=`echo $bName | egrep -o -e '[^.]+$'`
			case $ext in
				"tmpl"        ) mime="text/html" ;;
				"php" | "inc" ) mime="text/x-php";;
			esac

			cmd="svn ps svn:mime-type '"${mime}"; charset=koi8-r' "${file}
			eval $cmd
		fi
	fi
done