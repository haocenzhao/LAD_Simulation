# LAD_Simulation

simc: forked from https://github.com/JeffersonLab/simc_gfortran  
geant4: forked from https://github.com/ayerbeg/LAD

## Build LAD Geant4

```tcsh
module unload root geant4 cmake
module load geant4/11.2.2 root/6.30.06-gcc11.4.0 cmake/3.30.2

cd lad_Geant4/build_lad/
cmake ../LAD/
make -j20
```

## Build LAD SimC

```tcsh
module unload root geant4 cmake
module load root/6.30.04-gcc11.4.0

cd simc_dis_lad/
gmake clean
gmake

cd util/root_tree
gmake clean
gmake
```

## DIS Simulation Workflow

The integrated DIS workflow is controlled by `run_simulation.tcsh`.

Before running, edit the user settings at the top of the script:

```tcsh
setenv DIS_ROOT_FILE /path/to/dis_generator.root
setenv DIS_EVENT_COUNT 1000
```

- `DIS_ROOT_FILE`: input DIS generator ROOT file.
- `DIS_EVENT_COUNT`: number of DIS events to process.

Run with:

```bash
./run_simulation.tcsh
```

Each run creates a timestamped output directory:

```txt
Output/origin_output/YYYY-MM-DD-HH-MM-SS/
```

Workflow:

1. Copy the input DIS ROOT file into the timestamped output directory.

2. Convert the DIS ROOT file to a SimC `.dat` file.

   This is done with:

   ```txt
   simc_dis_lad/dis/DIS_root2dat.C
   ```

   The generated `.dat` file, diagnostic `.pdf`, and `.png` files are moved into the output directory.

3. Configure LAD Geant4 for the current run.

   The script updates:

   ```txt
   lad_Geant4/build_lad/LAD.ini
   lad_Geant4/build_lad/dis.mac
   ```

   `LAD.ini` sets the DIS generator mode and the run-local DIS ROOT file.  
   `dis.mac` sets the number of Geant4 events with `/run/beamOn`.

4. Configure SimC for the current run.

   The script updates:

   ```txt
   simc_dis_lad/infiles/dis_hms_e.inp
   simc_dis_lad/infiles/dis_shms_e.inp
   ```

   These input files use the run-local `.dat` file through `dis_datfile`.  
   The event count is set with negative `ngen`, so SimC processes the requested number of DIS input events.

5. Run the three simulations in parallel.

   The script launches:

   ```txt
   lad_g4.csh
   hms_simc.csh
   shms_simc.csh
   ```

   These produce the LAD Geant4, HMS SimC, and SHMS SimC ROOT outputs.

The generated simulation ROOT files, copied DIS ROOT file, converted `.dat` file, and diagnostic plots are saved in the timestamped output directory.

Log files are collected under:

```txt
Output/origin_output/YYYY-MM-DD-HH-MM-SS/log/
```

6. TODO： add these result root file together
