# Project: Smoothed-Particle Hydrodynamics simulation of Toy Star

Optimization project for DD2356 Methods in High Performance Computing at KTH Royal Institute of Technology, based on the Python implementation and write-up by Philip Mocz: https://philip-mocz.medium.com/create-your-own-smoothed-particle-hydrodynamics-simulation-with-python-76e1cec505f1

### Run
To run the different implementations in C++. Run the `run.sh` bash script with:

```
sbatch run.sh
```

Constants and other simulation parameters can be changed in `constants.hpp`.
Data from the simulation is then stored in ./output/*

In the tools directory it is possible to both plot the results and validate compared to the original Python version. Make sure that the constants match in `constants.hpp` and Python script `sph.py`. Also note that `sph.py` is dependent on existing output from the C++ implementation for initialization.

For results validation:
```
pip install -r requirements.txt
cd tools
python sph.py  #Run python script
python compare_output.py
```
### Profiling
To get profiling information from Craypat:
Run the scripts in the profiling directory, e.g.
```
cd profiling
sbatch profiling_MPI_single.sh
python plot_profiling.py
```

### Documentation

The documentation was generated using Doxygen by running `doxygen doxygen_config` in this directory, which is also available on https://alexarne.github.io/HPC-24/.
