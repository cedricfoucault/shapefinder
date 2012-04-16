shapefinder
=============

Locates a given shape in a target image. Renders the shape where it has been detected in a new image.

This is a work in progress.


What it does
------------

The program takes 2 images as inputs. One is the pattern image, it contains the shape that has to
be located - only the outline of the shape matter for the detection. The other is the target image.

The program then creates a new image and renders the shape where it has been located in the target image.
If the shape appears several time in the target image, the shape will appear several times at the correct locations.

It also optionally creates two other images. One represents the raw filtering of the two images -
the filter includes edge detection, smoothing and do the cross-correlation of the 2 images. The other selects
and renders only the peaks of the filtering. These outputs do not matter if you whish to locate the shapes, but
help to analyze the results if you want to go further. Note that in both images, there is an offset:
if there is a peak at [145][86], it means that you need to move the target image 145 pixels to the right and 86 pixels
to the bottom in order to find the correct location of the shape.

This process has been applied to videos to track a moving object in a video:
it locates the object at each time frame and renders its trajectory in an output image.
The result is highly experimental, but you can try it if you want by switching the call in main() to traceTrajectory()
(instead of findShape()).


Compilation & Execution
-----------------------

* To compile the project, just run the Makefile ($ make install). If it doesn't work, try to build it in your favorite IDE
under a new project. Don't forget to link it with the libtiff library in repo/folder/lib/.

* If you want to use the video tracking part (traceTrajectory()), you need to have OpenCV installed
(see opencv.willowgarage.com).


How to use it
-------------

The current version of the program use a 2 text files called "paths.txt"
and "tolerancethreshold.txt" (optional) to parse its parameters.
You can use the program for the images of your choice by modifying these files.

Each line of "paths.txt" contains a file path:
* line 1: path to the target image
* line 2: path to the pattern image
* line 3: path to the resulting image (the image will be created if it does not exist yet)
* line 4: path to the raw filtering image (optional) (the image will be created if it does not exist yet)
* line 5: path to the peaks image (optional) (the image will be created if it does not exist yet).

For the moment, the program will work only if the target and pattern image meet the following criteria:
* The images must be stored in the TIFF File Format, with no compression, in grayscale, with 8-bit color depth.
* The width and height of the images must be a power of 2.
* The target and pattern images must have the exact same size.

An optional parameter can be specified in "tolerancethreshold.txt".
It contains a float between 0 and 1 that represents the tolerance threshold of the detection.
At one point, the program must do a yes/no detection of the shape at a given location.
The higher the tolerance threshold, the more selective the detection is.
A lower tolerance threshold reduce the risk of missing the shape at one location,
but increase the chances for false-positives, and vice-versa.

By default the tolerance threshold is 0.7 (or 70%) - even if the file "tolerancethreshold.txt" does not exist.
So far, I've found empirically that this value gives the best results in the general case.
Note that if the shape is supposed to appear only 1 time in the target image, a tolerance threshold of 1 (100%)
should work perfectly.


Todo
----

* Extend the detection so that it works even if the shapes in the target image are bigger or smaller
than in the pattern image.
* Extend the detection so that it works even if the shapes in the target image is rotated.


Contact
-------

cedric.foucault@gmail.com
or http://github.com/rez-antlion
