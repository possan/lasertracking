//
//  BlobTrackerNode.cpp
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#include "BlobTrackerNode.h"
#include "misc.h"

BlobTrackerNode::BlobTrackerNode() {
    m_buffer = (unsigned char *)malloc(16 * 16);
    m_frame = 0;
}

BlobTrackerNode::~BlobTrackerNode() {
}

void BlobTrackerNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
    if (event->type == PE_TYPE_RGBAPIXELS) {
        m_width = event->i1;
        m_height = event->i2;
        
        m_x1 = m_width;
        m_y1 = m_height;
        m_x2 = 0;
        m_y2 = 0;
        
        unsigned char *pin = (unsigned char *)event->data;
        
        for(int j=0; j<m_height; j++) {
            for(int i=0; i<m_width; i++) {
            
                unsigned char r = *pin++;
                unsigned char g = *pin++;
                unsigned char b = *pin++;
                pin++;
           
                int v = (g) > 127;
                
                if (v) {
                    // high pixel
                    
                    if (i < m_x1) m_x1 = i;
                    if (i > m_x2) m_x2 = i;

                    if (j < m_y1) m_y1 = j;
                    if (j > m_y2) m_y2 = j;
                }
            
            }
        }
        
        if (m_x2 > m_x1 && m_y2 > m_y1) {
            int cx = (m_x2 + m_x1) / 2;
            int cy = (m_y2 + m_y1) / 2;
            
            PipelineEvent *evt = new PipelineEvent();
            strcpy(evt->source_port, "debug");
            evt->type = PE_TYPE_DEBUGPOINT;
            evt->i1 = cx * 100 / m_width;
            evt->i2 = cy * 100 / m_height;
            evt->i3 = m_frame % 10;
            node->emit(evt);
            
            m_frame ++;
        }
    }
}
