
# GStreamer logoimpose

## Overview

The GStreamer logoimpose  Plugin is a versatile tool that enables users to impose a logo on videostreams seamlessly within the GStreamer framework. This plugin is designed for flexibility and ease of use, making it an excellent choice for overlaying the png files in GStreamer pipelines.it only works for NV12 format.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Plugin Parameters](#plugin-parameters)
- [Example Pipelines](#example-pipelines)
- [License](#license)
- [Contributing](#contributing)
- [Changelog](#changelog)
- [Acknowledgements](#acknowledgements)

## Installation

Ensure that you have GStreamer installed on your system before proceeding with the installation of the logoimpose Plugin. Follow these steps:

```bash
clone the file and go to that directory
cmake -B build/
cd build
make
sudo cp liblogoimpose.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0/


## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Plugin Parameters](#plugin-parameters)
- [Example Pipelines](#example-pipelines)
- [License](#license)
- [Contributing](#contributing)
- [Changelog](#changelog)
- [Acknowledgements](#acknowledgements)


## Usage

Once the GStreamer logoimpose Plugin is installed, you can use it in your GStreamer pipelines. The element name for this plugin is logoimpose. Here's an example of how to use the logimpose Plugin in a GStreamer pipeline:

```console
$ gst-launch-1.0 videotestsrc ! video/x-raw,format=NV12,width=1920,height=1080 ! logoimpose path=<the path of image you required> animation=normal speed=slow direction=right xco=100 yco=100 ! autovideosink
```
In this example, the image given is overlayed and displays on the default raw video source of NV12 type as per given properties.you can give filesrc and v4l2src in place of videotestsrc .we can modify the animation,speed,opacity percentage speed in above operation as per requirement.


## Plugin Parameters
The logoimpose supports following parameters:

path: The path of image on your device which is to be overlayed on videoframes
animation: the type of animation like scroll,rotate and normal(steady)
           scroll : all 4 directions
           rotate : left and right
           normal : no need of direction
speed: describes the speed of animation like fast,medium,slow.
direction: on which direction the animation should be done >>right,left,up,down
alpha : opacity percentage it decribes the transperency of image,the value should be in between 0-100. 
xco: The x coordinate of the logoimpose location.
yco: The y coordinate of the logoimpose location.


Example Pipelines
```console
 gst-launch-1.0 videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12,framerate=60/1 ! logoimpose path=<.......> direction=right alpha=50 animation=rotate  xco=50 yco=100  ! autovideosink
```
```console
 gst-launch-1.0 videotestsrc ! video/x-raw,width=720,height=480,format=NV12,framerate=24/1 ! logoimpose path=<.....> direction=right alpha=100 animation=scroll  xco=300 yco=900  ! autovideosink
```
```console
 gst-launch-1.0 videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12,framerate=30/1 ! logoimpose direction=right alpha=20 animation=normal  xco=50 yco=100  ! autovideosink
```


## License
This project is licensed under the MIT License - see the LICENSE file for details.
GStreamer itself is licensed under the Lesser General Public License version 2.1 or (at your option) any later version: https://www.gnu.org/licenses/lgpl-2.1.html

## Contributing
Please read CONTRIBUTING.md for details on our code of conduct, and the process for submitting pull requests to us.

## Changelog
See the CHANGELOG.md file for details about the changes between versions.

## Acknowledgements
Hat tip to anyone whose code was used
Inspiration
etc
If you use this plugin in your project, consider adding your name/company to the acknowledgments.
