
# GStreamer drawshapes

## Overview

The GStreamer drawshapes  Plugin is a versatile tool that enables users to draw shapes on videostreams seamlessly within the GStreamer framework. This plugin is designed for flexibility and ease of use, making it an excellent choice for drawing shapes like square,rectangle,triangle and circle in GStreamer pipelines.it only works for NV12 format.

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

Ensure that you have GStreamer installed on your system before proceeding with the installation of the drawshapes Plugin. Follow these steps:

```bash
clone the file and go to that directory
cmake -B build/
cd build
make
sudo cp libdrawshapes.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0/


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

Once the GStreamer drawshapes Plugin is installed, you can use it in your GStreamer pipelines. The element name for this plugin is drawshapes. Here's an example of how to use the logimpose Plugin in a GStreamer pipeline:

```console
$ gst-launch-1.0 videotestsrc ! video/x-raw,format=NV12,width=1920,height=1080 ! drawshapes shape=<rectangle or square or circle or tringel> xco=100 yco=100 color=red or green or blue! autovideosink
```
In this example, the shape given is drawn on the frame and we can modify size other properties of shape,we can verify about properties by inspecting the plugin using gstreamer tools



## Plugin Parameters
The drawshapes supports following parameters:

shape:rectangle,circle,triangle and square select among these
color : select color among red,green,blue
dimensions of shape like
height
width
radius in accordance with shape
xco: The x coordinate of the drawshapes location.
yco: The y coordinate of the drawshapes location.


Example Pipelines
```console
 gst-launch-1.0 videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12,framerate=60/1 ! drawshapes  shape=rectangle xco=50 yco=100  ! autovideosink
```
```console
 gst-launch-1.0 videotestsrc ! video/x-raw,width=720,height=480,format=NV12,framerate=24/1 ! drawshapes shape=square color=green xco=300 yco=900  ! autovideosink
```
```console
 gst-launch-1.0 videotestsrc ! video/x-raw,width=1920,height=1080,format=NV12,framerate=30/1 ! drawshapes shape=circle color=red radius=50  xco=50 yco=100  ! autovideosink
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
