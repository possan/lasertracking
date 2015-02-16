/*
     File: AVRecorderDocument.m
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

#import "AVRecorderDocument.h"
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>
#include "misc.h"

@interface AVRecorderDocument () <AVCaptureFileOutputDelegate, AVCaptureFileOutputRecordingDelegate> {
    unsigned char *rawVideoTempBuffer;
    NSThread *workerThread;
}

// Properties for internal use
@property (retain) AVCaptureDeviceInput *videoDeviceInput;
@property (retain) AVCaptureAudioPreviewOutput *audioPreviewOutput;
@property (retain) AVCaptureMovieFileOutput *movieFileOutput;
@property (retain) AVCaptureVideoPreviewLayer *previewLayer;
@property (assign) NSTimer *audioLevelTimer;
@property (retain) NSArray *observers;

// Methods for internal use
- (void)refreshDevices;

- (IBAction)showDebugClicked:(id)sender;

@end

@implementation AVRecorderDocument
@synthesize videoDeviceInput;
@synthesize videoDevices;
@synthesize session;
@synthesize movieFileOutput;
@synthesize previewView;
@synthesize previewLayer;
@synthesize observers;
@synthesize tabView;

- (id)init
{
	self = [super init];
	if (self) {
		// Create a capture session
		session = [[AVCaptureSession alloc] init];
		
		// Capture Notification Observers
		NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
		id runtimeErrorObserver = [notificationCenter addObserverForName:AVCaptureSessionRuntimeErrorNotification
																  object:session
																   queue:[NSOperationQueue mainQueue]
															  usingBlock:^(NSNotification *note) {
																  dispatch_async(dispatch_get_main_queue(), ^(void) {
																	  [self presentError:[[note userInfo] objectForKey:AVCaptureSessionErrorKey]];
																  });
															  }];
		id didStartRunningObserver = [notificationCenter addObserverForName:AVCaptureSessionDidStartRunningNotification
																	 object:session
																	  queue:[NSOperationQueue mainQueue]
																 usingBlock:^(NSNotification *note) {
																	 NSLog(@"did start running");
																 }];
		id didStopRunningObserver = [notificationCenter addObserverForName:AVCaptureSessionDidStopRunningNotification
																	object:session
																	 queue:[NSOperationQueue mainQueue]
																usingBlock:^(NSNotification *note) {
																	NSLog(@"did stop running");
																}];
		id deviceWasConnectedObserver = [notificationCenter addObserverForName:AVCaptureDeviceWasConnectedNotification
																		object:nil
																		 queue:[NSOperationQueue mainQueue]
																	usingBlock:^(NSNotification *note) {
																		[self refreshDevices];
																	}];
		id deviceWasDisconnectedObserver = [notificationCenter addObserverForName:AVCaptureDeviceWasDisconnectedNotification
																		   object:nil
																			queue:[NSOperationQueue mainQueue]
																	   usingBlock:^(NSNotification *note) {
																		   [self refreshDevices];
																	   }];
		observers = [[NSArray alloc] initWithObjects:runtimeErrorObserver, didStartRunningObserver, didStopRunningObserver, deviceWasConnectedObserver, deviceWasDisconnectedObserver, nil];
		
		// Attach outputs to session
		movieFileOutput = [[AVCaptureMovieFileOutput alloc] init];
		[movieFileOutput setDelegate:self];
		[session addOutput:movieFileOutput];
		
        // Select devices if any exist
		AVCaptureDevice *videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
		if (videoDevice) {
			[self setSelectedVideoDevice:videoDevice];
		} else {
			[self setSelectedVideoDevice:[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeMuxed]];
		}
		
        pipeline = new Pipeline();
        node_videoinput = new RawVideoInputNode();
        node_colorlevels = new ColorLevelsNode();
        node_colorfilter = new ColorFilterNode();
        node_unwrap = new PerspectiveUnwrapNode();
        node_threshold = new ThresholdNode();
        node_blobtracker = new BlobTrackerNode();
        // node_osc = new OSCNode();
        node_debug1 = new DebugImageNode();
        node_debug2 = new DebugImageNode();
        node_debug3 = new DebugImageNode();
        node_debug32 = new DebugImageNode();
        node_debug4 = new DebugImageNode();

        pipeline->addNode(new PipelineNode("video", node_videoinput));
        pipeline->addNode(new PipelineNode("levels", node_colorlevels));
        pipeline->addNode(new PipelineNode("colorfilter", node_colorfilter));
        pipeline->addNode(new PipelineNode("unwrap", node_unwrap));
        pipeline->addNode(new PipelineNode("threshold", node_threshold));
        pipeline->addNode(new PipelineNode("blobtracker", node_blobtracker));
        pipeline->addNode(new PipelineNode("debug1", node_debug1));
        pipeline->addNode(new PipelineNode("debug2", node_debug2));
        pipeline->addNode(new PipelineNode("debug3", node_debug3));
        pipeline->addNode(new PipelineNode("debug32", node_debug32));
        pipeline->addNode(new PipelineNode("debug4", node_debug4));
        
        pipeline->clearConnections();
        pipeline->addConnection(new PipelineConnection("video", "out", "levels", "in"));
        pipeline->addConnection(new PipelineConnection("levels", "out", "colorfilter", "in"));
        pipeline->addConnection(new PipelineConnection("colorfilter", "out", "unwrap", "in"));
        pipeline->addConnection(new PipelineConnection("unwrap", "out", "threshold", "in"));
        
        pipeline->addConnection(new PipelineConnection("levels", "out", "debug1", "in"));
        pipeline->addConnection(new PipelineConnection("colorfilter", "out", "debug2", "in"));

        pipeline->addConnection(new PipelineConnection("colorfilter", "out", "debug32", "in"));
        pipeline->addConnection(new PipelineConnection("unwrap", "debug", "debug32", "in"));
        
        pipeline->addConnection(new PipelineConnection("unwrap", "out", "debug3", "in"));
        pipeline->addConnection(new PipelineConnection("threshold", "out", "debug4", "in"));
        pipeline->addConnection(new PipelineConnection("threshold", "out", "blobtracker", "in"));
        pipeline->addConnection(new PipelineConnection("blobtracker", "debug", "debug4", "in"));
        // pipeline->addConnection(new PipelineConnection("video", "out", "debug5", "in"));
        
        workerThread = [[NSThread alloc] initWithTarget:self selector:@selector(workerThread) object:nil];
        [workerThread start];
        
		[self refreshDevices];
	}
	return self;
}

- (void)windowWillClose:(NSNotification *)notification
{
	// Stop the session
	[[self session] stopRunning];
	
	// Set movie file output delegate to nil to avoid a dangling pointer
	[[self movieFileOutput] setDelegate:nil];

	// Remove Observers
	NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
	for (id observer in [self observers])
		[notificationCenter removeObserver:observer];
	[observers release];
}

- (void)dealloc
{
	[videoDevices release];
	[session release];
	[movieFileOutput release];
	[previewLayer release];
	[videoDeviceInput release];
	
	[super dealloc];
}

- (NSString *)windowNibName
{
	return @"AVRecorderDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
	[super windowControllerDidLoadNib:aController];

    [self.tabView removeTabViewItem:[self.tabView tabViewItemAtIndex:1]];
    [self.tabView removeTabViewItem:[self.tabView tabViewItemAtIndex:0]];

    NSTabViewItem *tvi;
    
    {
        tvi = [[NSTabViewItem alloc] init];
        [tvi setLabel:@"Camera"];
        [self.tabView addTabViewItem:tvi];
    }
    
    {
        tvi = [[NSTabViewItem alloc] init];
        [tvi setLabel:@"RGB Levels"];
        [self.tabView addTabViewItem:tvi];
        m_colorLevelsVC = [[ColorLevelsVC alloc] initWithNibName:@"ColorLevelsVC" bundle:[NSBundle mainBundle]];
        m_colorLevelsVC.node = node_colorlevels;
        m_colorLevelsVC.debugnode = node_debug1;
        [tvi setView: m_colorLevelsVC.view];
    }
    
    {
        
        tvi = [[NSTabViewItem alloc] init];
        [tvi setLabel:@"HSV Filter"];
        [self.tabView addTabViewItem:tvi];
        m_colorFilterVC = [[ColorFilterVC alloc] initWithNibName:@"ColorFilterVC" bundle:[NSBundle mainBundle]];
        m_colorFilterVC.node = node_colorfilter;
        m_colorFilterVC.debugnode = node_debug2;
        [tvi setView: m_colorFilterVC.view];
    }
    
    {
        
        tvi = [[NSTabViewItem alloc] init];
        [tvi setLabel:@"Perspective Unwrap"];
        [self.tabView addTabViewItem:tvi];
        m_unwrapVC = [[UnwrapVC alloc] initWithNibName:@"UnwrapVC" bundle:[NSBundle mainBundle]];
        m_unwrapVC.node = node_unwrap;
        m_unwrapVC.debugnode = node_debug32;
        m_unwrapVC.debugnode2 = node_debug3;
        [tvi setView: m_unwrapVC.view];
    }
    
    {
        tvi = [[NSTabViewItem alloc] init];
        [tvi setLabel:@"Blob Tracking"];
        [self.tabView addTabViewItem:tvi];
        m_blobTrackerVC = [[BlobTrackingVC alloc] initWithNibName:@"BlobTrackingVC" bundle:[NSBundle mainBundle]];
        m_blobTrackerVC.thresholdnode = node_threshold;
        m_blobTrackerVC.debugnode = node_debug3;
        m_blobTrackerVC.debugnode2 = node_debug4;
        [tvi setView: m_blobTrackerVC.view];
    }
    
    {
        tvi = [[NSTabViewItem alloc] init];
        [tvi setLabel:@"Clear Zone Tracking"];
        [self.tabView addTabViewItem:tvi];
    }
    
    [self.tabView selectFirstTabViewItem:nil];
    
	// Attach preview to session
	CALayer *previewViewLayer = [[self previewView] layer];
	[previewViewLayer setBackgroundColor:CGColorGetConstantColor(kCGColorBlack)];
	AVCaptureVideoPreviewLayer *newPreviewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:[self session]];
	[newPreviewLayer setFrame:[previewViewLayer bounds]];
	[newPreviewLayer setAutoresizingMask:kCALayerWidthSizable | kCALayerHeightSizable];
	[previewViewLayer addSublayer:newPreviewLayer];
	[self setPreviewLayer:newPreviewLayer];
	[newPreviewLayer release];

	// Start the session
	[[self session] startRunning];
}

- (void)didPresentErrorWithRecovery:(BOOL)didRecover contextInfo:(void  *)contextInfo
{
	// Do nothing
}

#pragma mark - Device selection
- (void)refreshDevices
{
	[self setVideoDevices:[[AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo] arrayByAddingObjectsFromArray:[AVCaptureDevice devicesWithMediaType:AVMediaTypeMuxed]]];
	
	[[self session] beginConfiguration];
	
	if (![[self videoDevices] containsObject:[self selectedVideoDevice]])
		[self setSelectedVideoDevice:nil];
		
	[[self session] commitConfiguration];
}

- (void)captureOutput:(AVCaptureFileOutput *)captureOutput didFinishRecordingToOutputFileAtURL:(NSURL *)outputFileURL fromConnections:(NSArray *)connections error:(NSError *)error {
    
}

- (AVCaptureDevice *)selectedVideoDevice
{
	return [videoDeviceInput device];
}

- (void)setSelectedVideoDevice:(AVCaptureDevice *)selectedVideoDevice
{
	[[self session] beginConfiguration];
	
	if ([self videoDeviceInput]) {
		// Remove the old device input from the session
		[session removeInput:[self videoDeviceInput]];
		[self setVideoDeviceInput:nil];
	}
	
	if (selectedVideoDevice) {
		NSError *error = nil;
		
		// Create a device input for the device and add it to the session
		AVCaptureDeviceInput *newVideoDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:selectedVideoDevice error:&error];
		if (newVideoDeviceInput == nil) {
			dispatch_async(dispatch_get_main_queue(), ^(void) {
				[self presentError:error];
			});
		} else {
			if (![selectedVideoDevice supportsAVCaptureSessionPreset:[session sessionPreset]])
				[[self session] setSessionPreset:AVCaptureSessionPresetHigh];
			
			[[self session] addInput:newVideoDeviceInput];
			[self setVideoDeviceInput:newVideoDeviceInput];
		}
	}
	
	[[self session] commitConfiguration];
}

#pragma mark - Device Properties

+ (NSSet *)keyPathsForValuesAffectingVideoDeviceFormat
{
	return [NSSet setWithObjects:@"selectedVideoDevice.activeFormat", nil];
}

- (AVCaptureDeviceFormat *)videoDeviceFormat
{
	return [[self selectedVideoDevice] activeFormat];
}

- (void)setVideoDeviceFormat:(AVCaptureDeviceFormat *)deviceFormat
{
	NSError *error = nil;
	AVCaptureDevice *videoDevice = [self selectedVideoDevice];
	if ([videoDevice lockForConfiguration:&error]) {
		[videoDevice setActiveFormat:deviceFormat];
		[videoDevice unlockForConfiguration];
	} else {
		dispatch_async(dispatch_get_main_queue(), ^(void) {
			[self presentError:error];
		});
	}
}

+ (NSSet *)keyPathsForValuesAffectingFrameRateRange
{
	return [NSSet setWithObjects:@"selectedVideoDevice.activeFormat.videoSupportedFrameRateRanges", @"selectedVideoDevice.activeVideoMinFrameDuration", nil];
}

- (AVFrameRateRange *)frameRateRange
{
	AVFrameRateRange *activeFrameRateRange = nil;
	for (AVFrameRateRange *frameRateRange in [[[self selectedVideoDevice] activeFormat] videoSupportedFrameRateRanges])
	{
		if (CMTIME_COMPARE_INLINE([frameRateRange minFrameDuration], ==, [[self selectedVideoDevice] activeVideoMinFrameDuration]))
		{
			activeFrameRateRange = frameRateRange;
			break;
		}
	}
	
	return activeFrameRateRange;
}

- (void)setFrameRateRange:(AVFrameRateRange *)frameRateRange
{
	NSError *error = nil;
	if ([[[[self selectedVideoDevice] activeFormat] videoSupportedFrameRateRanges] containsObject:frameRateRange])
	{
		if ([[self selectedVideoDevice] lockForConfiguration:&error]) {
			[[self selectedVideoDevice] setActiveVideoMinFrameDuration:[frameRateRange minFrameDuration]];
			[[self selectedVideoDevice] unlockForConfiguration];
		} else {
			dispatch_async(dispatch_get_main_queue(), ^(void) {
				[self presentError:error];
			});
		}
	}
}

- (IBAction)lockVideoDeviceForConfiguration:(id)sender
{
	if ([(NSButton *)sender state] == NSOnState)
	{
		[[self selectedVideoDevice] lockForConfiguration:nil];
	}
	else
	{
		[[self selectedVideoDevice] unlockForConfiguration];
	}
}

- (BOOL)captureOutputShouldProvideSampleAccurateRecordingStart:(AVCaptureOutput *)captureOutput {
    return NO;
}

static int numFrames = 0;

-(void)captureOutput:(AVCaptureFileOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
    
    if (rawVideoTempBuffer == NULL) {
        rawVideoTempBuffer = (unsigned char *)malloc(2048*1024*4);
    }

    // memset(rawVideoTempBuffer, 0, 2048*1024*4);

    // Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    // Lock the base address of the pixel buffer
    CVPixelBufferLockBaseAddress(imageBuffer, 0);

    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);

    if (width < 1 || height < 1) {
        CVPixelBufferUnlockBaseAddress(imageBuffer,0);
        return;
    }


    // Get the number of bytes per row for the pixel buffer
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    
    // Get the number of bytes per row for the pixel buffer
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    // Get the pixel buffer width and height
    
    OSType pf = CVPixelBufferGetPixelFormatType(imageBuffer);
    
    // Unlock the pixel buffer
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
    
    unsigned char *rgb = (unsigned char *)baseAddress;
    
    numFrames ++;
   
    long outputlength = 0;
    
    if (pf == '2vuy') {
        unsigned char *yv = (unsigned char *)baseAddress;
        unsigned char *op = (unsigned char *)rawVideoTempBuffer;
        long io = 0;
        long oo = 0;
        for(int j=0; j<height * width; j+= 2) {
            unsigned char _b0 = yv[io ++];
            unsigned char _b1 = yv[io ++];
            unsigned char _b2 = yv[io ++];
            unsigned char _b3 = yv[io ++];

            float U = (float)(_b0);
            float V = (float)(_b2);
            float Y = (float)(_b1);
            
            float B = 1.164f * (Y - 16)                      + 2.018f * (U - 128);
            float G = 1.164f * (Y - 16) - 0.813f * (V - 128) - 0.391f * (U - 128);
            float R = 1.164f * (Y - 16) + 1.596f * (V - 128);
            
            if (R < 0) R = 0;
            if (G < 0) G = 0;
            if (B < 0) B = 0;
            if (R > 255) R = 255;
            if (G > 255) G = 255;
            if (B > 255) B = 255;
            
            unsigned char r = (unsigned char)R;
            unsigned char g = (unsigned char)G;
            unsigned char b = (unsigned char)B;
            
            op[oo ++] = r;
            op[oo ++] = g;
            op[oo ++] = b;
            op[oo ++] = 255;

            op[oo ++] = r;
            op[oo ++] = g;
            op[oo ++] = b;
            op[oo ++] = 255;
        }
        
        outputlength = width * height * 4;
        
    } else if (pf == 'yuvs') {
        unsigned char *yv = (unsigned char *)baseAddress;
        unsigned char *op = (unsigned char *)rawVideoTempBuffer;
        long io = 0;
        long oo = 0;
        for(int j=0; j<height * width; j+= 2) {
            unsigned char _b0 = yv[io ++];
            unsigned char _b1 = yv[io ++];
            unsigned char _b2 = yv[io ++];
            unsigned char _b3 = yv[io ++];
            
            float U = (float)(_b1);//(_b1 >> 0) & 15) * 16;
            float V = (float)(_b3);//(_b1 >> 4) & 15) * 16;
            float Y = (float)(_b0);
            
            // B = 1.164(Y - 16)                   + 2.018(U - 128)
            // G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
            // R = 1.164(Y - 16) + 1.596(V - 128)
            
            float B = 1.164f * (Y - 16)                      + 2.018f * (U - 128);
            float G = 1.164f * (Y - 16) - 0.813f * (V - 128) - 0.391f * (U - 128);
            float R = 1.164f * (Y - 16) + 1.596f * (V - 128);
            
            if (R < 0) R = 0;
            if (G < 0) G = 0;
            if (B < 0) B = 0;
            if (R > 255) R = 255;
            if (G > 255) G = 255;
            if (B > 255) B = 255;
            
            unsigned char r = (unsigned char)R;
            unsigned char g = (unsigned char)G;
            unsigned char b = (unsigned char)B;
            
            op[oo ++] = r;
            op[oo ++] = g;
            op[oo ++] = b;
            op[oo ++] = 255;
            
            op[oo ++] = r;
            op[oo ++] = g;
            op[oo ++] = b;
            op[oo ++] = 255;
        }
            
        outputlength = width * height * 4;
            
    } else {
        
        NSLog(@"got unknown pixelformat frame #%d: %ld x %ld with %ld bytes per row, pixelformat %c%c%c%c, first Y pixels: %d,%d,%d",
              numFrames, width, height, bytesPerRow, (pf>>24)&255,(pf>>16)&255,(pf>>8)&255,(pf>>0)&255, rgb[1],rgb[3],rgb[5]);
        
        memcpy(rawVideoTempBuffer, baseAddress, bytesPerRow * height);
        outputlength = bytesPerRow * height;
        
    }
    
    PipelineEvent *evt = new PipelineEvent();
    strcpy(evt->target_node, "video");
    strcpy(evt->target_port, "raw");
    evt->type = PE_TYPE_RGBAPIXELS;
    evt->i1 = width;
    evt->i2 = height;
    evt->i3 = numFrames;
    evt->data_length = outputlength;
    evt->data = rawVideoTempBuffer;
    evt->data_free = false;
    if (pipeline->queueSize() < 15) {
        pipeline->queue(evt);
    }
}

- (void)workerThread {
    while(true) {
        pipeline->flushQueue();
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [m_colorLevelsVC updatePreview];
            [m_colorFilterVC updatePreview];
            [m_unwrapVC updatePreview];
            [m_blobTrackerVC updatePreview];
        });
        usleep(1000);
    }
}

@end
