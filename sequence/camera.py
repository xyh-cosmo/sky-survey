#coding=utf-8

#############################################################################################
# 作者：许优华
# 日期：2018年1月31日
#
# 一些必要的说明：
# 1）平台机动包括“转动指向到下一个目标天区”和“稳像”
#	平台的转动在数据开始读取后0.1秒启动，稳像在平台机动结束前16秒开始，并假定持续
#   时间为15.5秒。
# 2）CCD焦面的刷新安排在平台机动结束前2秒开始，仅刷新一次，持续时间为1秒。
#############################################################################################

#############################################################################################
# 相机事件的定义：（每一个事件用一个数字来表示）
# 注意：以下10个事件是按照发生时间的先后顺序所排列的！
#############################################################################################

# 打开快门：1 （打开快门的时刻定义为一个拍摄周期的开始）
# 快门全开：2 （快门完全打开的时刻定义为曝光的开始）
# 关闭快门：3 （快门开始关闭的时刻定义为曝光的结束）
# 快门全关：4  (快门完全关闭后立即进行图像数据的读取，假定读取持续的时间为40秒)
# 平台机动：5 （假定平台机动发生在数据读取开始后0.1秒）
# 结束读取：6
# 开始稳像：7 （假定平台机动结束前16秒开始稳像）
# 开始刷新：8 （假定刷新在稳像结束前两秒开始，并只刷新一次，所需时间为1秒）
# 结束刷新：9
# 结束稳像：10（假定稳像所需时间固定为15.5秒,稳像结束后剩余0.5秒到达本次拍摄周期的末尾）


#############################################################################################
# 组部件标号及状态说明：
# 注意：巡天观测时所有部件都加电工作！
# 主焦面前端电箱		A（0:不工作；1:工作，具体功耗暂时未定）
# 主焦面数传电箱		B（0:不工作；1:工作，具体功耗暂时未定）
# 主焦面电源箱			C（0:不工作；1:工作，具体功耗暂时未定）
# 主焦面控制箱			D（0:不工作；1:工作，具体功耗暂时未定）
# 短波红外电箱			E（0:不工作；1:工作，10W；2:工作，25W）
# 短波红外制冷			F（0:不工作；1:工作，210W；2:工作，222W）
# 主焦面制冷			G（0:不工作；1:工作，633W；2:工作，660W；3:工作，6663W；4:工作，690W；5:工作750W）
# 热控				H（0:不工作；1:工作，28W；2:工作，31W）
# 定标光源组件			I（0:不工作；1:前期，0W，2:中后期，0.6W）
# 快门组件			J（0:完全关闭状态，75.6W；1:正在打开，75.6W；2:完全打开，75.6W；3:正在关闭，75.6W）
# 像旋微调			K（0:不工作；1:工作，35W）
# 主电控箱			L（0:不工作；1:工作，34.4W)
#
# 目前仅“快门组件”的状态完全清楚
#############################################################################################

# -------------------------------------------------------------------------------------------
import os,sys
from pylab import *

sec_per_day = 24.0*3600.


# -------------------------------------------------------------------------------------------
def GetTransTime(angle):
	data = array([[1,80],[20,127],[45,196],[180,581]])
	if angle > 180.:
		print 'trans_angle is larger than 180.'
		sys.exit(0)
	time = 0.
	if angle < 1.:
		time = 70.0
	elif angle == 1.:
		time = 80.0
	else:
		for i in range(3):
			if angle > data[i,0] and angle <= data[i+1,0]:
				time = data[i,1] \
					+ (angle-data[i,0])*(data[i+1,1]-data[i,1])/(data[i+1,0]-data[i,0])
				break

	return time

# -------------------------------------------------------------------------------------------
def check_time_interval(time_current,
						time_next,
						shutter_time,
						exposure_time,
						trans_angle
						):
	transTime = GetTransTime(trans_angle)
	time_interval = (time_next - time_current)*sec_per_day
	time_diff = time_interval - (2.*shutter_time+exposure_time+transTime)
	if time_diff > 0:
		return True
	else:
		print '\n--- Error ---'
		print 'time_exposure = %g'%(exposure_time)
		print 'time_interval = %g'%(time_interval)
		print 'angle         = %g'%(trans_angle)
		print 'time_trans    = %g'%(transTime)
		print 'time_needed   = %g'%(2.*shutter_time+exposure_time+transTime)
		print 'time diff     = %g'%(time_diff)
		return False

# ----------------------------------------------------------------------------------------------------------

def print_state_head():
#	state_head = "{:<20}".format('time cost (seconds)')+'\t '
	state_head = "{:<20}".format('事件持续时间  (seconds)')+'\t '
	state_head += 'sA\t '
	state_head += 'sB\t '
	state_head += 'sC\t '
	state_head += 'sD\t '
	state_head += 'sE\t '
	state_head += 'sF\t '
	state_head += 'sG\t '
	state_head += 'sH\t '
	state_head += 'sI\t '
	state_head += 'sJ\t '
	state_head += 'sK\t '
	state_head += 'sL'

	return state_head

def print_state(time_cost=-1,
				sA=0,sB=0,sC=0,sD=0,
				sE=0,sF=0,sG=0,sH=0,
				sI=0,sJ=0,sK=0,sL=0):
	state = "{:<20}".format(str(time_cost))+'\t '
	state += str(sA)+'\t '
	state += str(sB)+'\t '
	state += str(sC)+'\t '
	state += str(sD)+'\t '
	state += str(sE)+'\t '
	state += str(sF)+'\t '
	state += str(sG)+'\t '
	state += str(sH)+'\t '
	state += str(sI)+'\t '
	state += str(sJ)+'\t '
	state += str(sK)+'\t '
	state += str(sL)

	return state

