w64
===

see `w64.c` comment block for high level overview


.. code:: sh

	mkdir build
	pushd build
		meson .. -Dprefix=/your/prefix/here
		meson test
		meson install
	popd
