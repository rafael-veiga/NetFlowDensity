# test.py
import pyfileconverter
#import numpy as np
import sys
import pickle as pk
import pacmap
from sklearn.decomposition import PCA 

samplefile = sys.argv[1]
file_out = sys.argv[2]
# Aqui você pode chamar as funções, por exemplo:
np_array, col_names = pyfileconverter.file_to_numpy(samplefile)
pca_space = PCA()
pca_space.fit(np_array)
pacmap_space = pacmap.PaCMAP(n_components=2, n_neighbors=15, MN_ratio=0.5, FP_ratio=2.0)
pacmap_space.fit(np_array)

with open(file_out, 'wb') as file: 
    pk.dump((pca_space,pacmap_space), file) 


