#!/usr/bin/env /usr/bin/python

import sys
import subprocess
from optparse import OptionParser


usage = "usage: %prog [options] dest-file.ext -- more-ffmpeg-options"
desc = """\
convert wz video dumps (ppm format) into video file.
output format depends on output file name extension, ffmpeg style.
ffmpeg specific options can be passed after -- 
"""
parser = OptionParser(usage = usage, description = desc)

parser.add_option("-s", "--scene", dest="scene", type="int", default='1',
				  help="scene number.  default:%default")

parser.add_option("-t", "--time", dest="time",
				  help="encode for TIME seconds")

parser.add_option("-b", "--bitrate", dest="bitrate",
				  default="1200k",
				  help="bitrate for video file.  default:%default kbps. k and m suffixes ok.")
parser.add_option("-r", "--ofr", dest="frame_rate",
				  default="25", type="int",
				  help="frame rate for video file. default:%default")

parser.add_option("--ifr", dest="input_frame_rate", type="int",
				  default='10',
				  help="input frame rate for ppm files. default:%default")

parser.add_option( "--basename", dest="basename", default="wz2100_",
				  help="basename for ppm files, default:%default")

parser.add_option( "--size", dest="size", type="string",
				   help="output size widthxheight    default: same as .ppm files.  ffmpeg abbreviations like 'svga' are supported")

# parse options

(options, args) = parser.parse_args()

## print 'options', options
## print 'args',args

# collect args after --
#print( sys.argv[ sys.argv.index('--'):])
tail = []
if '--' in sys.argv:
	tail = sys.argv[ sys.argv.index('--')+1 :]

extra_opts = ' '.join(tail)

# ffmpeg command  options format string
# source file, bitrate,  more options, dest file
#
#CMD='ffmpeg -r %s -i wz2100_%s_%03d.ppm -b %s %s %s '

basename = "wz2100_%03d" % (options.scene) + '_%03d.ppm'

# frame size is None if option is not specified

frame_size = options.size if options.size else ""
if options.size:
	frame_size = "-s %s" % options.size
else:
	frame_size = ""

# ffmpeg  IFR Basename Framerate Bitrate FrameSize ExtraOptions OutputFile

cmd = 'ffmpeg -r %d -i %s -r %d -b %s  %s %s %s' %\
(options.input_frame_rate, basename, options.frame_rate, options.bitrate, frame_size, extra_opts, args[0])

command = cmd 
print 'running command:', command


try:
	subprocess.check_call( command, shell=True )
except subprocess.CalledProcessError, e:
	print "command failed:", command

				  
	
