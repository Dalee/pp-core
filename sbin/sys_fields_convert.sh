#!/bin/bash

PSQL=~postgres/bin/psql
LOGIN=$1
TABLE=$2

if [[ $LOGIN == '' || $TABLE == '' ]]; then

	echo "To sys_* tables converter..."
	echo "USAGE: $0 <login> <table name> "
	exit 1
fi

echo 'BEGIN;'
for i in `echo '\d' | $PSQL -U $LOGIN $TABLE | awk '{print $3}'` 
do 
	if [[ $i != 'relations' && $i != 'Name' ]]; then
		
		echo -e "\n---------------------"
		echo "-- $i:"
		echo -e "---------------------"

		for t in `echo "\d $i" | ~postgres/bin/psql -U $LOGIN $TABLE | awk '{print $1}'`
		do
			if [[ $t == 'created' ]]; then
				echo "ALTER TABLE $i RENAME COLUMN created TO sys_created;"
			fi
			if [[ $t == 'modified' ]]; then
				echo "ALTER TABLE $i RENAME COLUMN modified TO sys_modified;"
			fi
			if [[ $t == 'owner' ]]; then
				echo "ALTER TABLE $i RENAME COLUMN owner TO sys_owner;"
			fi
			if [[ $t == 'accessmod' ]]; then
				echo "ALTER TABLE $i RENAME COLUMN accessmod TO sys_accessmod;"
			fi
			if [[ $t == 'accessput' ]]; then
				echo "ALTER TABLE $i RENAME COLUMN accessput TO sys_accessput;"
			fi
		done
	fi
done
echo 'COMMIT;'
