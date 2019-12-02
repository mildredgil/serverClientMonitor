#!/bin/sh

ORIGIN=$1
DESTINATION=$2

rsync --delete \
--archive \
--numeric-ids \
--human-readable \
--no-compress \
--whole-file \
--verbose \
--info=progress2 \
$ORIGIN $DESTINATION