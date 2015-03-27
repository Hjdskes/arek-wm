Arek window manager
=========

**A modern tiling window manager, based on libmutter**

For a quick introduction to Arek Window Manager (how it is set up, which
layouts it supports, what default keybindings it uses, et cetera) see [structure.txt][struct].

Installation
------------

Arek Window Manager requires the following dependencies in order to compile and
run:

* Mutter >= 3.14.0
* GLib

To generate the Makefiles, gnome-common is required.

Once these dependencies are installed, run the following commands to build and
install Arek Window Manager:

	$ ./autogen
	$ make
	# make clean install

TODO
----

* Dynamic workspaces support;
* Real multimonitor support;
* Client factor, requires setting data with `g_object_set_data ()`;
* Look into rearranging windows by draggin them around, instead of making them
  float.

Bugs
----

For any bug or request [fill an issue][bug] on [GitHub][ghp].

Contributing
------------

Contributions to Arek Window Manager are very welcome! All sorts of patches and
pull requests are accepted, be it documentation, bug fixes, optimisations or new
features.

The quickly-scrambled-together TODO list up above should give a few pointers, as
do the lines in *asterisks* in [structure.txt][struct] and the TODOs in the
code. If you want to implement a new feature that is currently not listed
somewhere, please check with me first to prevent yourself from working for
nothing.

Please do try to adhere to the coding style currently in use. Patches and pull
requests that are out of style will not be accepted.

License
-------

Please see [LICENSE][lic] on [GitHub][ghp].

**Copyright © 2015** Jente Hidskes <hjdskes@gmail.com>

  [struct]: https://github.com/Unia/arek-wm/blob/master/structure.txt
  [bug]: https://github.com/Unia/arek-wm/issues
  [lic]: https://github.com/Unia/arek-wm/blob/master/LICENSE
  [ghp]: https://github.com/Unia/arek-wm

