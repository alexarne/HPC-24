import pandas as pd
import numpy as np

serial_data = pd.read_csv("../output/particle_positions_serial.csv")
standard_data = pd.read_csv("../output/particle_positions_standard.csv")
python_data = pd.read_csv("../output/particle_positions_python.csv")
standard_data_omp = pd.read_csv("../output/particle_positions_standard_omp.csv")
standard_data_mpi = pd.read_csv("../output/particle_positions_standard_mpi.csv")

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
    MAE(standard_data, python_data)
    print("Optimized vs direct C++ port")
    MAE(standard_data, serial_data)
    print("OpenMP vs optimized serial port")
    MAE(serial_data, standard_data_mpi)
    print("MPI implementation vs optimized serial")

if __name__ == '__main__':
    main()