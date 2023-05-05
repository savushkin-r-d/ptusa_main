import numpy as np
from scipy.stats import norm

class NormDist:

    def __init__(self, x_min=26, x_max =30, mean=27.9, stdev=0.5, n=200):
        self.x_min = x_min # минимум диапозона распределния
        self.x_max = x_max # максимум диапозона распределния
        self.mean = mean # мат. ожидание
        self.stdev = stdev # стандартное отклонение 
        self.x = np.linspace(x_min, x_max, n)  # последовательность чисел в заданном интервале
        self.y= norm.pdf(self.x, loc=mean, scale=stdev) # функция для генерации чисел, соответствующих нормальному распредлению
    
    def set_ax(self, ax):
        ax.set_ylim(bottom=0) # задаем начало оси y от 0
        ax.grid(alpha=0.8,color='b') # добавляем сетку с прозрачностью 0.8 и задаем цвет
        ax.set_ylabel(r'$p$ (TE)') # называем ось y
        ax.set_xlabel('TE values') # называем ось x
        ax.vlines(self.mean, ymin=0, ymax=0.795, linewidths=2.0) # отрисовка линии по координатам x, y и верхней границе
