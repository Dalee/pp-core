if [ "${#@}" != "3" ]; then
	echo "usage: ./addReflexId.sh user database host";
	exit 0;
fi;

PSQL="psql -U $1 --dbname $2 -h $3"
SQL="\a \f | \t \\\ select table_name from information_schema.columns where column_name = 'sys_regions'" 
SQL_RESULT=`echo $SQL | $PSQL | awk '{ if (NR > 3)  { print }}' | sed  's/ /\n/g'`

REFLEX_COLUMN='sys_reflex_id int';
DENY_REGION_EDIT_COLUMN='deny_region_edit bool';

for i in $SQL_RESULT 
do
	echo $i;
	res=`echo -n "ALTER TABLE $i ADD $REFLEX_COLUMN;" | $PSQL 2>&1`
	res=`echo -n "ALTER TABLE $i ADD $DENY_REGION_EDIT_COLUMN;" | $PSQL 2>&1`
done;
