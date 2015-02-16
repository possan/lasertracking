//
//  DebugImageNode.cpp
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#include "DebugImageNode.h"
#include "misc.h"

DebugImageNode::DebugImageNode() {
    m_image = NULL;
    m_dirty = false;
    m_enabled = false;
    for(int k=0; k<10; k++) {
        m_dp_t[k] = 0;
    }
}

DebugImageNode::~DebugImageNode() {
}

void DebugImageNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
    // printf("DebugImageNode; got event %s.\n", event->toString());
    
    if (!m_enabled)
        return;

    if (event->type == PE_TYPE_DEBUGPOINT) {
        m_dp_x[event->i3] = event->i1;
        m_dp_y[event->i3] = event->i2;
        m_dp_t[event->i3] = 30;
    }
    
    if (event->type == PE_TYPE_RGBAPIXELS) {
        if (strcmp(event->target_port, "in") == 0) {
            int width = event->i1;
            int height = event->i2;
            // printf("Got RGB image of %d x %d pixels.", width, height);
            if (m_image == NULL) {
                // just create it.
                // printf("No previous image.");
            } else {
                size_t oldwidth = m_size.width;
                size_t oldheight = m_size.height;
                if (width != oldwidth || height != oldheight) {
                    // printf("Previous image of different size, resize.");
                    CGImageRelease(m_image);
                    m_image = NULL;
                }
            }
            
            if (m_image != NULL) {
                CGImageRelease(m_image);
                m_image = NULL;
            }
            
            // printf("Create debug image, %d x %d pixels.", width, height);
            
            unsigned char *bp = (unsigned char *) event->data;
            
            for(int k=0; k<10; k++) {
                if (m_dp_t[k] > 0) {
                    for(int dy=-5; dy<=5; dy++) {
                        for(int dx=-5; dx<=5; dx++) {
                            int x = dx + m_dp_x[k] * width / 100;
                            int y = dy + m_dp_y[k] * height / 100;
                            if (x >= 0 && y >= 0 && x < width && y < height) {
                                int oo = (y * width + x) * 4;
                                bp[ oo ] = 255;
                                bp[ oo+1 ] = 0;
                                bp[ oo+2 ] = 255;
                            }
                        }
                    }
                    m_dp_t[k] --;
                }
            }
            
            m_size = CGSizeMake(width, height);
            
            CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
            CGContextRef bitmapContext = CGBitmapContextCreate(event->data,
                                                               width,
                                                               height,
                                                               8,
                                                               4 * width,
                                                               colorSpace,
                                                               kCGImageAlphaNoneSkipLast);
            
            CFRelease(colorSpace);
            
            m_image = CGBitmapContextCreateImage(bitmapContext);
            m_dirty = true;
            
            CFRelease(bitmapContext);
        }
    }
    
    if (event->type == PE_TYPE_GRAYPIXELS) {
        if (strcmp(event->target_port, "in") == 0) {
            int width = event->i1;
            int height = event->i2;
            // printf("Got grayscale image of %d x %d pixels.", width, height);
            if (m_image == NULL) {
                // create it.
                // printf("No previous image.");
            } else {
                size_t oldwidth = m_size.width;
                size_t oldheight = m_size.height;
                if (width != oldwidth || height != oldheight) {
                    // printf("Previous image of different size, resize.");
                    CGImageRelease(m_image);
                    m_image = NULL;
                }
            }
            
            if (m_image != NULL) {
                CGImageRelease(m_image);
                m_image = NULL;
            }
            
            // printf("Create debug image, %d x %d pixels.\n", width, height);
            
            m_size = CGSizeMake(width, height);
            
            CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
            CGContextRef bitmapContext = CGBitmapContextCreate(event->data,
                                                               width,
                                                               height,
                                                               8,
                                                               1 * width,
                                                               colorSpace,
                                                               kCGImageAlphaNoneSkipLast);
            
            CFRelease(colorSpace);
            
            m_image = CGBitmapContextCreateImage(bitmapContext);
            m_dirty = true;
            
            CFRelease(bitmapContext);
        }
    }
    
}
