// global_defines_UOC.h

#ifndef __GLOBAL_DEFINES_UOC_H__
#define __GLOBAL_DEFINES_UOC_H__

//#define OPENCVFILESEP "//"
//static const CString STR_FILESEP = CString("\\");
#define BUFFER_LENGTH 5
#define FILESEPCHAR '\\'
#define FILESEP "/"
#define OLDFILESEP "\\"
// MUNI changed the size from 752->640
#define VRMAGIC_WIDTH  752 //640
#define VRMAGIC_HEIGHT 480
//#define INDIGO_WIDTH 324
//#define INDIGO_HEIGHT 256
#define FLOW_VECTOR_MAXLENGTH 25
#define FLOW_VECTOR_EXTREMELENGTH 125
#define MAX_SCHWERPUNKTS_DISTANZ 20 //Pix


// Variables used in CensusFlower_ObjectDetection
//#define VXWIDTH_HISTOGRAM 31 /// Vx width of the velocity histogram
//#define VYWIDTH_HISTOGRAM 31 /// Vx width of the velocity histogram
#define ENLARGEMENT_HISTOGRAM 10 /// Enlargement rate of the displayed histogram

// Located in same folder as global_defines_UOC.h
#include "Segment.hpp"

// STL:
#include <vector>

//struct segment {
//    int x1,y1,x2,y2;
//    int schwerpunkt_x, schwerpunkt_y, halbebreite_x, halbehoehe_y;
//    int predicted_schwerpunkt_x, predicted_schwerpunkt_y;
//    int pmd_x1,pmd_y1,pmd_x2,pmd_y2;
//    double timestamp;
//    int gueltigkeit;
//    int segment_id;
//    int TrackID;
//    int original_SegmentId;
//    int prev_seg_idx;
//    int class_no;
//    std::vector<double> prob_est;

//    double SegHeight;
//    double SegWidth;
//    double SegDepth;
//    double SegMeanDistance;
//    double SegStdDistance;
//    double SegMeanIntensity;
//    double SegStdIntensity;

//   int no_points;
//    std::vector<int> points_x;
//    std::vector<int> points_y;
//};

// Represents the initial and final positions of a pixel
struct flow_vector {
    int x1,y1,x2,y2;
};

struct flow_vec {
    int x2,y2,vx,vy;
	double length, angle;
	bool val;
};

//#define EXTENDED_VISUALISATION
//#define SAVE_ALL
//#define WAIT_VIS_ALL

#endif /*__GLOBAL_DEFINES_UOC_H__*/
