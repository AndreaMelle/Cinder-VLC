# Cinder-VLC

Cinder wrapper for VLC (Windows 32-bit only)

Note: compared to QuickTime, VLC cannot play media backwards (i.e. setting the rate to -1.0 will have no effect).
This is a known (https://forum.videolan.org/viewtopic.php?t=70191) technical limitation, therefore it's necessary to prepare backward versions of each clip we want to reverse.

## Instructions

* Currently requires manual copy of the content of vlc_bin folder in application output folder (Debug or Release)
* This block requires manual settings of cinder include and lib folders (I have an environment variable called CINDERSDK_DIR on my system), i.e. does not respect the conventional relative search paths that the other blocks do.

## TODO

Add post build script to copy content of vlc_bin folder

## Authors and contributors
* [Andrea Melle](https://github.com/andreamellehf) (Creator, developer)
