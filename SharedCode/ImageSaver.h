#pragma once

#include "ofMain.h"

class QueuedImage {
public:
    ofPixels image;
    string filename;
	QueuedImage(ofPixels& image, string filename)
	:image(image)
	,filename(filename) {
	}
};

class ThreadedImageSaver : public ofThread {
private:
    queue< ofPtr<QueuedImage> > data;
public:
    void threadedFunction() {
        while(!data.empty()) {
			if(!isThreadRunning()) {
				ofLogWarning("ThreadedImageSaver") << data.size() << " images left to save";
			}
			ofPtr<QueuedImage> cur = data.front();
			ofSaveImage(cur->image, cur->filename, OF_IMAGE_QUALITY_HIGH);
			lock();
			data.pop();
			unlock();
        }
    }
    void saveImage(ofPixels& img, string filename) {
		shared_ptr<QueuedImage> cur(new QueuedImage(img, filename));
		lock();
		data.push(cur);
		unlock();
		if(!isThreadRunning()){
			startThread();
		}
    }
	int getQueueSize() {
		lock();
		int size = data.size();
		unlock();
		return size;
	}
	void exit() {
		waitForThread();
	}
};

class MultiThreadedImageSaver {
protected:
	int currentThread;
	vector< ofPtr<ThreadedImageSaver> > threads;
public:
	MultiThreadedImageSaver(int threadCount = 4)
	:currentThread(0) {
		for(int i = 0; i < threadCount; i++) {
			threads.push_back(shared_ptr<ThreadedImageSaver>(new ThreadedImageSaver()));
		}
	}
	void saveImage(ofPixels& img, string filename) {
		threads[currentThread]->saveImage(img, filename);
		currentThread = (currentThread + 1) % threads.size();
	}
	int getQueueSize() {
		int size = 0;
		for(int i = 0; i < threads.size(); i++) {
			size += threads[i]->getQueueSize();
		}
		return size;
	}
	void exit() {
		for(int i = 0; i < threads.size(); i++) {
			threads[i]->waitForThread();
		}
	}
};
