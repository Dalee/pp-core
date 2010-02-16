#!/bin/bash
declare -a search
declare -a replace
declare -a check

search[0]='function (user|admin)(Action|Index|Popup)\([^)]+\)'
replace[0]='function \1\2()'
check[0]='function (user|admin)(Action|Index|Popup)\('
save[0]=0

search[1]='\$(app|db|user|request|layout|response|objects|tree)(->|\[)'
replace[1]='\$this->\1\2'
check[1]='(\$|\$this->)(app|db|user|request|layout|response|objects|tree)(->|\[)'
save[1]=0

search[2]='\$currentSid'
replace[2]='\$this->tree->current->id'
check[2]='(\$currentSid|\$this->tree->current->id)'
save[2]=0

search[3]='\$this->tree->leafs[$this->tree->current->id]'
replace[3]='\$this->tree->current'
check[3]='tree->current'
save[3]=1

for i in 0 1 2 3
do
	for file in `find ./local/modules/ -name '*.module.inc' | xargs egrep -li "${search[i]}"`
	do
		newText=`cat $file | sed -r "s/${search[i]}/${replace[i]}/g"`

		if [ ${save[i]} ]
		then
			echo "fix $file"
			echo -e $newText > "$file.fix";
			mv "$file.fix" $file

		else
			echo "test $file"
			echo -e $newText | egrep --color=always "${check[i]}"
		fi
	done
done
