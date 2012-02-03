#!/bin/bash

loadini() {
  BASE=`readlink -f "$0" | sed -r -e 's@(.*)/libpp/sbin/.*@\1@'`;
  inifile="$BASE/site/etc/database.ini"
  if [ ! -f $inifile ]; then usage "Can't load database.ini at base $BASE"; fi;
  for l in `cat $inifile | sed -r -e 's/\s*=\s*/=/g'`; do
    case "$l" in
      dbname=*) DB_NAME=${l//*=/} ;;
      dbtype=*) DB_TYPE=${l//*=/} ;;
      user=*) DB_USER=${l//*=/} ;;
      host=*) DB_HOST=${l//*=/} ;;
    esac
  done
  [[ "$DB_TYPE" != "" ]] && [[ "$DB_TYPE" != "pgsql" ]] && usage "unsupported database type $DB_TYPE"
}

usage() {
  [ "/$1/" != "//" ] && echo $1
  echo "Usage:"
  echo "  $0 [-t] [-q] [-r] [-U <user>] [-h <host>] [-p <port>] <database>"
  echo "Use -r for real dropping. Dry run by default"
  exit 1
}

notsql() {
  return `echo '' | $@ 2>&1 | grep 'psql: ' >/dev/null`;
}

loadini;

RUN=0;
QUITE=0;
TEST=0;
while [ "/$1/" != '//' ] ; do
  if [[ "$1" =~ ^-[Uhp]$ ]] && [ -z $2 ]; then
    usage "Flag $1 without value"
  fi
  if [[ "$1" == -* ]] && ! [[ "$1" =~ ^-[Uhpqtr]$ ]]; then
    usage "Unknown flag: $1"
  fi
  case "$1" in
    "-t") TEST=1; shift 1 ;;
    "-q") QUITE=1; shift 1 ;;
    "-r") RUN=1; shift 1 ;;
    "-U") [ ! -z $DB_USER   ] && usage "Too many users"; DB_USER=$2; shift 2 ;;
    "-h") [ ! -z $DB_HOST   ] && usage "Too many hosts"; DB_HOST=$2; shift 2 ;;
    "-p") [ ! -z $DB_PORT   ] && usage "Too many ports"; DB_PORT=$2; shift 2 ;;
       *) [ ! -z $DB_NAME   ] && usage "Too many databases"; DB_NAME=$1; shift 1 ;;
  esac
done

[ -z $DB_USER ] && DB_USER='web';
[ -z $DB_HOST ] && DB_HOST='db91';
[ -z $DB_PORT ] && DB_PORT='5432';
[ -z $DB_USER ] || [ -z $DB_HOST ] || [ -z $DB_PORT ] || [ -z $DB_NAME ] && usage;

SQLBIN="psql -U $DB_USER -h $DB_HOST --port $DB_PORT $DB_NAME";


if [[ $TEST == "1" ]]; then
  [[ $QUITE == "0" ]] && $SQLBIN;
  if notsql $SQLBIN; then
    [[ $QUITE == "0" ]] && echo "Can't connect to DB"
    exit 2
  else
    [[ $QUITE == "0" ]] && echo "Succesfully connected to DB"
    exit 0
  fi;
fi;

if notsql $SQLBIN; then
  [[ $QUITE == "0" ]] && echo "Can't connect to DB"
  exit;
fi;



TABLES=`echo '\a \dt' | $SQLBIN |grep 'table' |sed -r -e 's/^[^|]*\|([a-z_0-9]+)\|.*$/\1/'`

for table in $TABLES; do
  DROPS=`echo '\a \d '$table | $SQLBIN |grep -P 'sys_access(mod|put)' |sed -r -e 's/\s*(sys_access(mod|put)).*/ALTER TABLE "'$table'" DROP COLUMN "\1";/'`;
  if [[ $DROPS != "" ]]; then
    echo '-- '$table;
    [[ $RUN == "0" ]] && echo $DROPS | sed 's/; /;\n/'
    [[ $RUN == "1" ]] && echo $DROPS && echo $DROPS | $SQLBIN
  fi
done;

