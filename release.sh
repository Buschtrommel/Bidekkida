#!/bin/bash

PRJNAME=Bidekkida

if [ ! $1 ]
then
    echo "Missing version";
    exit 1;
fi

if [ ! "$2" ]
then
    echo "Missing output directory";
    exit 2;
fi

DIR="/tmp/${PRJNAME}-${1}"

rm -rf $DIR

mkdir $DIR

cp -r * $DIR

rm -f "${2}/${PRJNAME}-${1}.tar.xz"
rm -f "${2}/${PRJNAME}-${1}.tar.gz"

pushd /tmp > /dev/null
tar -c --xz -f "${2}/${PRJNAME}-${1}.tar.xz" ${PRJNAME}-${1}
tar -c --gzip -f "${2}/${PRJNAME}-${1}.tar.gz" ${PRJNAME}-${1}
popd > /dev/null

gpg --armor --detach-sign --yes --default-key 3A70A936614C3258 "${2}/${PRJNAME}-${1}.tar.xz"
gpg --armor --detach-sign --yes --default-key 3A70A936614C3258 "${2}/${PRJNAME}-${1}.tar.gz"

exit 0
