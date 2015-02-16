//
//  PerspectiveUnwrap.cpp
//  AVRecorder
//
//  Created by Per-Olov Jernberg on 2015-02-07.
//
//

#include "PerspectiveUnwrapNode.h"
#include "misc.h"

PerspectiveUnwrapNode::PerspectiveUnwrapNode() {
    m_x1 = 10;
    m_y1 = 10;
    m_x2 = 90;
    m_y2 = 10;
    m_x3 = 10;
    m_y3 = 90;
    m_x4 = 90;
    m_y4 = 90;
    m_outputwidth = 256;
    m_outputheight = 256;
    m_width = 0;
    m_height = 0;
    m_buffer = NULL;
}

PerspectiveUnwrapNode::~PerspectiveUnwrapNode() {
}

void PerspectiveUnwrapNode::updateCoeffs() {

    // calculate unwarping matrix + reverse (projection matrix).

    float sub = 1.0;
    float div = 2.0;

    float hw = 1.0;
    float hh = 1.0;
    float hl = 0;
    float ht = 0;

   // float scx = (0 + m_projectedwidth) / 2.0;
   // float scy = (0 + m_projectedheight) / 2.0;

    hw = m_projectedwidth / div;
    hh = m_projectedheight / div;
    hl = 0; // this.projectedleft / div;
    ht = 0; // this.projectedtop / div;

    float t_x1 = (((m_x1 * m_projectedwidth / 100.0)-hl) / hw) - sub;
    float t_y1 = (((m_y1 * m_projectedheight / 100.0)-ht) / hh) - sub;

    float t_x2 = (((m_x2 * m_projectedwidth / 100.0)-hl) / hw) - sub;
    float t_y2 = (((m_y2 * m_projectedheight / 100.0)-ht) / hh) - sub;

    float t_x3 = (((m_x3 * m_projectedwidth / 100.0)-hl) / hw) - sub;
    float t_y3 = (((m_y3 * m_projectedheight / 100.0)-ht) / hh) - sub;

    float t_x4 = (((m_x4 * m_projectedwidth / 100.0)-hl) / hw) - sub;
    float t_y4 = (((m_y4 * m_projectedheight / 100.0)-ht) / hh) - sub;

//    float coeff[ = [[0,0,0],[0,0,0],[0,0,0]];
#define RM(r,c) m_matrix[r*3+c]
#define WM(r,c,v) m_matrix[r*3+c] = v;

    float dx1, dx2, dx3, dy1, dy2, dy3;

    dx1 = t_x2 - t_x4;
    dx2 = t_x3 - t_x4;
    dx3 = t_x1 - t_x2 + t_x4 - t_x3;

    dy1 = t_y2 - t_y4;
    dy2 = t_y3 - t_y4;
    dy3 = t_y1 - t_y2 + t_y4 - t_y3;

    if ((dx3 == 0.0) && (dy3 == 0.0))
    {
        WM(0,0, t_x2 - t_x1);
        WM(0,1, t_x4 - t_x2);
        WM(0,2, t_x1);
        WM(1,0, t_y2 - t_y1);
        WM(1,1, t_y4 - t_y2);
        WM(1,2, t_y1);
        WM(2,0, 0.0);
        WM(2,1, 0.0);
    }
    else
    {
        float det1, det2;

        det1 = dx3 * dy2 - dy3 * dx2;
        det2 = dx1 * dy2 - dy1 * dx2;

        WM(2,0, (det2 == 0.0) ? 1.0 : det1 / det2);

        det1 = dx1 * dy3 - dy1 * dx3;

        WM(2,1, (det2 == 0.0) ? 1.0 : det1 / det2);

        WM(0,0, t_x2 - t_x1 + RM(2,0) * t_x2);
        WM(0,1, t_x3 - t_x1 + RM(2,1) * t_x3);
        WM(0,2, t_x1);

        WM(1,0, t_y2 - t_y1 + RM(2,0) * t_y2);
        WM(1,1, t_y3 - t_y1 + RM(2,1) * t_y3);
        WM(1,2, t_y1);
    }

    WM(2,2, 1.0);
#undef WM

//    console.log('projection matrix', JSON.stringify(coeff));
//    matrix = coeff;

   // function A(r,c) { return coeff[r][c]; }

    float determinant =
        +RM(0,0)*(RM(1,1)*RM(2,2)-RM(2,1)*RM(1,2))
        -RM(0,1)*(RM(1,0)*RM(2,2)-RM(1,2)*RM(2,0))
        +RM(0,2)*(RM(1,0)*RM(2,1)-RM(1,1)*RM(2,0));

    float invdet = 1.0 / determinant;

   // function R(r,c,v) { result[c][r] = v; };
#define WI(r,c,v) m_invmatrix[r*3+c] = v;
    WI(0,0, (RM(1,1)*RM(2,2)-RM(2,1)*RM(1,2))*invdet);
    WI(1,0,-(RM(0,1)*RM(2,2)-RM(0,2)*RM(2,1))*invdet);
    WI(2,0, (RM(0,1)*RM(1,2)-RM(0,2)*RM(1,1))*invdet);
    WI(0,1,-(RM(1,0)*RM(2,2)-RM(1,2)*RM(2,0))*invdet);
    WI(1,1, (RM(0,0)*RM(2,2)-RM(0,2)*RM(2,0))*invdet);
    WI(2,1,-(RM(0,0)*RM(1,2)-RM(1,0)*RM(0,2))*invdet);
    WI(0,2, (RM(1,0)*RM(2,1)-RM(2,0)*RM(1,1))*invdet);
    WI(1,2,-(RM(0,0)*RM(2,1)-RM(2,0)*RM(0,1))*invdet);
    WI(2,2, (RM(0,0)*RM(1,1)-RM(1,0)*RM(0,1))*invdet);
#undef WI
#undef RM

    // console.log('inverted matrix', JSON.stringify(result));
    // this.invmatrix = result;
}

