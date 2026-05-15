#!/bin/tcsh -f

module unload root geant4 cmake
module load geant4/11.2.2 root/6.30.06-gcc11.4.0 cmake/3.30.2

cd lad_Geant4/build_lad

./LAD -m dis.mac >& $RUN_OUTPUT_DIR/lad_geant4.log

set latest_g4_root = `ls -t HodoFile_*.root | head -n 1`

mv $latest_g4_root $RUN_OUTPUT_DIR/