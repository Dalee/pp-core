#!/bin/bash
PROJECT_ROOT_TMP=$(dirname "${BASH_SOURCE[0]}")
PROJECT_ROOT=$(readlink -f "$PROJECT_ROOT_TMP/../../")

LIBPP=$(find "$PROJECT_ROOT/libpp" \
	\( -name '*.inc' \
	-or -name '*.php' \
	-or -name '*.tmpl' \
	-or -name '*.js' \
	-or -name '*.sql' \
	-or -name '*.yaml' \
	-or -name '*.xml' \) \
	-and \( ! -regex '.*/\..*' \))

LOCAL=$(find "$PROJECT_ROOT/local" \
	\( -name '*.inc' \
	-or -name '*.php' \
	-or -name '*.tmpl' \
	-or -name '*.js' \
	-or -name '*.sql' \
	-or -name '*.yaml' \
	-or -name '*.xml' \) \
	-and \( ! -regex '.*/\..*' \))

XML_SED=$(find "$PROJECT_ROOT/local" \
	-name '*.xml' \
	-and \( ! -regex '.*/\..*' \))

for f in $LOCAL; do
	recode koi8-r..utf-8 $f
done
#for f in $LIBPP; do
#	recode koi8-r..utf-8 $f
#done
for f in $XML_SED; do
	sed "s/iso-8859-1/utf-8/" -i $f 
done
