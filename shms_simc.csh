#!/bin/tcsh -f

module unload root geant4 cmake
module load root/6.30.04-gcc11.4.0

cd simc_dis_lad

echo $SHMS_SIMC_INFILE | ./run_simc_tree $SHMS_SIMC_INFILE >& $RUN_OUTPUT_DIR/shms_simc.log

mv worksim/$SHMS_SIMC_INFILE.root $RUN_OUTPUT_DIR/