// sometimes has an error on exit

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
public:
    queue<QueuedImage> queue;
    void threadedFunction() {
        while(isThreadRunning() && !queue.empty()) {
			ofSaveImage(queue.front().image, queue.front().filename);
			queue.pop();
        }
    }
    void saveImage(ofPixels& img, string filename) {
		queue.push(QueuedImage(img, filename));
		if(!isThreadRunning()){
			startThread();
		}
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
			threads.push_back(ofPtr<ThreadedImageSaver>(new ThreadedImageSaver()));
		}
	}
	void saveImage(ofPixels& img, string filename) {
		threads[currentThread]->saveImage(img, filename);
		currentThread = (currentThread + 1) % threads.size();
	}
	int getActiveThreads() {
		int active = 0;
		for(int i = 0; i < threads.size(); i++) {
			if(threads[i]->isThreadRunning()) {
				active++;
			}
		}
		return active;
	}
};