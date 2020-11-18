project('w64', 'c', version : '1.0.0')
cc = meson.get_compiler('c')
prefix = get_option('prefix')
bin_dest='@0@/bin'.format(prefix)

add_test_setup('valgrind',
    exe_wrapper : [
        'valgrind',
        '--error-exitcode=1',
        '--leak-check=full',
        '--track-origins=yes'],
    timeout_multiplier : 100)

s_dir = './src'
i_dir = './include'
u_dir = './util'
inc = include_directories( i_dir )

log = '@0@/log.c'.format(s_dir)

lib_sources = [
	'@0@/w64.c'.format(s_dir),
	log]

test_sources = [
	'@0@/test.c'.format(s_dir),
	log]

sources = [
	'@0@/main.c'.format(s_dir)]

check_sources = [
	'@0@/check_file.c'.format(u_dir)]


libw64d = shared_library('libw64',
	lib_sources,
	include_directories : inc,
	link_args: [
		'-fuse-ld=lld',
		'-flto'],
	install: false)

libw64s = static_library('libw64',
	lib_sources,
	link_args: [
		'-fuse-ld=lld',
		'-flto'],
	include_directories : inc,
	install: false)

w64s = executable('w64',
	sources,
	include_directories : inc,
	install: true,
	install_dir: bin_dest,
	link_args: [
		'-static',
		'-fuse-ld=lld',
		'-flto'],
	link_with: libw64s)

check_file = executable('check_file',
	check_sources,
	include_directories: inc,
	install: true,
	install_dir: bin_dest,
	link_args: [
		'-static',
		'-fuse-ld=lld',
		'-flto'],
	link_with: libw64s)


w64d = executable('w64_d',
	sources,
	include_directories : inc,
	link_args: [
		'-flto',
		'-fuse-ld=lld'],
	install: false,
	link_with: libw64d)

t = executable('t_unit',
	test_sources,
	include_directories: inc,
	link_args: [
		'-flto',
		'-fuse-ld=lld'],
	install: false,
	link_with: libw64d)

test('t_unit', t)
test('full', w64d, args : ['--iter=1', '--path=/tmp/io.file', '--size=1', '--flush=0'])
