//
//  ColorLevelsVC.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import <Cocoa/Cocoa.h>
#import "ColorLevelsNode.h"
#import "misc.h"
#import "NodeEditorProtocol.h"
#import "DebugImageNode.h"

@interface ColorLevelsVC : NSViewController<NodeEditorProtocol>

@property (nonatomic, readwrite) ColorLevelsNode *node;
@property (nonatomic, readwrite) DebugImageNode *debugnode;

@property (assign) IBOutlet NSTextField *levelsR1;
@property (assign) IBOutlet NSTextField *levelsR2;
@property (assign) IBOutlet NSTextField *levelsR3;
@property (assign) IBOutlet NSTextField *levelsR4;
@property (assign) IBOutlet NSTextField *levelsG1;
@property (assign) IBOutlet NSTextField *levelsG2;
@property (assign) IBOutlet NSTextField *levelsG3;
@property (assign) IBOutlet NSTextField *levelsG4;
@property (assign) IBOutlet NSTextField *levelsB1;
@property (assign) IBOutlet NSTextField *levelsB2;
@property (assign) IBOutlet NSTextField *levelsB3;
@property (assign) IBOutlet NSTextField *levelsB4;

@property (assign) IBOutlet NSImageView *previewImage;

- (IBAction)valueChanged:(id)sender;

@end
