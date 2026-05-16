#!/bin/tcsh -f

# =========================
# User settings
# =========================
# path to dis root file
setenv DIS_ROOT_FILE /w/hallc-scshelf2102/c-lad/haocen/Deuteron_DIS/GEMC/generator/10.2/tagged_full/PRC/test_band_tagged_full_PRC_E10.2GeV_recp.root

# num of events: must > 0
setenv DIS_EVENT_COUNT 1000


# =========================
# Initialize module command
# =========================
if ( -e /etc/profile.d/modules.csh ) then
  source /etc/profile.d/modules.csh
else if ( -e /usr/share/Modules/init/tcsh ) then
  source /usr/share/Modules/init/tcsh
else if ( -e /etc/profile.d/lmod.csh ) then
  source /etc/profile.d/lmod.csh
else
  echo "WARNING: cannot initialize module command automatically."
endif

# =========================
# Paths and run tag
# =========================
set TOP_DIR = `pwd`

setenv RUN_TAG "`date "+%Y-%m-%d-%H-%M-%S"`_${DIS_EVENT_COUNT}Events"
setenv RUN_OUTPUT_DIR ${TOP_DIR}/Output/origin_output/${RUN_TAG}

setenv HMS_SIMC_INFILE dis_hms_e
setenv SHMS_SIMC_INFILE dis_shms_e

setenv SIMC_DIR ${TOP_DIR}/simc_dis_lad
setenv SIMC_DIS_DIR ${SIMC_DIR}/dis
setenv LAD_G4_BUILD_DIR ${TOP_DIR}/lad_Geant4/build_lad

mkdir -p $RUN_OUTPUT_DIR

if ( ! -e $DIS_ROOT_FILE ) then
  echo "ERROR: DIS_ROOT_FILE does not exist:"
  echo "  $DIS_ROOT_FILE"
  exit 1
endif

if ( $DIS_EVENT_COUNT <= 0 ) then
  echo "ERROR: DIS_EVENT_COUNT must be positive."
  echo "  DIS_EVENT_COUNT = $DIS_EVENT_COUNT"
  exit 1
endif

foreach req_file ( $SIMC_DIS_DIR/DIS_root2dat.C $LAD_G4_BUILD_DIR/LAD.ini $LAD_G4_BUILD_DIR/dis.mac $SIMC_DIR/infiles/${HMS_SIMC_INFILE}.inp $SIMC_DIR/infiles/${SHMS_SIMC_INFILE}.inp )
  if ( ! -e $req_file ) then
    echo "ERROR: required file does not exist:"
    echo "  $req_file"
    exit 1
  endif
end

set DIS_ROOT_BASE = `basename $DIS_ROOT_FILE .root`

setenv RUN_DIS_ROOT_FILE ${RUN_OUTPUT_DIR}/${DIS_ROOT_BASE}.root
setenv DIS_DAT_FILE ${RUN_OUTPUT_DIR}/${DIS_ROOT_BASE}.dat

@ SIMC_EVENT_COUNT = -1 * $DIS_EVENT_COUNT
setenv SIMC_EVENT_COUNT $SIMC_EVENT_COUNT

echo "Output directory: $RUN_OUTPUT_DIR"
echo "DIS_ROOT_FILE    = $DIS_ROOT_FILE"
echo "RUN_DIS_ROOT_FILE= $RUN_DIS_ROOT_FILE"
echo "DIS_DAT_FILE     = $DIS_DAT_FILE"
echo "G4 events        = $DIS_EVENT_COUNT"
echo "SimC events      = $SIMC_EVENT_COUNT"

# =========================
# Copy DIS ROOT file into this run directory
# =========================
echo "Copying DIS ROOT file..."
cp -f $DIS_ROOT_FILE $RUN_DIS_ROOT_FILE

if ( $status != 0 ) then
  echo "ERROR: failed to copy DIS ROOT file."
  exit 1
endif

# =========================
# Convert DIS ROOT file to SIMC dat file
# =========================
echo "Converting DIS ROOT file to dat..."

set ROOT2DAT_TEMPLATE = $SIMC_DIS_DIR/DIS_root2dat.C
set ROOT2DAT_MACRO = $RUN_OUTPUT_DIR/DIS_root2dat.C

setenv ROOT2DAT_INPUT $RUN_DIS_ROOT_FILE

perl -0pe 's#const std::string inFile\s*=\s*".*?";#"const std::string inFile =\n    \"" . $ENV{ROOT2DAT_INPUT} . "\";"#se' $ROOT2DAT_TEMPLATE > $ROOT2DAT_MACRO

if ( $status != 0 ) then
  echo "ERROR: failed to prepare temporary DIS_root2dat.C."
  exit 1
endif

cd $SIMC_DIS_DIR

module unload root geant4 cmake
module load root/6.30.04-gcc11.4.0

echo "ROOT used for DIS_root2dat:"
which root
root-config --version

rm -f ${DIS_ROOT_BASE}.dat ${DIS_ROOT_BASE}_pidrec.pdf ${DIS_ROOT_BASE}_pidrec.png

root -l -b -q $ROOT2DAT_MACRO >& $RUN_OUTPUT_DIR/DIS_root2dat.log

if ( $status != 0 ) then
  echo "ERROR: DIS_root2dat.C failed. See:"
  echo "  $RUN_OUTPUT_DIR/DIS_root2dat.log"
  exit 1
endif

if ( ! -e ${DIS_ROOT_BASE}.dat ) then
  echo "ERROR: expected dat file was not produced:"
  echo "  $SIMC_DIS_DIR/${DIS_ROOT_BASE}.dat"
  exit 1
endif

