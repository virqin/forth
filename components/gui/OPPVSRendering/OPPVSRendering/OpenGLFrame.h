//
//  OpenGLFrame.h
//  OPPVSRendering
//
//  Created by Cao Minh Trang on 5/13/15.
//  Copyright (c) 2015 Cao Minh Trang. All rights reserved.
//

#ifndef OPPVSRendering_OpenGLFrame_h
#define OPPVSRendering_OpenGLFrame_h

#import <QuartzCore/QuartzCore.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

@interface OpenGLFrame : CAOpenGLLayer
{
@private
    GLsizei frameWidth;
    GLsizei frameHeight;
}

@property GLubyte *pixelBuffer;
@property GLsizei frameWidth;
@property GLsizei frameHeight;

@property (assign,getter=isReverse) BOOL reverse;
@property (assign,getter=isInitialized) BOOL initialized;

- (void) setup;

@end


#endif
