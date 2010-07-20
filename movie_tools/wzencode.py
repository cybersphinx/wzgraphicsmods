#!/usr/bin/env /usr/bin/python

import sys
import subprocess
from optparse import OptionParser


#time='' # transcode for TIME seconds


usage = "usage: %prog [options] dest-file.ext -- more-ffmpeg-options"
desc = """\
convert wz video dumps (ppm format) into video file.
output format depends on output file name extension, ffmpeg style.
"""
parser = OptionParser(usage = usage, description = desc)

## parser.add_option("-i", dest="infile",
## 				  help="input filename")
## parser.add_option("-o", dest="outfile",
## 				  help="out filename")

parser.add_option("-s", "--scene", dest="scene", type="int", default='1',
				  help="scene number.  default:%default")

parser.add_option("-t", "--time", dest="time",
				  help="encode for TIME seconds")

parser.add_option("-b", "--bitrate", dest="bitrate",
				  default="200",
				  help="bitrate for video file.  default:%default kbps. k and m suffixes ok.")
parser.add_option("-r", "--ofr", dest="frame_rate",
				  default="25", type="int",
				  help="frame rate for video file. default:%default")

parser.add_option("--ifr", dest="input_frame_rate", type="int",
				  default='10',
				  help="input frame rate for ppm files. default:%default")

parser.add_option( "--basename", dest="basename", default="wz2100_",
				  help="basename for ppm files, default:%default")


#
# parse options
#

(options, args) = parser.parse_args()

print 'options', options
print 'args',args

# collect args after --
#print( sys.argv[ sys.argv.index('--'):])
tail = []
if '--' in sys.argv:
	tail = sys.argv[ sys.argv.index('--')+1 :]

##print 'tail:',tail



# options.bitrate is a string to match ffmpeg options
# but info.video[0].bitrate is an int

## if options.bitrate:
## 	bitrate = options.bitrate

## print 'bitrate is: ', bitrate

# ffmpeg command  options format string
# source file, bitrate,  more options, dest file
#
#CMD='ffmpeg -r %s -i wz2100_%s_%03d.ppm -b %s %s %s '

basename = "wz2100_%03d" % (options.scene) + '_%03d.ppm'

CMD = 'ffmpeg -r %d -i %s -r %d -b %s %s' %\
(options.input_frame_rate, basename, options.frame_rate, options.bitrate, args[0])

# ffmpeg (-r IFR)  -i wz2100_SCENE_%03d.ppm -b BITRATE -r FRAMERATE OUTFILE
#print( CMD % (options.infile, bitrate, options.outfile) )
#print( CMD % (args[0], bitrate, ' '.join(tail), args[1] ) )

myopts = ' '.join(tail)

## if options.time:
## 	myopts = ' '.join( ['-t', options.time, myopts])
## print 'myopts:', myopts
command =  CMD 
print 'command:', command


try:
	subprocess.check_call( command, shell=True )
except subprocess.CalledProcessError, e:
	logfile= open('./delete-me','a')
	logfile.write( "%s\n" % args[0])
	logfile.close()

				  
	
