#!/bin/bash

if [ $# -lt 4 ]; then
	echo "usage: $0 <cc.json> <version> <host> <port>"
	exit -1
fi

PARAM_JSON_FILE=$1
PARAM_VERSION=$2
PARAM_HOST=$3
PARAM_PORT=$4

if [ ! -f $PARAM_JSON_FILE ]; then
	echo "json file ${PARAM_JSON_FILE} not exits"
	exit -1
fi

rm -rf ./compiled
mkdir ./compiled

echo -n "making database..."
DATABASE_FILE=$(basename $PARAM_JSON_FILE | sed "s/.json$/_$PARAM_VERSION.sqlite/g")
php make_db.php ${PARAM_JSON_FILE} ./compiled/${DATABASE_FILE}
echo "done"

echo -n "making sources..."
php make_sources/make_source_definition_h.php ${PARAM_HOST} ${PARAM_PORT} ${PARAM_VERSION} > ./compiled/CCDefinition.h
php make_sources/make_source_item_h.php ${PARAM_JSON_FILE} > ./compiled/CCItem.h
php make_sources/make_source_item_m.php ${PARAM_JSON_FILE} > ./compiled/CCItem.m
cp make_sources/templates/CCStorage_h.tpl ./compiled/CCStorage.h
cp make_sources/templates/CCStorage_m.tpl ./compiled/CCStorage.m
cp make_sources/templates/CCNetwork_h.tpl ./compiled/CCNetwork.h
cp make_sources/templates/CCNetwork_m.tpl ./compiled/CCNetwork.m
cp make_sources/templates/CC_h.tpl ./compiled/CC.h
cp make_sources/templates/CC_m.tpl ./compiled/CC.m
tar czf ./compiled.tgz ./compiled
echo "done"
