//
//  ColorFilterVC.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import <Cocoa/Cocoa.h>
#import "ColorFilterNode.h"
#import "NodeEditorProtocol.h"
#import "DebugImageNode.h"

@interface ColorFilterVC : NSViewController<NodeEditorProtocol>

@property (nonatomic, readwrite) ColorFilterNode *node;
@property (nonatomic, readwrite) DebugImageNode *debugnode;

@property (assign) IBOutlet NSTextField *hue1;
@property (assign) IBOutlet NSTextField *hue2;
@property (assign) IBOutlet NSTextField *sat1;
@property (assign) IBOutlet NSTextField *sat2;
@property (assign) IBOutlet NSTextField *val1;
@property (assign) IBOutlet NSTextField *val2;

@property (assign) IBOutlet NSImageView *previewImage;

- (IBAction)valueChanged:(id)sender;

@end
