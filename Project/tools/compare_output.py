import pandas as pd
import numpy as np

serial_data = pd.read_csv("../output/particle_positions_serial.csv")
standard_data = pd.read_csv("../output/particle_positions_standard.csv")
omp_data = pd.read_csv("../output/particle_positions_omp.csv")

def count_diff(df1, df2):
    total_elements = len(omp_data)
    true = ((df1 == df2)*1).sum()
    print(f"\nMismatching entries (%): \n{100*(total_elements-true)/total_elements}\n")

count_diff(omp_data, standard_data)
count_diff(serial_data, standard_data)