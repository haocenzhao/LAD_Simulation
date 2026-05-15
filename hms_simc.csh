#!/bin/tcsh -f

module unload root geant4 cmake
module load root/6.30.04-gcc11.4.0

cd simc_dis_lad

echo $HMS_SIMC_INFILE | ./run_simc_tree $HMS_SIMC_INFILE >& $RUN_OUTPUT_DIR/hms_simc.log

mv worksim/$HMS_SIMC_INFILE.root $RUN_OUTPUT_DIR/