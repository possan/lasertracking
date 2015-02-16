//
//  PerspectiveUnwrapNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__PerspectiveUnwrap__
#define __AVRecorder__PerspectiveUnwrap__

#include <stdio.h>
#include "pipeline.h"

class PerspectiveUnwrapNode : public IPipelineNodeProcessor {
private:
    int m_width;
    int m_height;
    unsigned char *m_buffer;
    float m_matrix[9];
    float m_invmatrix[9];
    void updateCoeffs();
    void projectedToFlat(float px, float py, float *fx, float *fy);
    void flatToProjected(float fx, float fy, float *px, float *py);
    float m_projectedwidth;
    float m_projectedheight;
    float m_flatwidth;
    float m_flatheight;
public:
    int m_x1;
    int m_y1;
    int m_x2;
    int m_y2;
    int m_x3;
    int m_y3;
    int m_x4;
    int m_y4;
    int m_outputwidth;
    int m_outputheight;
public:
    PerspectiveUnwrapNode();
    ~PerspectiveUnwrapNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};

#endif /* defined(__AVRecorder__PerspectiveUnwrap__) */
