#!/bin/sh

# Design files
DESIGN_PATH=./Design

# create a directory for design 
mkdir -p $DESIGN_PATH

# remove the old files
rm $DESIGN_PATH/*.cpp
rm $DESIGN_PATH/*.h

# gather all the files for the design
find . -name "*.cpp" -exec cp -f -t $DESIGN_PATH {} +
find . -name   "*.h" -exec cp -f -t $DESIGN_PATH {} +