def produce_one_sequence(time_current,
						 time_next,
						 shutter_time,
						 exposure_time,
						 trans_angle):
	sequence = ""
	
	# step 1) 快门开始打开
	event_time = time_current
	state = print_state(time_cost=1.5,
						sA=1,sB=0,sC=1,sD=1,
						sE=2,sF=2,sG=1,sH=1,
						sI=0,sJ=1,sK=1,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t1\t\t' + state + '\n'
	sequence += temp

	# step 2) 快门完全打开，开始曝光
	event_time = time_current + shutter_time/sec_per_day
	state = print_state(time_cost=round(exposure_time,5),
						sA=1,sB=0,sC=1,sD=1,
						sE=2,sF=2,sG=3,sH=1,
						sI=0,sJ=2,sK=1,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t2\t\t' + state + '\n'
	sequence += temp

	# step 3) 曝光结束，快门开始关闭
	event_time = time_current + (shutter_time+exposure_time)/sec_per_day
	state = print_state(time_cost=1.5,
						sA=1,sB=0,sC=1,sD=1,
						sE=2,sF=2,sG=3,sH=1,
						sI=0,sJ=3,sK=1,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t3\t\t' + state + '\n'
	sequence += temp

	# step 4) 快门完全关闭，开始读取 （假定读取数据所需时间为40秒）
	event_time = time_current + (shutter_time*2+exposure_time)/sec_per_day
	state = print_state(time_cost=40,
						sA=1,sB=1,sC=1,sD=1,
						sE=2,sF=2,sG=4,sH=2,
						sI=0,sJ=0,sK=0,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t4\t\t' + state + '\n'
	sequence += temp

	# step 5) 平台机动开始，指向下一个目标天区
	event_time = time_current + (shutter_time*2+exposure_time+0.1)/sec_per_day
	transTime = GetTransTime(trans_angle) - 0.1 #减去0.1秒是为了补偿前面增加的0.1秒
	state = print_state(time_cost=round(transTime,5),
						sA=1,sB=1,sC=1,sD=1,
						sE=1,sF=2,sG=5,sH=2,
						sI=0,sJ=0,sK=0,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t5\t\t' + state + '\n'
	sequence += temp

	# step 6) 平台机动结束前16秒开始稳像
	event_time = time_current + (shutter_time*2+exposure_time + 0.1 + (transTime-16))/sec_per_day
	state = print_state(time_cost=15,
						sA=0,sB=0,sC=1,sD=1,
						sE=1,sF=1,sG=2,sH=2,
						sI=0,sJ=0,sK=0,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t6\t\t' + state + '\n'
	sequence += temp

	# step 7) 平台机动结束前2秒开始刷新
	event_time = time_current + (shutter_time*2+exposure_time + 0.1 + (transTime-2))/sec_per_day
	state = print_state(time_cost=1,
						sA=0,sB=0,sC=1,sD=1,
						sE=1,sF=1,sG=3,sH=2,
						sI=0,sJ=0,sK=1,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t7\t\t' + state + '\n'
	sequence += temp

	# step 8) 平台机动结束前1秒开始刷新结束
	event_time = time_current + (shutter_time*2+exposure_time + 0.1 + (transTime-1))/sec_per_day
	state = print_state(time_cost=1,
						sA=0,sB=0,sC=1,sD=1,
						sE=1,sF=1,sG=3,sH=2,
						sI=0,sJ=0,sK=1,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t8\t\t' + state + '\n'
	sequence += temp

	# step 9) 平台机动结束、稳像结束，进入下一个拍摄周期
	event_time = time_current + (shutter_time*2+exposure_time + 0.1 + (transTime-0.5))/sec_per_day
	state = print_state(time_cost=0.5,
						sA=0,sB=0,sC=1,sD=1,
						sE=1,sF=1,sG=3,sH=2,
						sI=0,sJ=0,sK=1,sL=1)
	temp = "{:<20}".format(str(event_time)) + '\t9\t\t' + state + '\n'
	sequence += temp

	# sequence += '\n'

	return sequence

def generate_sequence(time_start,
					  time_exposure,
					  trans_angle,
					  shutter_time=1.5,
					  seq_filename='action_seq.txt'):
	fp = open(seq_filename,'w')
#	print >> fp, '# event time (JD)       event   ' + print_state_head()
	print >> fp, '# 事件触发时间 (JD)     事件   ' + print_state_head()
	seq_len = len(time_start)-1
	cnt = 0
	while cnt < seq_len:
	# while cnt <= 10:
		print 'counting : %7d'%(cnt)
		time_current	= time_start[cnt]
		time_next	 	= time_start[cnt+1]
		exposure_time	= time_exposure[cnt] # 这里将快门时间包含在曝光时间内
		angle 			= trans_angle[cnt+1]
		shutter_time	= shutter_time
		
		# 修正快门时间，从150秒或250秒减去快门开闭的时间（3秒）
		exposure_time -= 3.0

		# if check_time_interval(time_current,time_next,shutter_time,exposure_time,angle) == True:

		seq = produce_one_sequence(time_current,time_next,shutter_time,exposure_time,angle)
		print >> fp, '%s'%(seq)
		cnt += 1

	fp.close()

####################################################################################################
raw_data = loadtxt('temp.txt')

time_start 		= raw_data[:,0]
time_exposure	= raw_data[:,16]
trans_angle		= raw_data[:,17]
shutter_time	= 1.5

generate_sequence(time_start,time_exposure,trans_angle,shutter_time)
