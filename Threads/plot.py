from matplotlib import pyplot as plt
import numpy
x = [float(i) for i in input().split(';')[:-1]]
order = numpy.argsort(x)
x = numpy.array(x)[order]
y = numpy.array([float(i) for i in input().split(';')[:-1]])[order]

y_model = numpy.array([float(i) for i in input().split(';')[:-1]])[order]

plt.scatter(x, y)
plt.plot(x, y_model, color="green")
plt.show()
