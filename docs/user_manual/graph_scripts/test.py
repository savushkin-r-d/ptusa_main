import scipy
import numpy as np
import matplotlib.pyplot as plt 
from scipy.stats import norm
 
X = np.arange(26, 30, 0.0001)  # X is an array of x's
Y = norm.pdf(X, loc=27.9, scale=0.5)  # Y is an array of probabilities
fig, ax = plt.subplots()
plt.title('Graph of Standard Normal Dist, mu=0 , sigma=1')
plt.xlabel('X-values')
plt.ylabel('PDF value of Std Normal Dist')
ax.set_ylim(0, 0.8)
plt.xticks([26, 27, 28, 29, 30])
ax.plot(X, Y)

plt.show()