void PerspectiveUnwrapNode::projectedToFlat(float px, float py, float *fx, float *fy) {
    /*
        project from projectedspace to flatspace
            a11*j + a12*i + a13
        x = -------------------
            a31*j + a32*i + a33
            a21*j + a22*i + a23
        y = -------------------
            a31*j + a32*i + a33
    */

    float htw = m_flatwidth / 1;
    float hth = m_flatheight / 1;
    float scx = (0 * 0.0) + (m_projectedwidth * 0.5);
    float scy = (0 * 0.0) + (m_projectedheight * 0.5);

    float u2 = (px - 1*scx) / scx;
    float v2 = (py - 1*scy) / scy;

#define RI(r,c) m_invmatrix[r*3+c]
    float xt = (RI(0,0)*u2) + (RI(0,1)*v2) + RI(0,2);
    float yt = (RI(1,0)*u2) + (RI(1,1)*v2) + RI(1,2);
    float di = (RI(2,0)*u2) + (RI(2,1)*v2) + RI(2,2);
#undef RI
    *fx = (htw * xt) / di;
    *fy = (hth * yt) / di;
}

void PerspectiveUnwrapNode::flatToProjected(float fx, float fy, float *px, float *py) {

    /*
        projects from flatspace to projectedspace
            a11*j + a12*i + a13
        x = -------------------
            a31*j + a32*i + a33
            a21*j + a22*i + a23
        y = -------------------
            a31*j + a32*i + a33
    */
    float hsw = m_projectedwidth / 2;
    float hsh = m_projectedheight / 2;
    float htw = m_flatwidth / 1;
    float hth = m_flatheight / 1;

    float u = (fx - 0 * htw) / htw;
    float v = (fy - 0 * hth) / hth;
    
#define RM(r,c) m_matrix[r*3+c]
    float xt = (RM(0,0) * u) + (RM(0,1) * v) + RM(0,2);
    float yt = (RM(1,0) * u) + (RM(1,1) * v) + RM(1,2);
    float di = (RM(2,0) * u) + (RM(2,1) * v) + RM(2,2);
#undef RI

    *px = hsw + (hsw * xt) / di;
    *py = hsh + (hsh * yt) / di;
}


void PerspectiveUnwrapNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
    if (event->type == PE_TYPE_RGBAPIXELS) {
        
        if (m_outputwidth != m_width || m_outputheight != m_height) {
            if (m_buffer != NULL) {
                free(m_buffer);
            }
            
            m_width = m_outputwidth;
            m_height = m_outputheight;
            m_buffer = (unsigned char *)malloc(m_width * m_height * 4);
        }
        
        m_projectedwidth = event->i1;
        m_projectedheight = event->i2;
        m_flatwidth = m_outputwidth;
        m_flatheight = m_outputheight;
        updateCoeffs();
        
        unsigned char *pin = (unsigned char *)event->data;
        unsigned char *pout = (unsigned char *)m_buffer;
        
        for(int j=0; j<m_outputheight; j++) {
            for(int i=0; i<m_outputwidth; i++) {
                
                float fx = i;
                float fy = j;
                
                float px = 0;
                float py = 0;

                flatToProjected(fx, fy, &px, &py);
               
                int ix = (int)px;
                int iy = (int)py;
                if (ix >= 0 && iy >= 0 && ix < m_projectedwidth && iy < m_projectedheight) {
                    int io = (((int)py * m_projectedwidth) + (int)px) * 4;
                    int oo = ((j * m_outputwidth) + i) * 4;
                    
                    pout[oo++] = pin[io++];
                    pout[oo++] = pin[io++];
                    pout[oo++] = pin[io++];
                    pout[oo++] = pin[io++];
                }
            }
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
        
        evt = new PipelineEvent();
        strcpy(evt->source_port, "debug");
        evt->type = PE_TYPE_DEBUGPOINT;
        evt->i1 = m_x1;
        evt->i2 = m_y1;
        evt->i3 = 1;
        node->emit(evt);
        
        evt = new PipelineEvent();
        strcpy(evt->source_port, "debug");
        evt->type = PE_TYPE_DEBUGPOINT;
        evt->i1 = m_x2;
        evt->i2 = m_y2;
        evt->i3 = 2;
        node->emit(evt);
        
        evt = new PipelineEvent();
        strcpy(evt->source_port, "debug");
        evt->type = PE_TYPE_DEBUGPOINT;
        evt->i1 = m_x3;
        evt->i2 = m_y3;
        evt->i3 = 3;
        node->emit(evt);
        
        evt = new PipelineEvent();
        strcpy(evt->source_port, "debug");
        evt->type = PE_TYPE_DEBUGPOINT;
        evt->i1 = m_x4;
        evt->i2 = m_y4;
        evt->i3 = 4;
        node->emit(evt);
    }
}

