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

for ax in axs.flat:  #  в цикле выполняем одинаковые операции для каждого графика
    ax.set_ylim(bottom=0) # задаем начало оси y от 0
    ax.set_ylabel(r'$p$ (TE)') # называем ось y
    ax.set_xlabel('TE values') # называем ось x
    ax.plot(x, norm.pdf(x, loc=mean, scale=stdev))

axs[0,0].set_title(r'$p(X) < 27$')
axs[0,1].set_title(r'$p(X) > 28$')
axs[1,0].text(28.5, 0.6, r'$p(27 < X < 28)$', fontsize=10, color='w') # добавляем обозначения на график
axs[1,1].text(28.5, 0.6, r'$p(26 < X < 30)$', fontsize=10, color='w') # добавляем обозначения на график

px1=np.arange(26, 27, 0.01)
square1= norm.sf(x=27, loc=mean, scale=stdev) # вычисляем площадь графика, которая больше 27
axs[0,0].fill_between(px1, norm.pdf(px1, loc=mean, scale=stdev), alpha=0.5, color='m')  # заливка площади графика от 26 до 27
px_value1 = 1 - square1 # площадь графика от 26 до 27 
axs[0,0].text(27, 0.1, round(px_value1, 2), fontsize=15, color='c') # отмечаем на графике значение площади

px2=np.arange(28, 30, 0.01)
square2=norm.sf(x=28, loc=mean, scale=stdev) # вычисляем площадь графика, которая больше 28
axs[0,1].fill_between(px2, norm.pdf(px2, loc=mean, scale=stdev), alpha=0.5, color='m') # заливка площади графика от 28 до 30
axs[0,1].text(28, 0.3, round(square2, 2), fontsize=15, color= 'c')

px3=np.arange(27, 28, 0.01)
square3=norm(mean,stdev).cdf(28) - norm(mean, stdev).cdf(27) # вычисляем разность площади графика, в диапозоне [26,27]  от площади [28,30]
axs[1,0].fill_between(px3, norm.pdf(px3, loc=mean, scale=stdev), alpha=0.5, color='m')  # заливка площади графика от 27 до 28
axs[0,1].text(28, 0.3, round(square2, 2), fontsize=15, color= 'c')
axs[1,0].text(28, 0.3, round(square3, 2), fontsize=15, color='c')

px4=np.arange(26, 30, 0.01)
square4=norm(mean, stdev).sf(26) # вычисляем площадь графика в диапозоне [26,30]
axs[1,1].fill_between(px4, norm.pdf(px4, loc=mean, scale=stdev), alpha=0.5, color='m') # заливка площади графика от 26 до 30
axs[1,1].text(28, 0.3, round(square4, 2), fontsize=15, color='c')

plt.show()
