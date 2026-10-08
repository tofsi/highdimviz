import numpy as np
import pandas as pd

cov = 10000000*np.matrix([[2.0, 0.0, 0.5, 0.5],
       [0.0, 1.0, 0.7, -0.3],
       [0.5, 0.7, 1.5, 0.5],
       [0.5, -0.3, 0.5, 1.0]])
means = 10000000*np.array([1.0, 0.0, 2.0, 0.5])
points = np.random.multivariate_normal(mean=means,cov=cov, size = 1000000)

names = ["first", "second", "third", "fourth"]

df = pd.DataFrame(points, columns=names)
df.to_csv("data/gaussian_example.csv", index=False)