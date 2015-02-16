//
//  BlobTrackerNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__BlobTrackerNode__
#define __AVRecorder__BlobTrackerNode__

#include <stdio.h>
#include "pipeline.h"

class BlobTrackerNode : public IPipelineNodeProcessor {
private:
    int m_width;
    int m_height;
    int m_x1;
    int m_y1;
    int m_x2;
    int m_y2;
    int m_frame;
    unsigned char *m_buffer;
public:
    BlobTrackerNode();
    ~BlobTrackerNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};

#endif /* defined(__AVRecorder__BlobTrackerNode__) */
