from matplotlib import pyplot as plt
from norm_dist import NormDist

plt.style.use('dark_background') # задаем стииль окна
normobj = NormDist()
fig, ax = plt.subplots() # фигура для отрисовки графика
normobj.set_ax(ax)
ax.text(28, 0.1,r'$\mu $', fontsize=15, color='w') # ставим обозначение мат. ожидания 
ax.plot(normobj.x, normobj.y) # рисуем график 
plt.show()