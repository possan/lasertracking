//
//  ThresholdFilterNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__ThresholdFilterNode__
#define __AVRecorder__ThresholdFilterNode__

#include <stdio.h>
#include "pipeline.h"

class ThresholdNode : public IPipelineNodeProcessor {
private:
    int m_width;
    int m_height;
    unsigned char *m_buffer;
public:
    int m_threshold;
public:
    ThresholdNode();
    ~ThresholdNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};

#endif /* defined(__AVRecorder__ThresholdFilterNode__) */
