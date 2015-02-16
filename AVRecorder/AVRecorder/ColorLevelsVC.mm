//
//  ColorLevelsVC.m
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import "ColorLevelsVC.h"
#import "misc.h"

@interface ColorLevelsVC ()

@end

@implementation ColorLevelsVC {
    bool m_visible;
}

@synthesize previewImage;
@synthesize levelsR1;
@synthesize levelsR2;
@synthesize levelsR3;
@synthesize levelsR4;
@synthesize levelsG1;
@synthesize levelsG2;
@synthesize levelsG3;
@synthesize levelsG4;
@synthesize levelsB1;
@synthesize levelsB2;
@synthesize levelsB3;
@synthesize levelsB4;

- (IBAction)valueChanged:(id)sender {
    NSLog(@"save values");
    self.node->m_r1 = [self.levelsR1 intValue];
    self.node->m_r2 = [self.levelsR2 intValue];
    self.node->m_r3 = [self.levelsR3 intValue];
    self.node->m_r4 = [self.levelsR4 intValue];
    self.node->m_g1 = [self.levelsG1 intValue];
    self.node->m_g2 = [self.levelsG2 intValue];
    self.node->m_g3 = [self.levelsG3 intValue];
    self.node->m_g4 = [self.levelsG4 intValue];
    self.node->m_b1 = [self.levelsB1 intValue];
    self.node->m_b2 = [self.levelsB2 intValue];
    self.node->m_b3 = [self.levelsB3 intValue];
    self.node->m_b4 = [self.levelsB4 intValue];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"bind values");
    m_visible = false;
    self.previewImage.layer.backgroundColor = CGColorGetConstantColor(kCGColorBlack);
}

- (void)viewDidAppear {
    NSLog(@"appear");
    m_visible = true;
    self.debugnode->m_enabled = true;
    [self.levelsR1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_r1]];
    [self.levelsR2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_r2]];
    [self.levelsR3 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_r3]];
    [self.levelsR4 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_r4]];
    [self.levelsG1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_g1]];
    [self.levelsG2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_g2]];
    [self.levelsG3 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_g3]];
    [self.levelsG4 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_g4]];
    [self.levelsB1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_b1]];
    [self.levelsB2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_b2]];
    [self.levelsB3 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_b3]];
    [self.levelsB4 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_b4]];
}

- (void)viewWillDisappear {
    NSLog(@"disappear");
    m_visible = false;
    self.debugnode->m_enabled = false;
}

- (void) updatePreview {
    if (!m_visible)
        return;
    
    if (self.debugnode->m_dirty && self.debugnode->m_enabled) {
        self.debugnode->m_dirty = false;
        self.previewImage.image = nil;
        self.previewImage.image = [[NSImage alloc] initWithCGImage:self.debugnode->m_image size:self.debugnode->m_size];
    }
}

@end
