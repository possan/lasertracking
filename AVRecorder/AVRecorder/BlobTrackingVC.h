//
//  BlobTrackingVC.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import <Cocoa/Cocoa.h>
#import "DebugImageNode.h"
#import "ThresholdFilterNode.h"
#import "NodeEditorProtocol.h"

@interface BlobTrackingVC : NSViewController<NodeEditorProtocol>

@property (nonatomic, readwrite) ThresholdNode *thresholdnode;
@property (nonatomic, readwrite) DebugImageNode *debugnode;
@property (nonatomic, readwrite) DebugImageNode *debugnode2;

@property (assign) IBOutlet NSTextField *threshold;

@property (assign) IBOutlet NSImageView *previewImage;
@property (assign) IBOutlet NSImageView *previewImage2;

- (IBAction)valueChanged:(id)sender;

@end
