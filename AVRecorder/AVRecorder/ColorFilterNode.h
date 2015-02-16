
//
//  ColorFilterNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__ColorFilterNode__
#define __AVRecorder__ColorFilterNode__

#include <stdio.h>
#include "pipeline.h"

class ColorFilterNode : public IPipelineNodeProcessor {
private:
    int m_width;
    int m_height;
    unsigned char *m_buffer;
public:
    int m_h1;
    int m_h2;
    int m_s1;
    int m_s2;
    int m_v1;
    int m_v2;
public:
    ColorFilterNode();
    ~ColorFilterNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};

#endif /* defined(__AVRecorder__ColorFilterNode__) */
