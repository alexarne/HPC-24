# Course work DD2356 Methods in High Performance Computing, spring 2024.
## David Ahnlund, Emil Gestsson, Alex Gunnarsson

# Project:

To run the different implementations in C++. Run the ```run.sh```bash sricpt with:

```sbatch run.sh```

Constants and other simulation parameters can be changed in ```constants.hpp```.
Data from the simulation is then stored in ./output/*

In the tools directory it is possible to both plot the results and validate compared to the original Python version. Make sure that the constants match in ```constants.hpp```and Python script ```sph.py```. Also note that ```sph.py``` is dependent on existing output from the C++ implementation for initialization.

For results validation:
```
pip install -r requirements.txt
cd tools
python sph.py  #Run python script
python compare_output.py
```
