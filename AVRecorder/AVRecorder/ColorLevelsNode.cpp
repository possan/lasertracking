//
//  ColorLevelsNode.cpp
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#include "ColorLevelsNode.h"
#include "misc.h"

ColorLevelsNode::ColorLevelsNode() {
    m_r1 = 10;
    m_r2 = 240;
    m_r3 = 0;
    m_r4 = 255;
    m_g1 = 10;
    m_g2 = 240;
    m_g3 = 0;
    m_g4 = 255;
    m_b1 = 10;
    m_b2 = 240;
    m_b3 = 0;
    m_b4 = 255;
    m_width = 0;
    m_height = 0;
    m_buffer = NULL;
}

ColorLevelsNode::~ColorLevelsNode() {
    if (m_buffer != NULL) {
        free(m_buffer);
    }
}

static unsigned char _lerp(unsigned char in, int p0, int p1, int p2, int p3) {
    float f = (float)in;
    
    if (f < p0)
        f = p0;
    if (f > p1)
        f = p1;
    
    f -= p0;
    f /= (p1 - p0);

    f *= (p3 - p2);
    f += p2;
        
    if (f < p2)
        f = p2;
    if (f > p3)
        f = p3;
    
    if (f < 0)
        f = 0;
    if (f > 255)
        f = 255;
    return (unsigned char)f;
};

void ColorLevelsNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
    if (event->type == PE_TYPE_RGBAPIXELS) {
        if (event->i1 != m_width || event->i2 != m_height) {
            if (m_buffer != NULL) {
                free(m_buffer);
            }

            m_width = event->i1;
            m_height = event->i2;
            m_buffer = (unsigned char *)malloc(m_width * m_height * 4);
        }
        
        unsigned char *pin = (unsigned char *)event->data;
        unsigned char *pout = (unsigned char *)m_buffer;
        
        for(int i=0; i<m_width*m_height; i++) {
            
            unsigned char r = *pin++;
            unsigned char g = *pin++;
            unsigned char b = *pin++;
            pin++;
            
            r = _lerp(r, m_r1, m_r2, m_r3, m_r4);
            g = _lerp(g, m_g1, m_g2, m_g3, m_g4);
            b = _lerp(b, m_b1, m_b2, m_b3, m_b4);
            
            *pout++ = r;
            *pout++ = g;
            *pout++ = b;
            pout ++;
        }
        
        
        PipelineEvent *evt = new PipelineEvent();
        strcpy(evt->source_port, "out");
        evt->type = PE_TYPE_RGBAPIXELS;
        evt->i1 = m_width;
        evt->i2 = m_height;
        evt->data_length = m_width * m_height * 4;
        evt->data = m_buffer;
        evt->data_free = false;
        node->emit(evt);
    }
}

