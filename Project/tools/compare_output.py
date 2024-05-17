import pandas as pd
import numpy as np

serial_data = pd.read_csv("../output/particle_positions_serial.csv")
standard_data = pd.read_csv("../output/particle_positions_standard.csv")
python_data = pd.read_csv("../output/particle_positions_python.csv")

def count_diff(df1, df2):
    df1 = df1.round(10); df2 = df2.round(10) #Round to 10'th decimal
    total_elements = len(df1)
    true = ((df1 == df2)*1).sum()
    print(f"\nMismatching entries (%): \n{100*(total_elements-true)/total_elements}\n")

count_diff(standard_data, python_data)
count_diff(serial_data, standard_data)