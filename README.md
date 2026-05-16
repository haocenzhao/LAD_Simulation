# LAD_Simulation
simc: forked from https://github.com/JeffersonLab/simc_gfortran;  
geant4: forked from https://github.com/ayerbeg/LAD;



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

The workflow first copies the input DIS ROOT file into the timestamped output directory, then runs `simc_dis_lad/dis/DIS_root2dat.C` to convert the ROOT file into a SimC `.dat` file. The script then updates the LAD Geant4 and SimC input files with the run-specific ROOT/dat paths and event count. Finally, LAD Geant4, HMS SimC, and SHMS SimC are launched in parallel.

The generated simulation ROOT files, copied DIS ROOT file, converted `.dat` file, and diagnostic plots are saved in the timestamped output directory. Log files are collected under:

```txt
Output/origin_output/YYYY-MM-DD-HH-MM-SS/log/
```



