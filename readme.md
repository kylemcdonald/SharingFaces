# Sharing Faces

An installation for APAP Festival 2013 in Anyang, South Korea.

## Theory

Each visitor might spend 3 minutes with the piece, which could mean up to 5,000 images. Many of these images will be similar to each other or to previous images, so we only save the most representative ones (an order of magnitude fewer).

There is the potential for a very high dimensional search space when matching two faces. Important things to match include:

* Position (x, y) and scale
* Orientation (roll, pitch, yaw)
* Expression (object points)

This corresponds to around 700B of metadata per image.

The most important thing to match is the position of the face. Then the scale and orientation, and finally the expression. After 1000 visitors we are searching through more than 50,000 images.

While the images are sparse, different search algorithms yield a different exprience. Once the data is dense, different search algorithms only serve to optimize the time required to retrieve a decent image.

A simple algorithm consists of sarching all the available images for some percentage of the total that are within a given position distance threshold. If none are available, the closest one is chosen. If multiple are available, they are searched for a given scale and orientation threshold. Again, if none are available, the closest one is chosen. Finally, if multiple are available, the best expression match is chosen.

The easiest way to implement the above is by doing all the distance checks at each level and then sorting everything after the fact. Another technique for getting the N closest matches is to use a quadtree or binned system if we know the distribution is even.

## Todo

### Code

* Setup remote access (need to configure network ports)
* Make sure it doesn't crash, and restarts when it does, runs when the computer starts, restarts at 4am, screen is on during apap hours. switch for lights would be good to avoid cable movement.
* Measure everything
* Ensure that it's harder to have metadata without an image (consider saving metadata in jpeg exif)
* Optimize face tracking parameters, especially min haar size
* Fix camera latency
* Make ofxFaceTracker threading work correctly
* Use eye center instead of face center?
* Add config file and multiple city support
* Do benchmarks on different parts of the code to understand speed and scaling constraints
* Optimize search function?
* Optimize JPG saving and loading, see [libjpeg-turbo](http://libjpeg-turbo.virtualgl.org/) saving is pretty fast, but loading on GPU?
* Export statistics about how many frames are saved and how many are dropped for each visitor
* Fix ofxBlackMagicGrabber crash on exit
* Abstract the data representation out of ofxFaceTracker
* Check if FaceTracker can be run faster using [clang-omp](http://clang-omp.github.io/)

### Hardware

* Find replacement 12V power supply
* Find replacement BNC cable
