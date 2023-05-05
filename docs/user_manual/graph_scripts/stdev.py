from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import norm
from norm_dist import NormDist

plt.style.use('dark_background') # задаем стииль окна
fig, ax = plt.subplots() # фигура для отрисовки графика
normobj1 = NormDist() # создаем объект класса нормального распределения для инициализации параметров 
normobj1.set_ax(ax)  # настраиваем отображение осей 
mu_subtract_stdev = normobj1.mean - normobj1.stdev # начальная координата заливки
mu_plus_stdev = normobj1.mean + normobj1.stdev     # конечная координата заливки
px=np.arange(mu_subtract_stdev, mu_plus_stdev, 0.01)  # диапозон чисел для заливки с шагом 0.01
ax.fill_between(px, norm.pdf(px, loc=normobj1.mean, scale=normobj1.stdev), alpha=0.5, color='blue')   # функция для заливки
ax.text(27.5, 0.1, r'$\mu $ - $\sigma$', fontsize=15, color='w') # добавляем обозначения на график
ax.text(28, 0.1,r'$\mu $ + $\sigma$', fontsize=15, color='w') # добавляем текст на график
ax.plot(normobj1.x, normobj1.y) # рисуем график 
plt.show() # метод для показа графика, без него никак