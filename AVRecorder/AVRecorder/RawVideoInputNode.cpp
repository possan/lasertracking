//
//  RawVideoInputNode.cpp
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#include "RawVideoInputNode.h"
#include "misc.h"

//
// facetime - Y'CbCr 4:2:2 (uyvy)
//
// external - Y'CbCr 4:2:2 (yuvy)
//          - MJPEG Open DML
//

RawVideoInputNode::RawVideoInputNode() {
}

RawVideoInputNode::~RawVideoInputNode() {
}

void RawVideoInputNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
    // printf("in RawVideoInputNode, got %d bytes\n", event->data_length);
    // this node is just a proxy for the incoming events for now
    PipelineEvent *evtcopy = new PipelineEvent(event);
    strcpy(evtcopy->source_port, "out");
    node->emit(evtcopy);
}

void RawVideoInputNode::dataFromAV() {
}