from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import norm

plt.style.use('dark_background') # задаем стииль окна
fig, axs = plt.subplots(nrows= 2 , ncols= 2) # фигура для отрисовки графика

x_min, x_max = 26, 30 # задаем диапозон случайной величины 
mean = 27.9 # мат. ожидание 
stdev = 0.5   # стандартное отклонение

n = 200 # количестов элементов в linspace (массиве)
x = np.linspace(x_min, x_max, n)  # последовательность чисел в заданном интервале 

for ax in axs.flat:
    ax.set_ylim(bottom=0) # задаем начало оси y от 0
    ax.set_ylabel(r'$p$ (TE)') # называем ось y
    ax.set_xlabel('TE values') # называем ось x
    ax.plot(x, norm.pdf(x, loc=mean, scale=stdev))



