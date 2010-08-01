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
				  default='20',
				  help="input frame rate for ppm files. default:%default")

parser.add_option( "--basename", dest="basename", default="wz2100_",
				  help="basename for ppm files, default:%default")

parser.add_option( "--size", dest="size", type="string",
				   help="output size widthxheight    default: same as .ppm files.  ffmpeg abbreviations like 'svga' are supported")

parser.add_option( "--test", dest = "test", action="store_true", default="False",
				   help="test arguments.  prints ffmpeg command but does not run it")

# parse options

(options, args) = parser.parse_args()

print 'options', options
print 'args',args

if len(args) is 0:
	print 'missing filename argument'
	raise Exception('you forgot the name for the new video file')


# collect args after --
#print( sys.argv[ sys.argv.index('--'):])
tail = []
if '--' in sys.argv:
	tail = sys.argv[ sys.argv.index('--')+1 :]

extra_opts = ''.join(tail)

# ffmpeg command  options format string
# source file, bitrate,  more options, dest file
#
#CMD='ffmpeg -r %s -i wz2100_%s_%03d.ppm -b %s %s %s '

video_data = "wz2100_%03d" % (options.scene) + '.ppm'
audio_data = "wz2100_%03d" % (options.scene) + '.raw'

# frame size is None if option is not specified

if options.size:
	frame_size = "-s %s" % options.size
else:
	frame_size = ""

'''
ffmpeg
-r 20 -vcodec ppm -f image2pipe -i wz2100_001.ppm
-f s16le -ar 48000 -ac 2 -i wz2100_001.raw
-r 25 -b 1200k   -y out.avi

'''
audio_opts = " -f s16le -ar 48000 -ac 2 -i %s" % (audio_data)
## print audio_opts

video_opts = " -r %d -vcodec ppm -f image2pipe -i %s -r %d -b %s %s %s" %\
(options.input_frame_rate, video_data, options.frame_rate, options.bitrate, frame_size, extra_opts)


# ffmpeg  InFR   Basename Framerate Bitrate FrameSize ExtraOptions OutputFile
# ffmpeg  -r %d  -i %s    -r %d     -b %s   %s         %s          %s'

'''
command = 'ffmpeg -r %d -vcodec ppm -f image2pipe -i %s -r %d -b %s %s %s %s' %\
(options.input_frame_rate, video_data, options.frame_rate, options.bitrate, frame_size, extra_opts, args[0])
'''

command = "ffmpeg %s %s %s" %(video_opts, audio_opts, args[0])


if options.test:
	print 'command is:\n\t', command
else:
	try:
		print 'running command:', command
		subprocess.check_call( command, shell=True )
	except subprocess.CalledProcessError, e:
		print "command failed:", command


				  
	
