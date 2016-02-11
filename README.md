# Cinder-VLC

Cinder wrapper for VLC libraries, for Cinder vs2013 x86 and x64.

Note: compared to QuickTime, VLC cannot play media backwards (i.e. setting the rate to -1.0 will have no effect).
This is a known (https://forum.videolan.org/viewtopic.php?t=70191) technical limitation.

## Instructions

* Currently requires manual copy of the content of vlc_bin folder in application output folder (Debug or Release). Contains .dlls and runtime stuff needed by VLC.

## TODO

* Add post build script to copy content of vlc_bin folder
* Add audio
* Test with several video codecs
* Add extended VLC functionalities, such as playlist management, etc...

## Authors and contributors
* [Andrea Melle](https://github.com/AndreaMelle) (Creator, developer)
