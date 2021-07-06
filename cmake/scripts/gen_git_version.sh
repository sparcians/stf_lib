#!/bin/bash

CACHE_FILE=.git_sha_cache

repo_dir=$1
input_file=$2
output_file=$3
var_name=$4

sha=$(cd $repo_dir && git describe --match="" --always --abbrev=40 --dirty)

if [ -f "$CACHE_FILE" ]; then
    if [ "$sha" == "$(cat $CACHE_FILE)" ]; then
        exit 0
    fi
fi

echo $sha > $CACHE_FILE

sed -r "s/@${var_name}@/${sha}/g" $input_file > $output_file
