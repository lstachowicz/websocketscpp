#!/usr/bin/env python
import sys
import os
import shutil
import argparse

root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
build_dir = os.path.join(root_dir, "BUILD_DIR")
coverage_build_dir = os.path.join(root_dir, "BUILD_DIR_COVERAGE")

def configure(type):
	if os.path.exists(build_dir):
		shutil.rmtree(build_dir)

	os.mkdir(build_dir)
	os.chdir(build_dir)
	os.system('cmake ../ -DCMAKE_BUILD_TYPE=%s' % type)

def build(type):
	os.chdir(build_dir)
	os.system('cmake --build . --config %s' % type)

def tests():
	pass

def coverage(type):
	if type == 'Debug':
		os.chdir(build_dir)
	else:
		if os.path.exists(coverage_build_dir):
			shutil.rmtree(coverage_build_dir)
		os.mkdir(coverage_build_dir)
		os.chdir(coverage_build_dir)
		os.system('cmake ../ -DCMAKE_BUILD_TYPE=Debug')
	os.system('cmake --build . --config Debug --target websocketscpp_coverage')
	

def sdk():
	# TODO: Build sdk out of artifacts
	pass

def main(argv):
	parser = argparse.ArgumentParser('Build websocketscpp project')
	parser.add_argument('--coverage', dest='coverage', action='store_true', help='Generate coverage')
	parser.add_argument('--tests', dest='tests', action='store_true', help='Run tests')
	parser.add_argument('--sdk', dest='sdk', action='store_true', help='Generate sdk')
	parser.add_argument('--type', dest='type', choices=['Debug', 'RelWithDebInfo'], default='RelWithDebInfo', help='Build type')
	args = parser.parse_args(argv[:1])

	configure(args.type)
	build(args.type)

	if args.tests:
		tests()

	if args.coverage:
		coverage(args.type)

	if args.sdk:
		sdk()

if __name__ == "__main__":
	main(sys.argv[1:])