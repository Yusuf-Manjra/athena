#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# This script reads metadata from a given file

import sys
import json
import argparse
import time
import logging

msg = logging.getLogger('MetaReader')

from PyUtils.MetaReader import read_metadata


def __tree_print(content, indent = 2, pad = 0, list_max_items = -1, dict_sort=None, ascii=False):
	s = ''

	if isinstance(content, dict):

		if dict_sort == 'key':
			items = list(sorted(content.items(), key = lambda t: t[0]))
		elif dict_sort == 'value':
			items = list(sorted(content.items(), key =  lambda t: t[1]))
		else:
			items = list(content.items())

		items_count = len(items)
		for i in range(items_count):

			key, value = items[i]

			last = i == items_count - 1

			if pad == 0:
				skey = str(key)
			elif pad < 0:
				skey = str(key).rjust(-pad)
			else:
				skey = str(key).ljust(pad)

			if not ascii:
				s += ('├' if not last else '└') + '─' * indent + ' ' + skey + ': '
			else:
				s += ('|' if not last else '`') + '-' * indent + ' ' + skey + ': '

			lines = __tree_print(value, indent=indent, pad = pad, dict_sort = dict_sort, list_max_items = list_max_items, ascii = ascii).split('\n')

			if len(lines) == 1:
				s += lines[0] + '\n'
			else:
				for line in lines:
					if line.strip():
						if not ascii:
							s += '\n' + ('│' if not last else ' ') + ' ' * indent + ' ' + str(line)
						else:
							s += '\n' + ('|' if not last else ' ') + ' ' * indent + ' ' + str(line)

				s += '\n'
	elif isinstance(content, (list, tuple)) and list_max_items >= 0 and len(content) > list_max_items:

		items = list(content)
		items_count = len(items)

		for i in range(items_count):
			value = items[i]

			last = i == items_count - 1

			s += ('├' if not last else '└') + '─' * indent + ' ' + str(value) + '\n'
	else:
		s += str(content)

	return s


def __main():
	# Parsing the arguments provided by user
	parser = argparse.ArgumentParser(description='This script reads metadata from a given file')
	parser.add_argument('filenames',
						nargs = '+',
						help= 'The filenames to read. User can provide a single file or a list of files.')
	parser.add_argument('-v',
						'--verbose',
						action='store_true',
						help='print detailed output on screen')
	parser.add_argument('-o',
						'--output',
						metavar='FILE',
						default=None,
						help="Saves the output in a file. By default, the output is written on the screen (stdout) in a prettier format for better readabiilty.")
	parser.add_argument('--json',
						action='store_true',
						help="Sets the output file format as json.")
	parser.add_argument('--indent',
						metavar='N',
						type=int,
						default=2,
						help="Sets the indent spaces in the output either on screen (without -o flag) either on file (with -o flag). By default, uses two spaces as indent.")
	parser.add_argument('-m',
						'--mode',
						default= 'lite',
						metavar='MODE',
						type=str,
						choices=['tiny', 'lite', 'full'],
						help="This flag provides the user capability to select the amount of metadata retrieved. There three options: "
							 "tiny (only those values used in PyJobTransforms), "
							 "lite (same output as dump-athfile) "
							 "and full ( all  available data found) ")
	parser.add_argument('-t',
						'--type',
						default= None,
						metavar='TYPE',
						type=str,
						choices=['POOL', 'BS'],
						help="The file type of the input filename. By default, it tries to determine itself the file type of the input.")
	args = parser.parse_args()

	verbose = args.verbose
	filenames = args.filenames
	output = args.output
	is_json = args.json
	indent = args.indent
	mode = args.mode
	file_type = args.type

	msg.setLevel(logging.INFO if verbose else logging.WARNING)
	# create a stream handler
	handler = logging.StreamHandler()
	handler.setLevel(logging.INFO if verbose else logging.WARNING)
	# create a logging format
	formatter = logging.Formatter('%(name)s                       %(levelname)s %(message)s')
	handler.setFormatter(formatter)
	# add the handlers to the logger
	msg.addHandler(handler)

	startTime = time.time()
	msg.info('Imported headers in: {0} miliseconds'.format((time.time() - startTime) * 1e3))
	msg.info('The output file is: {0}'.format(output))

	metadata = read_metadata(filenames, file_type, mode= mode)

	if output is None:
		if is_json:
			print(json.dumps(metadata, indent= indent))
		else:
			print(__tree_print(metadata, indent= indent, pad= 18, dict_sort='key', list_max_items = 8))

	else:
		if is_json:
			with open(output, 'w') as fd:
				print >> fd, json.dumps(metadata, indent=indent)
		else:
			with open(output, 'w') as fd:
				print >> fd, __tree_print(metadata, indent = indent, pad = 18, dict_sort = 'key', list_max_items = 8, ascii = True)

	msg.info('Done!')

if __name__ == '__main__':
	__main()






