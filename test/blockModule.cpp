#include <stdio.h>
#include <iostream>
#include <stdint.h>


typedef struct tag_HRegFeature
{
//    HFeatureFlags flags;
    uint8_t shape;/* SHAPE_*                                      */
    bool is_convex;
    bool is_filled;
    bool is_connected4;
    bool is_connected8;
    bool is_thin;/* one pixel thin                               */
    float circularity;
    float compactness;
    float contlength;
    float convexity;
    float phi;
    float ra,rb;/* elliptic_axis                                */
    float ra_,rb_;/* elliptic_shape                               */
    float anisometry,bulkiness,structure_factor;
    float m11,m20,m02,ia,ib;
    float row,col;
    uint32_t area;
    uint16_t row1,col1,row2,col2;
    float row_rect,col_rect,phi_rect,length1,length2;
    float row_circle,col_circle,radius;
    uint16_t min_chord,max_chord;
    uint16_t min_chord_gap,max_chord_gap;
    float rectangularity;

}HRegFeature;


typedef struct {
    uint16_t r;
    uint16_t cb;
    uint16_t ce;
}Hrun;

typedef struct tag_Hrlregion{
    bool is_compl;
 //   HITEMCNT num;
  //  HITEMCNT num_max;
    uint32_t *ref_count;
    HRegFeature feature;
    Hrun* rl;
}Hrlregion;


void threshold(char** Image, Hrlregion* out, double MinGray, double MaxGray)
{


}



int main()
{



    return 0;
}
