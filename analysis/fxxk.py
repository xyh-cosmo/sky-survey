from pylab import *

idx_time = 0
idx_lat = 1
idx_lon = 2
idx_satx = 3
idx_saty = 4
idx_satz = 5
idx_sunx = 6
idx_suny = 7
idx_sunz = 8
idx_moonx = 9
idx_moony = 10
idx_moonz = 11
idx_area1 = 12
idx_area2 = 13
idx_isdeep = 14
idx_in_galaxy_disk = 15
idx_trans_angle = 16
idx_exptime = 16
idx_in_sunside = 18
idx_cmg = 19
idx_battery = 20
idx_sun_panel_angle = 21
idx_saa_time = 22
idx_sky_id = 23


#data = loadtxt('../SM_config20180311.dat')
data = loadtxt('../survey_result/SM_config20180228_c5_290_1.dat')

idx_ss = data[:,idx_in_sunside] == 1

ts = data[idx_ss,idx_time]
ang= data[idx_ss,idx_sun_panel_angle]
tang=data[idx_ss,idx_trans_angle]


#plot(ts-ts[0],ang,'.')

#i = 0
#while i <= 23:
#    idx1=i*10000
#    idx2=(i+1)*10000
#    if idx2>len(ts):
#        idx2 = len(ts)
#    hist(ang[idx1:idx2],100)
#    xlabel(r'$\theta$')
#    ylabel(r'Observed times')
#    savefig('fig_'+str(i)+'.png')
#    close()
#    i += 1

hist2d(ts-ts[0],tang,bins=[50,20])
colorbar()
xlabel(r'days')
ylabel(r'$\Delta \theta$')
savefig('time_trans_angle.pdf')

show()

