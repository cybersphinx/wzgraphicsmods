#!/usr/bin/env python
'''
script to format Changelog with BBcode tags for posting on webpages.
Formatted text is sent to standard out
usage:  log2bbc.py Changelog
'''

import sys
import re
from optparse import OptionParser

# option stuff

usage = "usage: %prog Changelog >output_file"
desc="""\
script to format Changelog with BBcode tags for posting on webpages.
Formatted text is sent to standard out
"""

parser = OptionParser( usage= usage, description = desc)

(options, args) = parser.parse_args()e

# regex stuff

# revisions - rNNNN
revPat = re.compile(r'r([0-9]+)')
revSub = lambda x: '[rev]'+ x.group(1) + '[/rev]'

# tickets - ticket:NNNN
ticketPat = re.compile( r'ticket:([0-9]+)')
ticketSub = lambda x: '[ticket]' + x.group(1) + '[/ticket]'

# heading - * Something:
headPat = re.compile(r'^ \* (.*):$')
headSub = lambda x: '[b]' + x.group(1) + '[/b]'

# version -  NNNN-NN-NN: some-version
versionPat = re.compile( r'^\d\d\d\d-\d\d-\d\d:.*$')
versionSub = lambda x: '[b][u]' + x.group() + '[/u][/b]\n'
						

if __name__ == '__main__':
	with open( sys.argv[1], 'r') as f:
		for line in f.readlines():
			line = revPat.sub( revSub , line)
			line = ticketPat.sub( ticketSub, line)
			line = headPat.sub( headSub, line)
			line = versionPat.sub( versionSub, line )
			print line,
			


	
