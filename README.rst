w64
===

For debugging filesystem oddities.  see `w64.c` comment block for high level overview


compile
=======

.. code:: sh

	mkdir build
	pushd build
		meson .. -Dprefix=/your/prefix/here
		meson test
		meson install
	popd


run
===

.. code:: sh

   w64 --help



Run one iteration, 1Gib, written to `/tmp`, don't expliclly call `sync()`

.. code:: sh

	./w64 --iter=1 --size=1 --path=/tmp --flush=0
