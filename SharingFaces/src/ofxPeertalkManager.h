#import "PeertalkBridgingHeader.h"
#import <Cocoa/Cocoa.h>
#import "PTChannel.h"

static const NSTimeInterval PTAppReconnectDelay = 1.0;

@interface ofxPeertalkManager : NSObject<NSApplicationDelegate, PTChannelDelegate> {
    
}


@end
