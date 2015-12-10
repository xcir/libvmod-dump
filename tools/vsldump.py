#!/usr/bin/python

# coding: utf-8
import varnishapi,time,os,sys,syslog,traceback


class SampleVarnishLog:
	def execute(self,vap):
		#connect varnishapi
		self.vap     = vap
		while 1:
			ret = self.vap.Dispatch(self.vapCallBack)
			if 0 == ret:
				time.sleep(0.5)
		
		
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
		print data[6:length-1],

def main(smp):
	try:
		vap = varnishapi.VarnishLog(['-g','raw','-q','Debug:DUMP'])
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

	    
