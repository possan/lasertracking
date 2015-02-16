/*
     File: AVRecorderDocument.h
 Abstract: n/a
  Version: 2.1
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2012 Apple Inc. All Rights Reserved.
 
 */

#import <Cocoa/Cocoa.h>
#import "pipeline.h"

#include "PerspectiveUnwrapNode.h"
#include "RawVideoInputNode.h"
#include "BlobTrackerNode.h"
#include "ColorFilterNode.h"
#include "ColorLevelsNode.h"
#include "OSCNode.h"
#include "DebugImageNode.h"
#include "ThresholdFilterNode.h"
#import "NodeEditorProtocol.h"
#import "ColorLevelsVC.h"
#import "ColorFilterVC.h"
#import "UnwrapVC.h"
#import "BlobTrackingVC.h"

@class AVCaptureVideoPreviewLayer;
@class AVCaptureSession;
@class AVCaptureDeviceInput;
@class AVCaptureMovieFileOutput;
@class AVCaptureConnection;
@class AVCaptureDevice;
@class AVCaptureDeviceFormat;
@class AVFrameRateRange;

@interface AVRecorderDocument : NSDocument <NSTabViewDelegate>
{
@private
	NSView						*previewView;
	AVCaptureVideoPreviewLayer	*previewLayer;
	AVCaptureSession			*session;
	AVCaptureDeviceInput		*videoDeviceInput;
	AVCaptureMovieFileOutput	*movieFileOutput;
	NSArray						*videoDevices;
	NSArray						*observers;
    
    Pipeline                    *pipeline;
    RawVideoInputNode           *node_videoinput;
    ColorLevelsNode             *node_colorlevels;
    ColorFilterNode             *node_colorfilter;
    PerspectiveUnwrapNode       *node_unwrap;
    ThresholdNode               *node_threshold;
    BlobTrackerNode             *node_blobtracker;

    DebugImageNode              *node_debug1;
    DebugImageNode              *node_debug2;
    DebugImageNode              *node_debug3;
    DebugImageNode              *node_debug32;
    DebugImageNode              *node_debug4;

    ColorLevelsVC *m_colorLevelsVC;
    ColorFilterVC *m_colorFilterVC;
    UnwrapVC *m_unwrapVC;
    BlobTrackingVC *m_blobTrackerVC;

    NSButton *showDebug;
    NSTabView *tabs;
    
    NSViewController<NodeEditorProtocol> *lastTabVC;
}

@property (retain) NSArray *videoDevices;
@property (assign) AVCaptureDevice *selectedVideoDevice;

@property (assign) AVCaptureDeviceFormat *videoDeviceFormat;
@property (assign) AVFrameRateRange *frameRateRange;
- (IBAction)lockVideoDeviceForConfiguration:(id)sender;

@property (retain) AVCaptureSession *session;

@property (assign) IBOutlet NSView *previewView;

@property (assign) IBOutlet NSTabView *tabView;

@end
