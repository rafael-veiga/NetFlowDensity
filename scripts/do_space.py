import pyfileconverter
import numpy as np
import sys
import pickle as pk

file_in = sys.argv[1]
file_space = sys.argv[2]
file_out = sys.argv[3]

# Aqui você pode chamar as funções, por exemplo:
np_array, col_names = pyfileconverter.file_to_numpy(file_in)
with open(file_space, 'rb') as file:
    pca_space, pacmap_space,sample = pk.load(file)

np_array2 = pca_space.transform(np_array)
res = pacmap_space.transform(np_array2,sample)
combined = np.concatenate([res, np_array], axis=1)
col_names = ["dim1","dim2"]+ col_names
pyfileconverter.numpy_to_file(file_out,combined,col_names)