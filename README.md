libVV
=====

libVV is a Volume Visualization library.

Basicly this is a rewritten version of gtf (GTK Transfer Function).


Features
--------

* Volume rendering
* Iso-surface extraction


Build Dependencies
-----------------

* libegl-dev
* libgbm-dev
* libgles-dev
* libwayland-dev
* libwayland-egl-backend-dev
* wayland-protocols


Volume Data
-----------

1. [The Stanford volume data archive][1]

Run data/download.sh to generate raw volume data.

2. [Open Scientific Visualization Datasets][2]

3. [InVols VIEWER][3]

[1]: https://graphics.stanford.edu/data/voldata/voldata.html
[2]: https://klacansky.com/open-scivis-datasets/
[3]: https://ngavrilov.ru/invols/index.php?id=Download

TODO List
---------

* Better GUI/TUI
* API reference documentation with doxygen
* Write .clang-format
* Faster iso-surface extraction.
* Volume data manipulation
