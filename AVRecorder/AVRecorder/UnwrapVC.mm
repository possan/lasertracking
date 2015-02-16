//
//  UnwrapVC.m
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-09.
//
//

#import "UnwrapVC.h"

@implementation UnwrapVC {
    bool m_visible;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    m_visible = false;
    self.previewImage.layer.backgroundColor = CGColorGetConstantColor(kCGColorBlack);
    self.previewImage.delegate = self;
    self.previewImage2.layer.backgroundColor = CGColorGetConstantColor(kCGColorBlack);
}

- (void)viewDidAppear {
    NSLog(@"appear");
    m_visible = true;
    self.debugnode->m_enabled = true;
    self.debugnode2->m_enabled = true;
    [self.unwrapX1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_x1]];
    [self.unwrapY1 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_y1]];
    [self.unwrapX2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_x2]];
    [self.unwrapY2 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_y2]];
    [self.unwrapX3 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_x3]];
    [self.unwrapY3 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_y3]];
    [self.unwrapX4 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_x4]];
    [self.unwrapY4 setStringValue:[NSString stringWithFormat:@"%d", self.node->m_y4]];
    [self.unwrapW setStringValue:[NSString stringWithFormat:@"%d", self.node->m_outputwidth]];
    [self.unwrapH setStringValue:[NSString stringWithFormat:@"%d", self.node->m_outputheight]];
}

- (void)viewWillDisappear {
    NSLog(@"disappear");
    m_visible = false;
    self.debugnode->m_enabled = false;
    self.debugnode2->m_enabled = false;
}

- (IBAction)valueChanged:(id)sender {
    self.node->m_x1 = [self.unwrapX1 intValue];
    self.node->m_y1 = [self.unwrapY1 intValue];
    self.node->m_x2 = [self.unwrapX2 intValue];
    self.node->m_y2 = [self.unwrapY2 intValue];
    self.node->m_x3 = [self.unwrapX3 intValue];
    self.node->m_y3 = [self.unwrapY3 intValue];
    self.node->m_x4 = [self.unwrapX4 intValue];
    self.node->m_y4 = [self.unwrapY4 intValue];
    self.node->m_outputwidth = [self.unwrapW intValue];
    self.node->m_outputheight = [self.unwrapH intValue];
}

- (IBAction)gestureTest:(id)sender {
    NSLog(@"in gesture!");
}

- (void) updatePreview {
    if (!m_visible)
        return;
    
    if (self.debugnode->m_dirty && self.debugnode->m_enabled) {
        self.debugnode->m_dirty = false;
        self.previewImage.image = nil;
        self.previewImage.image = [[NSImage alloc] initWithCGImage:self.debugnode->m_image size:self.debugnode->m_size];
    }
    if (self.debugnode2->m_dirty && self.debugnode2->m_enabled) {
        self.debugnode2->m_dirty = false;
        self.previewImage2.image = nil;
        self.previewImage2.image = [[NSImage alloc] initWithCGImage:self.debugnode2->m_image size:self.debugnode2->m_size];
    }
}

- (BOOL)isTextFieldInFocus:(NSTextField *)textField
{
    BOOL inFocus = NO;
    
    inFocus = ([[[textField window] firstResponder] isKindOfClass:[NSTextView class]]
               && [[textField window] fieldEditor:NO forObject:nil]!=nil
               && [textField isEqualTo:(id)[(NSTextView *)[[textField window] firstResponder]delegate]]);
    
    return inFocus;
}

- (void)clickableImage:(ClickableImageView *)image didMouseDownAt:(NSPoint)point {
    if ([self isTextFieldInFocus:self.unwrapX1] || [self isTextFieldInFocus:self.unwrapY1]) {
        [self.unwrapX1 setStringValue:[NSString stringWithFormat:@"%1.0f", point.x]];
        [self.unwrapY1 setStringValue:[NSString stringWithFormat:@"%1.0f", point.y]];
        self.node->m_x1 = [self.unwrapX1 intValue];
        self.node->m_y1 = [self.unwrapY1 intValue];
    }
    if ([self isTextFieldInFocus:self.unwrapX2] || [self isTextFieldInFocus:self.unwrapY2]) {
        [self.unwrapX2 setStringValue:[NSString stringWithFormat:@"%1.0f", point.x]];
        [self.unwrapY2 setStringValue:[NSString stringWithFormat:@"%1.0f", point.y]];
        self.node->m_x2 = [self.unwrapX2 intValue];
        self.node->m_y2 = [self.unwrapY2 intValue];
    }
    if ([self isTextFieldInFocus:self.unwrapX3] || [self isTextFieldInFocus:self.unwrapY3]) {
        [self.unwrapX3 setStringValue:[NSString stringWithFormat:@"%1.0f", point.x]];
        [self.unwrapY3 setStringValue:[NSString stringWithFormat:@"%1.0f", point.y]];
        self.node->m_x3 = [self.unwrapX3 intValue];
        self.node->m_y3 = [self.unwrapY3 intValue];
    }
    if ([self isTextFieldInFocus:self.unwrapX4] || [self isTextFieldInFocus:self.unwrapY4]) {
        [self.unwrapX4 setStringValue:[NSString stringWithFormat:@"%1.0f", point.x]];
        [self.unwrapY4 setStringValue:[NSString stringWithFormat:@"%1.0f", point.y]];
        self.node->m_x4 = [self.unwrapX4 intValue];
        self.node->m_y4 = [self.unwrapY4 intValue];
    }
}

- (void)clickableImage:(ClickableImageView *)image didMouseDragAt:(NSPoint)point {
    [self clickableImage:image didMouseDownAt:point];
}

- (void)clickableImage:(ClickableImageView *)image didMouseUpAt:(NSPoint)point {
    
}

@end
