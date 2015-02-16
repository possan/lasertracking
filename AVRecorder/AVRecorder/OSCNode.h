//
//  OSCNode.h
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#ifndef __AVRecorder__OSCNode__
#define __AVRecorder__OSCNode__

#include <stdio.h>
#include "pipeline.h"

class OSCNode : public IPipelineNodeProcessor {
public:
    OSCNode();
    ~OSCNode();
    void handleEvent(PipelineNode *node, PipelineEvent *event);
};


#endif /* defined(__AVRecorder__OSCNode__) */
