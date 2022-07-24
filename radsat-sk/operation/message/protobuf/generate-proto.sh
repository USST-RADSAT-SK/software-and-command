#!/bin/bash

# generate new source and header files
# NOTE: May have to convert line-endings of script;
# run `dos2unix ./nanopb/generator/nanopb_generator.py` on Linux/WSL
# run `unix2dos ./nanopb/generator/nanopb_generator.py` on Windows
./nanopb/generator/nanopb_generator.py --strip-path proto/*.proto

if [ "$?" -ne 0 ]; then
    echo -e "Failed while generating source and header files. Exiting..."
    exit -1
fi

# delete the old source and header files
rm types/* >/dev/null 2>&1

# move source and header files to protobuf/types/ folder
find proto/ -name *.pb.c -exec mv '{}' "./types/" ";"
find proto/ -name *.pb.h -exec mv '{}' "./types/" ";"

# convert all absolute includes into relative includes
sed -i -E 's/#include "(.*)"/#include <\1>/' types/*
sed -i -E 's/#include <proto\//#include </' types/*
