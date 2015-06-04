//
//  Document.h
//  OPPVSGUI
//
//  Created by Cao Minh Trang on 3/13/15.
//  Copyright (c) 2015 Cao Minh Trang. All rights reserved.
//

#pragma once

#import <Cocoa/Cocoa.h>
#include "mac_video_capture.hpp"
#include "stream.hpp"
#include "streaming_engine.hpp"

#include "ViewController.h"


@class ViewController;

@interface Document : NSDocument
{
@private
    oppvs::MacVideoEngine *videoEngine;
    oppvs::PixelBuffer *sharedBuffer;
}

@property NSMutableArray *videoCaptureDevices;
@property NSMutableDictionary *windowCaptureInputs;
@property NSInteger severPort;

- (void) addSource: (NSString*) sourceid hasType: (oppvs::VideoSourceType) type sourceRect: (CGRect)srect renderRect: (CGRect)rrect withViewID: (id) viewid;
- (void) startRecording;
- (void) stopRecording;
- (void) startStreaming;
- (void) stopStreaming;

@end

