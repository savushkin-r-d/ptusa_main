from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import norm

plt.style.use('dark_background') # задаем стииль окна
fig, ax = plt.subplots() # фигура для отрисовки графика

x_min, x_max = 26, 30 # задаем диапозон случайной величины 
mean = 27.9 # мат. ожидание 
stdev = 0.5   # стандартное отклонение

n = 200 # количестов элементов в linspace (массиве)
x = np.linspace(x_min, x_max, n)  # последовательность чисел в заданном интервале 
ax.set_ylim(bottom=0) # задаем начало оси y от 0
ax.grid(alpha=0.8,color='b') # добавляем сетку с прозрачностью 0.8 и задаем цвет
ax.vlines(x=mean, ymin=0, ymax=0.795, linewidths=2.0) # отрисовка линии по координатам x, y и верхней границе
ax.set_ylabel(r'$p$ (TE)') # называем ось y
ax.set_xlabel('TE values') # называем ось x
ax.vlines(x=mean, ymin=0, ymax=0.795, linewidths=2.0) # отрисовка линии по координатам x, y и верхней границе

mu_subtract_stdev = mean - stdev # начальная координата заливки
mu_plus_stdev = mean + stdev     # конечная координата заливки
px=np.arange(mu_subtract_stdev, mu_plus_stdev, 0.01)  # диапозон чисел для заливки с шагом 0.01
ax.fill_between(px, norm.pdf(px, loc=mean, scale=stdev), alpha=0.5, color='blue')   # функция для заливки
ax.text(27.5, 0.1, r'$\mu $ - $\sigma$', fontsize=15, color='w') # добавляем обозначения на график
ax.text(28, 0.1,r'$\mu $ + $\sigma$', fontsize=15, color='w')
ax.plot(x, norm.pdf(x, loc=mean, scale=stdev)) # рисуем график 
plt.show() # метод для показа графика, без него никак