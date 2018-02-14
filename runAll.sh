#!/bin/bash

make
cd alpha-expan-str
./run.sh

cd ../ab-swp-str
./run.sh

cd ../GRSA_stereo
./run.sh

cd ..