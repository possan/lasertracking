//
//  ColorFilterVC.m
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import "ColorFilterVC.h"

@interface ColorFilterVC ()

@end

@implementation ColorFilterVC {
    bool m_visible;
}

@synthesize hue1;
@synthesize hue2;
@synthesize sat1;
@synthesize sat2;
@synthesize val1;
@synthesize val2;
@synthesize previewImage;

- (void)viewDidLoad {
    [super viewDidLoad];
    m_visible = false;
    self.previewImage.layer.backgroundColor = CGColorGetConstantColor(kCGColorBlack);
}

- (void)viewDidAppear {
    NSLog(@"appear");
    m_visible = true;
    self.debugnode->m_enabled = true;
    [self.hue1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_h1]];
    [self.hue2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_h2]];
    [self.sat1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_s1]];
    [self.sat2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_s2]];
    [self.val1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_v1]];
    [self.val2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_v2]];
}

- (void)viewWillDisappear {
    NSLog(@"disappear");
    m_visible = false;
    self.debugnode->m_enabled = false;
}

- (IBAction)valueChanged:(id)sender {
    self.node->m_h1 = [self.hue1 intValue];
    self.node->m_h2 = [self.hue2 intValue];
    self.node->m_s1 = [self.sat1 intValue];
    self.node->m_s2 = [self.sat2 intValue];
    self.node->m_v1 = [self.val1 intValue];
    self.node->m_v2 = [self.val2 intValue];
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
