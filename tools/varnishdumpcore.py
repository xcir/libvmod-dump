# coding: utf-8

import varnishapi,time,os,sys,syslog,traceback,datetime


class varnishDump:
	def __init__(self, opts):
		self.logdir   = os.path.dirname(__file__).rstrip('/') + '/log/'
		self.outproxy = 0
		self.buf      = {}
		vops = [
			'-g', 'vxid',
			'-q', 'Debug:VMD-DUMP',
			'-i', 'Debug,ReqStart,End,Timestamp'
		]
		arg = {}
		for o,a in opts:
			if   o == '-o':
				if not os.path.isdir(a):
					print "%s does not exist" % (a)
					exit(1)
				if not os.access(a, os.R_OK or os.W_OK):
					print "%s permission denied" % (a)
					exit(1)
					
			elif o == '-p':
				self.outproxy = 1
			elif o == '--sopath':
				arg["sopath"] = a
			elif o == '-n':
					vops += ['-n', a]
		
		arg["opt"]   = vops
		self.vap     = varnishapi.VarnishLog(**arg)
		if self.vap.error:
			print self.vap.error
			exit(1)

	def execute(self):
		#connect varnishapi
		while 1:
			ret = self.vap.Dispatch(self.vapCallBack)
			if 0 == ret:
				time.sleep(0.5)
		
	def prnData(self,vxid):
		d = self.buf[vxid]
		tf = "%s_%d" % (d['time'], vxid)
		print self.buf[vxid]
		for v in ['req','resp']:
			if d[v]['raw']:
				fname = "%s%s_%s.dump" % (self.logdir , tf , v)
				f = open(fname, 'w')
				f.write(d[v]['raw'])
				f.close()
	
	def vapCallBack(self,vap,cbd,priv):
		level       = cbd['level']
		vxid        = cbd['vxid']
		vxid_parent = cbd['vxid_parent']
		type        = cbd['type']
		tag         = cbd['tag']
		data        = cbd['data']
		isbin       = cbd['isbin']
		length      = cbd['length']
		t_tag = vap.VSL_tags[tag]
		var   = vap.vut.tag2VarName(t_tag,data)

		if   t_tag == 'Timestamp' and data[:4] == 'Req:' :
			self.buf[vxid] = {
				'addr' : None,
				'port' : None,
				'time' : None,
				'req'  : {'val':'','raw':''},
				'resp' : {'val':'','raw':''},
				'cur'  : None,
				}
			spl   = data[:length-1].split(' ',3)
			self.buf[vxid]['time'] = time.strftime("%Y%m%d-%H%M%S", datetime.datetime.fromtimestamp(float(spl[1])).timetuple())
		elif t_tag == 'ReqStart':
			spl   = data[:length-1].split(' ')
			self.buf[vxid]['addr'] = spl[0]
			self.buf[vxid]['port'] = spl[1]
		elif t_tag == 'End':
			#Flush
			self.prnData(vxid)
			del self.buf[vxid]
		elif t_tag == 'Debug':
			if data[:8] == "VMD-DUMP":
				if   data[:10] == "VMD-DUMP-S":
					if   data[12:length-1] == 'RES':
						self.buf[vxid]['cur'] = 'resp'
					elif data[12:length-1] == 'REQ':
						self.buf[vxid]['cur'] = 'req'
					elif data[12:length-1] == 'END':
						self.buf[vxid]['cur'] = None
				elif data[:10] == "VMD-DUMP-V":
					self.buf[vxid][self.buf[vxid]['cur']]['val'] = self.buf[vxid][self.buf[vxid]['cur']]['val'] + data[12:length-1]
				else:
					self.buf[vxid][self.buf[vxid]['cur']]['raw'] = self.buf[vxid][self.buf[vxid]['cur']]['raw'] + data[10:length-1]
					
		
'''
def main(smp):
	try:
		# 後でIPとポート関連も仕込んどく(PROXYpor関連
		vap = varnishapi.VarnishLog([
			'-g', 'request',
			'-q', 'Debug:VMD-DUMP',
			'-i', 'Debug'
			])
		if vap.error:
			print vap.error
			exit(1)
		smp.execute(vap)
	except KeyboardInterrupt:
		vap.Fini()
	except Exception as e:
		syslog.openlog(sys.argv[0], syslog.LOG_PID|syslog.LOG_PERROR, syslog.LOG_LOCAL0)
		syslog.syslog(syslog.LOG_ERR, traceback.format_exc())

if __name__ == '__main__':
	smp = SampleVarnishLog()
	main(smp)
'''  

