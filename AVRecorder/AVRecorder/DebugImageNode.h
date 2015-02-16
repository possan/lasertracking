//
//  DebugImageNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__DebugImageNode__
#define __AVRecorder__DebugImageNode__

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include "pipeline.h"

class DebugImageNode : public IPipelineNodeProcessor {
public:
    CGImageRef m_image;
    CGSize m_size;
    bool m_dirty;
    bool m_enabled;
    int m_dp_x[10];
    int m_dp_y[10];
    int m_dp_t[10];
    
public:
    DebugImageNode();
    ~DebugImageNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};

#endif /* defined(__AVRecorder__DebugImageNode__) */
