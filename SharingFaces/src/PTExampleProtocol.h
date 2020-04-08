//
//  PTExampleProtocol.h
//  PeertalkTest
//
//  Created by Elliott Spelman on 2/28/19.
//

#ifndef peertalk_PTExampleProtocol_h
#define peertalk_PTExampleProtocol_h

#import <Foundation/Foundation.h>
#include <stdint.h>

static const int PTExampleProtocolIPv4PortNumber = 2345;

enum {
    PTExampleFrameTypeDeviceInfo = 100,
    PTExampleFrameTypeTextMessage = 101,
    PTExampleFrameTypePing = 102,
    PTExampleFrameTypePong = 103,
    PTFacePRSData = 104,
    PTFaceExpressionData = 105,
    PTFaceHasArrived = 106,
    PTFaceHasLeft = 107,
};

typedef struct _PTExampleTextFrame {
    uint32_t length;
    uint8_t utf8text[0];
} PTExampleTextFrame;


static dispatch_data_t PTExampleTextDispatchDataWithString(NSString *message) {
    // Use a custom struct
    const char *utf8text = [message cStringUsingEncoding:NSUTF8StringEncoding];
    size_t length = strlen(utf8text);
    PTExampleTextFrame *textFrame = (PTExampleTextFrame*)CFAllocatorAllocate(nil, sizeof(PTExampleTextFrame) + length, 0);
    memcpy(textFrame->utf8text, utf8text, length); // Copy bytes to utf8text array
    textFrame->length = htonl(length); // Convert integer to network byte order
    
    // Wrap the textFrame in a dispatch data object
    return dispatch_data_create((const void*)textFrame, sizeof(PTExampleTextFrame)+length, nil, ^{
        CFAllocatorDeallocate(nil, textFrame);
    });
}

#endif /* PTExampleProtocol_h */
