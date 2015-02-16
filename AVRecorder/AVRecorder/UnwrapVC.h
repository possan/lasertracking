//
//  UnwrapVC.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import <Cocoa/Cocoa.h>
#import "NodeEditorProtocol.h"
#import "PerspectiveUnwrapNode.h"
#import "DebugImageNode.h"
#import "ClickableImageView.h"

@interface UnwrapVC : NSViewController<NodeEditorProtocol, ClickableImageViewDelegate>

@property (nonatomic, readwrite) PerspectiveUnwrapNode *node;
@property (nonatomic, readwrite) DebugImageNode *debugnode;
@property (nonatomic, readwrite) DebugImageNode *debugnode2;

@property (assign) IBOutlet NSTextField *unwrapX1;
@property (assign) IBOutlet NSTextField *unwrapY1;
@property (assign) IBOutlet NSTextField *unwrapX2;
@property (assign) IBOutlet NSTextField *unwrapY2;
@property (assign) IBOutlet NSTextField *unwrapX3;
@property (assign) IBOutlet NSTextField *unwrapY3;
@property (assign) IBOutlet NSTextField *unwrapX4;
@property (assign) IBOutlet NSTextField *unwrapY4;
@property (assign) IBOutlet NSTextField *unwrapW;
@property (assign) IBOutlet NSTextField *unwrapH;

@property (assign) IBOutlet ClickableImageView *previewImage;
@property (assign) IBOutlet NSImageView *previewImage2;

- (IBAction)valueChanged:(id)sender;
- (IBAction)gestureTest:(id)sender;

@end
