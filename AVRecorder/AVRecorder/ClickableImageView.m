//
//  ClickableImageView.m
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-10.
//
//

#import "ClickableImageView.h"

@implementation ClickableImageView {
    CGPoint lastPoint;
}

@synthesize delegate;

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    // Drawing code here.
}

- (BOOL)mouse:(NSPoint)aPoint inRect:(NSRect)aRect {
    lastPoint = aPoint;
    return [super mouse:aPoint inRect:aRect];
}

- (void)mouseMoved:(NSEvent *)theEvent {
}

- (void)mouseDragged:(NSEvent *)theEvent {
    NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    pt.x *= 100;
    pt.y *= 100;
    pt.x /= self.frame.size.width;
    pt.y /= self.frame.size.height;
    pt.y = 100 - pt.y;
    NSLog(@"mouse dragged: %f, %f", pt.x, pt.y);
    [self.delegate clickableImage:self didMouseDragAt:pt];
}

- (void) mouseUp:(NSEvent *)theEvent {
    NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    pt.x *= 100;
    pt.y *= 100;
    pt.x /= self.frame.size.width;
    pt.y /= self.frame.size.height;
    pt.y = 100 - pt.y;
    NSLog(@"mouse up: %f, %f", pt.x, pt.y);
    [self.delegate clickableImage:self didMouseUpAt:pt];
}

- (void) mouseExited:(NSEvent *)theEvent {
}

- (void)mouseDown:(NSEvent *)theEvent {
    NSPoint pt = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    pt.x *= 100;
    pt.y *= 100;
    pt.x /= self.frame.size.width;
    pt.y /= self.frame.size.height;
    pt.y = 100 - pt.y;
    NSLog(@"mouse down: %f, %f", pt.x, pt.y);
    [self.delegate clickableImage:self didMouseDownAt:pt];
}

@end
