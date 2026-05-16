#!/bin/tcsh -f

setenv RUN_TAG `date "+%Y-%m-%d-%H-%M-%S"`
setenv RUN_OUTPUT_DIR `pwd`/Output/origin_output/$RUN_TAG

mkdir -p $RUN_OUTPUT_DIR

setenv HMS_SIMC_INFILE dis_hms_e
setenv SHMS_SIMC_INFILE dis_shms_e

echo "Output directory: $RUN_OUTPUT_DIR"

echo "Starting LAD Geant4..."
tcsh lad_g4.csh < /dev/null >& /dev/null &

echo "Starting HMS SimC..."
tcsh hms_simc.csh < /dev/null >& /dev/null &

echo "Starting SHMS SimC..."
tcsh shms_simc.csh < /dev/null >& /dev/null &

wait

echo "All simulations finished."

setenv LAD_G4_ROOT `ls -t $RUN_OUTPUT_DIR/HodoFile_*.root | head -n 1`
setenv HMS_SIMC_ROOT $RUN_OUTPUT_DIR/${HMS_SIMC_INFILE}.root
setenv SHMS_SIMC_ROOT $RUN_OUTPUT_DIR/${SHMS_SIMC_INFILE}.root

echo "Output files:"
ls -lh $RUN_OUTPUT_DIR

echo "LAD_G4_ROOT   = $LAD_G4_ROOT"
echo "HMS_SIMC_ROOT = $HMS_SIMC_ROOT"
echo "SHMS_SIMC_ROOT = $SHMS_SIMC_ROOT"