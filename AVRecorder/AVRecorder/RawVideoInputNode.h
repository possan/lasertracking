//
//  RawVideoInputNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__RawVideoInputNode__
#define __AVRecorder__RawVideoInputNode__

#include <stdio.h>
#include "pipeline.h"

class RawVideoInputNode : public IPipelineNodeProcessor {
public:
    RawVideoInputNode();
    ~RawVideoInputNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
    void dataFromAV();
};

#endif /* defined(__AVRecorder__RawVideoInputNode__) */
