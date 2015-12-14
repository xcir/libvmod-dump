#!/usr/bin/python

import varnishdumpcore,getopt,os,sys,syslog,traceback
#based on Jurgen Hermanns http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/66012

def main(opts):
	try:
		vhs = varnishdumpcore.varnishDump(opts)
		vhs.execute()
	except KeyboardInterrupt:
		pass
	except Exception as e:
		syslog.openlog(sys.argv[0], syslog.LOG_PID|syslog.LOG_PERROR, syslog.LOG_LOCAL0)
		syslog.syslog(syslog.LOG_ERR, traceback.format_exc())

if __name__ == '__main__':
	try:
		opts,args = getopt.getopt(sys.argv[1:],"po:Dn:P:V",["sopath="])
	except getopt.GetoptError:
		print 'invalid option'
		print 'usage: varnishhoststat -p -o [logdir] --sopath [libvarnishapi.so] -D -n [instance-name] -P [pid-file] -V'
		sys.exit(2)
	
	d_flag = False
	p_file = False
	for o,a in opts:
		if   o == '-D':
			d_flag = True
		elif o == '-P':
			p_file = a
		elif o == '-V':
			print 'varnishdump (v0.1)'
			sys.exit(0)
		elif o == '-n':
			print 'using instance %s' % a
	if d_flag:
		try:
			pid = os.fork()
			if pid > 0:
				sys.exit(0)
		except OSError, e:
			print >>sys.stderr, "fork #1 failed: %d (%s)" % (e.errno, e.strerror)
			sys.exit(1)
		os.chdir("/")
		os.setsid()
		os.umask(0)
		try:
			pid = os.fork()
			if pid > 0:
				if p_file:
					open(p_file,'w').write("%d"%pid)
				sys.exit(0)
		except OSError, e:
			print >>sys.stderr, "fork #2 failed: %d (%s)" % (e.errno, e.strerror)
			sys.exit(1)
	main(opts)


