#! /usr/bin/python3
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
import datetime
import scipy.signal

plt.rcParams['figure.figsize'] = (13,4)
#plt.rcParams['figure.subplot.left'] = 0.04
plt.rcParams['figure.subplot.right'] = 0.97

plt.rcParams['figure.subplot.left'] = 0.08

#dates,var0,var1 = np.loadtxt('t2020.txt',delimiter=',',skiprows=0,unpack=True)
dates,var0,var1 = np.loadtxt('last24.txt',delimiter=',',skiprows=0,unpack=True)
#np.savez('numpy_file',dates,var0)
#np.save('numpy_dates',dates)
#np.save('numpy_var0',var0)
#dates = np.load('numpy_dates.npy')
#var0  = np.load('numpy_var0.npy')
#dates = np.load('numpy_file.npy')
#var0  = np.load('numpy_file.npy')
#dates,var0  = np.load('numpy_file.npz')
#xdates = [datetime.datetime.strptime('{:014}'.format(int(date)),'%S%M%H%d%m%Y') for date in dates]
xdates = [datetime.datetime.strptime('{:010}'.format(int(date)),'%H%M%S%d%m') for date in dates]
var0=var0/10
var1=var1/10
#T=scipy.signal.savgol_filter(var0,51,3)

fig = plt.figure()
ax = plt.subplot(111)

plt.plot(xdates, var0,'g-',label='Tem1',linewidth=1)
plt.plot(xdates, var1,'b-',label='Tem2',linewidth=1)
#plt.plot(xdates, var0-var1,'r-',label='Tem1',linewidth=1)
#plt.plot(xdates, T,'r-',label='Temp',linewidth=1)

#plt.legend(loc=4)
#plt.ylabel('Wartosc C')
#plt.xlabel('Data')
plt.gcf().autofmt_xdate()
#plt.savefig('plot_from_file.png')
#plt.gca().xaxis.set_major_locator(mdates.DayLocator())
plt.grid()
plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M,%d/%m'))
#plt.ylim([5,30])
#plt.xlim([25, 50])
plt.savefig('temper.png')
plt.show()
