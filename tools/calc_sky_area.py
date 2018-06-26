from pylab import *
from numpy import *
from mpl_toolkits.basemap import Basemap
from matplotlib.colors import ListedColormap

###############################################################
# def CoordinateSpin_x( coor0, coorR, angle=):



###############################################################

NUM = 100000
phi0 = np.random.rand(NUM)*2*np.pi
u = np.random.rand(NUM)*2-1
theta0 = np.arccos(u) - 0.5*np.pi

# convert to degrees
phi = phi0*180/np.pi
theta = np.arccos(u)*180/np.pi-90

x = cos(theta0)*cos(phi)
y = cos(theta0)*sin(phi)
z = sin(theta0)

# figure(figsize=(8,5))
# m =Basemap(projection='hammer',lat_0=0,lon_0=-180.5,resolution='l')
# m.drawparallels(np.arange(-90.,90.,30.),labels=[1,0,1,1],fontsize=18)
# m.drawmeridians(np.arange(-180.,180.,60.))
# x,y = m(phi,theta);
# cMap = ListedColormap(['b'])
# m.scatter(x,y,1,marker='s',color='b',latlon=False,alpha=0.05);
# show();


# get galactic coordinate {b}

# area = (180/np.pi)**2 * 4*np.pi * sum(np.abs(theta)>=20.5) / NUM

# print('area = %g'%(area))

