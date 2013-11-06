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

### Necessary

* Measure distances to everything
* Add config file and multiple city support, handle syncing with rsync.
* Export statistics about how many frames are saved and how many are dropped for each visitor.

### Extra

* Make sure it restarts when it crashes. switch for lights would be good to avoid cable movement.
* Consider saving metadata in jpeg exif.
* Drop duplicates from the first day.
* Abstract the data representation out of ofxFaceTracker

### Optimizations

* If the N nearest neighbors within a bin are farther than distance to the edge of the bin, need to look into the next bin.
* Use DeckLink SDK for YUV conversion.
* Only do YUV conversion when saving to disk, or maybe there is a library that will generate jpegs from raw YUV.
* Resize grayscale image from camera before transposing and flipping to save operations.
* Do benchmarks on different parts of the code to understand speed and scaling constraints
* Optimize search function?
* Optimize JPG saving and loading, see [libjpeg-turbo](http://libjpeg-turbo.virtualgl.org/) saving is pretty fast, but loading on GPU?
* Check if FaceTracker can be run faster using [clang-omp](http://clang-omp.github.io/)

## Technical Specifications

### Computer

A single Mac computer with at least a 256 GB SSD, 2.3 GHz Quad-core CPU, 4GB of RAM, any modern graphics card. A current model Mac Mini with an upgrade on the hard drive will fit these requirements.

### Camera

The piece is designed to work with the [BlackMagic Ultra Studio Mini Recorder](http://www.bhphotovideo.com/c/product/892453-REG/Blackmagic_Design_bdlkulsdzminrec_Ultrastudio_Mini_Recorder.html), but may also work with other BlackMagic grabber devices. We use an [EverFocus Megapixel surveillance camera](http://www.bhphotovideo.com/c/product/887209-REG/EverFocus_eqh5102_MINI_BOX_CAMERA_12VDC_1080P.html) with a [Tamron 2.8-8mm lens](http://www.bhphotovideo.com/c/product/855399-REG/Tamron_M13VM288IR_1_3_2_8_to.html). The lens is positioned at maximum zoom (8mm) with the aperture wide open, and focused at 2.1m.

### Network

Each installation will capture approximately 100-150 MB of images each day. These images are synced at the end of each day using rsync over ssh (port 22). rsync only sends the new images and metadata.

### Screen

The installation at APAP uses an LG 42LN5700, which is a 42" 1080p screen with a wide viewing angle. Because the screen is mounted in portrait mode it's important that the screen has a large vertical viewing angle.

### Design

The measurements and layout of all the components required for the installation are available in the sketchup file "installation.skp".

### Television Settings

For the APAP television, the following settings are used.

#### PICTURE

* Energy saving: Off
* Picture Mode: Game (User)
* Backlight: 100
* Contast: 90
* Brightness: 50
* Sharpness: 0
* Color: 50
* Tint: 0
* Color Temp.: 0
* Advanced Control
	* Dynamic Contrast: Off
	* Dynamic Color: Off
	* Super Resolution: Off
	* Gamma: Low
* Picture Option
	* Noise Reduction: Off
	* MPEG Noise Reduction: Off
	* Black Level: Low
	* Motion Eye Care: Off
* Aspect Ratio: Just Scan

### Camera Settings

For the APAP camera, the following settings are used.

#### EXPOSURE

* BRIGHTNESS: 10
* MODE: INDOOR
* LENS: MANUAL
* SHUTTER: AUTO
* DSS: OFF
* AGC: 10

#### MWB

* COLOR TEMP: MIDDLE
* R-GAIN: 10
* B-GAIN: 10

#### IMAGE

* SHARPNESS: 0
* MIRROR: OFF
* FLIP: OFF
* DZOOM: 0
* PRIVACY: OFF
* SHADING: ON
* HLMASK: OFF
* CTB: ON
* GAMMA: 0.45

#### DAY&NIGHT

* MODE: COLOR
* AGC THRS: 10
* DELAY: 5 SEC

#### BLC

* MODE: OFF
* BLC OSD: OFF
* BLC POS-X: 7
* BLX POS-Y: 7
* BLC SIZ-X: 5
* BLC SIZ-Y: 5

#### SYSTEM

* CAM TITLE: OFF
* DOUT SCALE: FULL
* DOUT FORMAT: 1080P
* DOUT FPS: 30
* FREQ: 60HZ
* CVBS: NTSC

#### COLOR

* AWB: MANUAL
* CHROMA: 10
* R-OFFSET: 12
* B-OFFSET: 5