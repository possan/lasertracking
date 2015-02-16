//
//  BlobTrackingVC.m
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import "BlobTrackingVC.h"

@implementation BlobTrackingVC {
    bool m_visible;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    m_visible = false;
    self.previewImage.layer.backgroundColor = CGColorGetConstantColor(kCGColorBlack);
    self.previewImage2.layer.backgroundColor = CGColorGetConstantColor(kCGColorBlack);
}

- (void)viewDidAppear {
    NSLog(@"appear");
    m_visible = true;
    self.debugnode->m_enabled = true;
    self.debugnode2->m_enabled = true;
    [self.threshold setStringValue:[NSString stringWithFormat:@"%d", self.thresholdnode->m_threshold]];
}

- (void)viewWillDisappear {
    NSLog(@"disappear");
    m_visible = false;
    self.debugnode->m_enabled = false;
    self.debugnode2->m_enabled = false;
}

- (IBAction)valueChanged:(id)sender {
    self.thresholdnode->m_threshold = [self.threshold intValue];
}

- (void) updatePreview {
    if (!m_visible)
        return;
    
    if (self.debugnode->m_dirty && self.debugnode->m_enabled) {
        self.debugnode->m_dirty = false;
        self.previewImage.image = [[NSImage alloc] initWithCGImage:self.debugnode->m_image size:self.debugnode->m_size];
    }
    
    if (self.debugnode2->m_dirty && self.debugnode2->m_enabled) {
        self.debugnode2->m_dirty = false;
        self.previewImage2.image = [[NSImage alloc] initWithCGImage:self.debugnode2->m_image size:self.debugnode2->m_size];
    }
}

@end
