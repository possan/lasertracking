//
//  ColorLevelsNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__ColorLevelsNode__
#define __AVRecorder__ColorLevelsNode__

#include <stdio.h>
#include "pipeline.h"

class ColorLevelsNode : public IPipelineNodeProcessor {
private:
    int m_width;
    int m_height;
    unsigned char *m_buffer;
public:
    int m_r1;
    int m_r2;
    int m_r3;
    int m_r4;
    int m_g1;
    int m_g2;
    int m_g3;
    int m_g4;
    int m_b1;
    int m_b2;
    int m_b3;
    int m_b4;
public:
    ColorLevelsNode();
    ~ColorLevelsNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};

#endif /* defined(__AVRecorder__ColorLevelsNode__) */
