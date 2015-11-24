#! /usr/bin/env python
# encoding: utf-8
# Konstantinos Chatzilygeroudis - 2015

"""
Quick n dirty nlopt detection
"""

import os, glob, types
from waflib.Configure import conf


def options(opt):
	opt.add_option('--nlopt', type='string', help='path to nlopt', dest='nlopt')


@conf
def check_nlopt(conf):
	if conf.options.nlopt:
		includes_check = [conf.options.nlopt + '/include']
		libs_check = [conf.options.nlopt + '/lib']
	else:
		includes_check = ['/usr/local/include', '/usr/include']
		libs_check = ['/usr/local/lib', '/usr/lib']
		if 'RESIBOTS_DIR' in os.environ:
			includes_check = [os.environ['RESIBOTS_DIR'] + '/include'] + includes_check
			libs_check = [os.environ['RESIBOTS_DIR'] + '/lib'] + libs_check

	try:
		conf.start_msg('Checking for NLOpt includes')
		res = conf.find_file('nlopt.hpp', includes_check)
		conf.end_msg('ok')
		conf.start_msg('Checking for NLOpt libs')
		res = res and conf.find_file('libnlopt_cxx.so', libs_check)
		conf.end_msg('ok')
		conf.env.INCLUDES_NLOPT = includes_check
		conf.env.LIBPATH_NLOPT = libs_check
		conf.env.DEFINES_NLOPT = ['USE_NLOPT']
		conf.env.LIB_NLOPT = ['nlopt_cxx']
	except:
		conf.end_msg('Not found', 'RED')
		return
	return 1