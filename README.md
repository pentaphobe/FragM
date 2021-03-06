
# FragM
This is derived from Mikael Hvidtfeldt Christensen's Fragmentarium representing a compilation of features and fixes contributed by many users at FractalForums since 2013.

Status
----------------------
[![Release][release-image]][releases]
[![License][license-image]][license]
[![Build Status](https://travis-ci.org/3Dickulus/FragM.svg?branch=master)](https://travis-ci.org/3Dickulus/FragM)

[release-image]: https://img.shields.io/badge/release-2.5.0-green.svg?style=flat
[releases]: https://github.com/3Dickulus/FragM/releases

[license-image]: https://img.shields.io/badge/license-GPL3-green.svg?style=flat
[license]: https://github.com/3Dickulus/FragM/blob/master/LICENSE

----------------------

# Changes since v0.9.5
Please see https://github.com/3Dickulus/FragM/blob/master/Fragmentarium-Source/3Dickulus.changes
and git logs for all of the changes since 2.0.

----------------------

# Building with Ubuntu Linux


## Add the main SDK repository...

      sudo add-apt-repository -y ppa:ubuntu-sdk-team/ppa
      sudo apt-get update -qq

## Ensure these packages are installed...

      sudo apt-get -qq install cmake
      sudo apt-get -qq install extra-cmake-modules
      sudo apt-get -qq install libopenexr-dev
      sudo apt-get -qq install libilmbase-dev
      sudo apt-get -qq install libqt5scripttools5
      sudo apt-get -qq install libqt5gui5
      sudo apt-get -qq install libqt5svg5-dev
      sudo apt-get -qq install libqt5opengl5-dev
      sudo apt-get -qq install libqt5xmlpatterns5-dev
      sudo apt-get -qq install qt5-qmake
      sudo apt-get -qq install qttools5-dev-tools
      sudo apt-get -qq install qtbase5-dev
      sudo apt-get -qq install qtbase5-dev-tools
      sudo apt-get -qq install qt5-style-plugins
      sudo apt-get -qq install qt5-image-formats-plugins
      sudo apt-get -qq install qtscript5-dev
#### We need the latest version of GLM
      wget https://launchpad.net/ubuntu/+archive/primary/+files/libglm-dev_0.9.9.3-2_all.deb
      sudo dpkg -i libglm-dev_0.9.9.3-2_all.deb

## Get the source...

      git clone https://github.com/3Dickulus/FragM.git FragM
      
or download and extract the .zip or .tar.gz source archive from https://github.com/3Dickulus/FragM/releases/

## Build and install

      cd FragM/Fragmentarium-Source
      mkdir build
      cd build
      cmake -DUSE_OPEN_EXR=ON -DCMAKE_INSTALL_PREFIX=~/ ..
      make install

The above will install in /home/username/Fragmentarium folder with includes and support files.

To install for all users change -DCMAKE_INSTALL_PREFIX=~/ to -DCMAKE_INSTALL_PREFIX=/usr/local and use sudo make install

# Building for MacOS

These steps need refinement, just first steps for now since I got it to actually build and run.  (Whether it works or not is an entirely different question)

- Install QT
  ```bash
  brew install qt
  ```
- Clone glm (currently being cloned to same _parent_ dir as FragM)
  ```bash
  pushd ..
  git clone https://github.com/g-truc/glm.git
  popd
  ```
- Generate XCode project file
  ```bash
  cd Fragmentarium-Source
  qmake -spec macx-xcode Fragmentarium-Source.pro
  qmake -project -after "QT += widgets opengl xml script scripttools" -after "INCLUDEPATH += '../../glm'"
  ```
- Open up Xcode and compile
  ```bash
  open Fragmentarium-Source.xcodeproj
  ```

# Pre-built packages
You can find pre-built packages for Ubuntu 16.0.4 Xenial (deb), SuSE Leap 15.0 (rpm), Windows (7z) at https://github.com/3Dickulus/FragM/releases/


# Guidelines for contributing
1. any new feature must work on the widest range of hardware, ( AMD Intel and nVidia ) all if possible
2. any changes to existing features must be compatible with the existing fragment base
3. existing features that only work on specific hardware should be phased out or modified to work on all hardware, AMD Intel and nVidia.
4. when adding options or settings they must be in line with the exiting structure for handling options and settings (unless you have a better idea that makes the current structure obsolete)
5. priority should go towards handling more modern GLSL versions and practices
6. changes and additions must be tested against all other features, animating, timeline management, easing curve controls etc...
   and render the base frags properly before merging will be considered

...of course none of this is carved in stone and I'm always open for discussing ideas.



## Notes
The deb and rpm packages should take care of any dependencies when installing via **apt / synaptic** or **yast / zypper**.

The Windows .7z file is an archive with everything FragM needs to run, includes all of the required Qt and runtime DLLs, this can be extracted and placed anywhere.

The best version will be the one you compile yourself because it will be optimized for your hardware setup.

And finally, please see the FAQ http://blog.hvidtfeldts.net/index.php/2011/12/fragmentarium-faq/ and https://fractalforums.org/fragmentarium/17 if you have any problems.
If you don't find an answer to your problem/question you can post on the Fragmentarium board at FractalForums.org or raise an issue here on github in the FragM repo.
