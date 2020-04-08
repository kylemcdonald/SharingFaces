//
//  PeertalkBridgingHeader.h
//  PeertalkTest
//
//  Created by Elliott Spelman on 2/27/19.
//

#ifndef PeertalkBridgingHeader_h
#define PeertalkBridgingHeader_h

#import <Foundation/Foundation.h>
#include "ofMain.h"

class PTManager
{
public:
    PTManager ( void );
    ~PTManager( void );
    
    int someNumber;
    
    void init( void );
    int  doSomethingWith( void * aParameter );
    void processMessage( int * messageInt );
    
private:
    void * self;
};

#endif /* PeertalkBridgingHeader_h */
