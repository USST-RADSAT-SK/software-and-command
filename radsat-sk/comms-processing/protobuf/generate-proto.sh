#!/bin/bash

# generate new source and header files
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
