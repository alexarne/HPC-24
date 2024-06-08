##
# @file compare_output.py
# @brief prints a comparison of the outputs found in ../outputs/
#
import pandas as pd
import numpy as np

serial_data = pd.read_csv("../output/particle_positions_serial.csv")
optimized_data = pd.read_csv("../output/particle_positions_optimized.csv")
python_data = pd.read_csv("../output/particle_positions_python.csv")
optimized_data_omp = pd.read_csv("../output/particle_positions_optimized_omp.csv")
optimized_data_mpi = pd.read_csv("../output/particle_positions_optimized_mpi.csv")

def count_diff(df1, df2):
    df1 = df1.round(12); df2 = df2.round(12) #Round to 10'th decimal
    total_elements = len(df1)
    true = ((df1 == df2)*1).sum()
    print(f"\nMismatching entries (%): \n{100*(total_elements-true)/total_elements}\n")

def MAE(df1, df2):
    diff = df1-df2; diff=diff.to_numpy()[:,1:]
    print(f'MAE: {np.mean(np.abs(diff))}, Max abs error: {np.max(np.abs(diff))}')


def main():
    print("Comparing outputs...")
    print("Direct C++ port vs original Python implementation:")
    MAE(serial_data, python_data)
    print("Optimized C++ port vs original Python implementation:")
    MAE(optimized_data, python_data)
    print("Optimized vs direct C++ port:")
    MAE(optimized_data, serial_data)
    print("OpenMP vs optimized serial port:")
    MAE(optimized_data, optimized_data_omp)
    print("MPI implementation vs optimized serial:")
    MAE(optimized_data, optimized_data_mpi)

if __name__ == '__main__':
    main()