mv -f ${DIS_ROOT_BASE}.dat $RUN_OUTPUT_DIR/

if ( -e ${DIS_ROOT_BASE}_pidrec.pdf ) then
  mv -f ${DIS_ROOT_BASE}_pidrec.pdf $RUN_OUTPUT_DIR/
endif

if ( -e ${DIS_ROOT_BASE}_pidrec.png ) then
  mv -f ${DIS_ROOT_BASE}_pidrec.png $RUN_OUTPUT_DIR/
endif

cd $TOP_DIR
rm -f $ROOT2DAT_MACRO

# =========================
# Update Geant4 and SimC input files
# =========================
echo "Updating Geant4 and SimC input files..."

setenv G4_DIS_ROOT_FILE $RUN_DIS_ROOT_FILE
setenv G4_EVENT_COUNT $DIS_EVENT_COUNT

perl -0pi -e 's#^GeneratorCase\s+.*#GeneratorCase 3#m' $LAD_G4_BUILD_DIR/LAD.ini
perl -0pi -e 's#^DISrootFile\s+.*#"DISrootFile " . $ENV{G4_DIS_ROOT_FILE}#me' $LAD_G4_BUILD_DIR/LAD.ini
perl -0pi -e 's#^/run/beamOn\s+.*#"/run/beamOn " . $ENV{G4_EVENT_COUNT}#me' $LAD_G4_BUILD_DIR/dis.mac

foreach simc_inp ( $SIMC_DIR/infiles/${HMS_SIMC_INFILE}.inp $SIMC_DIR/infiles/${SHMS_SIMC_INFILE}.inp )
  perl -0pi -e 's#^\s*ngen\s*=.*#"  ngen = " . $ENV{SIMC_EVENT_COUNT} . "       ; +N: require N accepted events; -N: process exactly N input events"#me' $simc_inp
  perl -0pi -e 's#^\s*dis_datfile\s*=.*#"  dis_datfile = " . $ENV{DIS_DAT_FILE}#me' $simc_inp
end

# =========================
# Run three simulations
# =========================
echo "Starting LAD Geant4..."
tcsh -c "source lad_g4.csh" < /dev/null >& /dev/null &

echo "Starting HMS SimC..."
tcsh -c "source hms_simc.csh" < /dev/null >& /dev/null &

echo "Starting SHMS SimC..."
tcsh -c "source shms_simc.csh" < /dev/null >& /dev/null &

wait

echo "All simulations finished."

setenv LAD_G4_ROOT `ls -t $RUN_OUTPUT_DIR/HodoFile_*.root | head -n 1`
setenv HMS_SIMC_ROOT $RUN_OUTPUT_DIR/${HMS_SIMC_INFILE}.root
setenv SHMS_SIMC_ROOT $RUN_OUTPUT_DIR/${SHMS_SIMC_INFILE}.root

mkdir -p $RUN_OUTPUT_DIR/log
foreach logfile ( $RUN_OUTPUT_DIR/*.log )
  if ( -e $logfile ) then
    mv -f $logfile $RUN_OUTPUT_DIR/log/
  endif
end

echo "Output files:"
#ls -lh $RUN_OUTPUT_DIR
echo "LAD_G4_ROOT    = $LAD_G4_ROOT"
echo "HMS_SIMC_ROOT  = $HMS_SIMC_ROOT"
echo "SHMS_SIMC_ROOT = $SHMS_SIMC_ROOT"
echo "DIS_ROOT_COPY  = $RUN_DIS_ROOT_FILE"
echo "DIS_DAT_FILE   = $DIS_DAT_FILE"


# =========================
# Merge SimC and LAD Geant4 outputs
# =========================
echo "Merging SimC and LAD Geant4 outputs..."

setenv HMS_OUTPUT_DIR ${TOP_DIR}/Output/hms/${RUN_TAG}
setenv SHMS_OUTPUT_DIR ${TOP_DIR}/Output/shms/${RUN_TAG}

mkdir -p $HMS_OUTPUT_DIR/log
mkdir -p $SHMS_OUTPUT_DIR/log

setenv HMS_MERGED_ROOT ${HMS_OUTPUT_DIR}/hms_g4.root
setenv SHMS_MERGED_ROOT ${SHMS_OUTPUT_DIR}/shms_g4.root

setenv MERGE_MACRO ${TOP_DIR}/merge_simc_g4.C

echo "Merging HMS SimC with LAD Geant4..."
root -l -b -q ${MERGE_MACRO}\(\"${HMS_SIMC_ROOT}\",\"${LAD_G4_ROOT}\",\"${HMS_MERGED_ROOT}\",\"hms\"\) >& ${HMS_OUTPUT_DIR}/log/merge_hms_g4.log

if ( $status != 0 ) then
  echo "ERROR: HMS merge failed. See:"
  echo "  ${HMS_OUTPUT_DIR}/log/merge_hms_g4.log"
  exit 1
endif

echo "Merging SHMS SimC with LAD Geant4..."
root -l -b -q ${MERGE_MACRO}\(\"${SHMS_SIMC_ROOT}\",\"${LAD_G4_ROOT}\",\"${SHMS_MERGED_ROOT}\",\"shms\"\) >& ${SHMS_OUTPUT_DIR}/log/merge_shms_g4.log

if ( $status != 0 ) then
  echo "ERROR: SHMS merge failed. See:"
  echo "  ${SHMS_OUTPUT_DIR}/log/merge_shms_g4.log"
  exit 1
endif

echo "Merged output files:"
echo "HMS_MERGED_ROOT  = $HMS_MERGED_ROOT"
echo "SHMS_MERGED_ROOT = $SHMS_MERGED_ROOT"