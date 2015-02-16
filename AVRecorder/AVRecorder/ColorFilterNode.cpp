//
//  ColorFilterNode.cpp
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#include "ColorFilterNode.h"
#include "misc.h"


ColorFilterNode::ColorFilterNode() {
    m_h1 = 180;
    m_h2 = 180;
    m_s1 = 128;
    m_s2 = 128;
    m_v1 = 128;
    m_v2 = 128;
    m_width = 0;
    m_height = 0;
    m_buffer = NULL;
}

ColorFilterNode::~ColorFilterNode() {
    if (m_buffer != NULL) {
        free(m_buffer);
    }
}



void ColorFilterNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
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
            
            
            
            

            int h, s, v;
            
            unsigned char rgbMin, rgbMax;
            
            rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
            rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);
            
            v = rgbMax;
            if (v == 0)
            {
                h = 0;
                s = 0;
            } else {
            
                s = 255 * (rgbMax - rgbMin) / v;
                if (s == 0) {
                    h = 0;
                } else {
                    if (rgbMax == r)
                        h = 0 + 60 * (g - b) / (rgbMax - rgbMin);
                    else if (rgbMax == g)
                        h = 120 + 60 * (b - r) / (rgbMax - rgbMin);
                    else
                        h = 240 + 60 * (r - g) / (rgbMax - rgbMin);
                }
            }
    
            int in_range = false;
            
            if (h > m_h1 - m_h2 && h < m_h1 + m_h2 &&
                s > m_s1 - m_s2 && s < m_s1 + m_s2 &&
                v > m_v1 - m_v2 && v < m_v1 + m_v2)
                in_range = true;
            
            if (in_range) {
                *pout++ = r;
                *pout++ = g;
                *pout++ = b;
            } else {
                *pout++ = 0;
                *pout++ = 0;
                *pout++ = 0;
            }
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

