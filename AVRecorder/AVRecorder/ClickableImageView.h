//
//  ClickableImageView.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-10.
//
//

#import <Cocoa/Cocoa.h>

@class ClickableImageView;

@protocol ClickableImageViewDelegate

- (void) clickableImage:(ClickableImageView*)image didMouseDownAt:(NSPoint)point;
- (void) clickableImage:(ClickableImageView*)image didMouseDragAt:(NSPoint)point;
- (void) clickableImage:(ClickableImageView*)image didMouseUpAt:(NSPoint)point;

@end

@interface ClickableImageView : NSImageView

@property (nonatomic, assign) id<ClickableImageViewDelegate> delegate;

@end
