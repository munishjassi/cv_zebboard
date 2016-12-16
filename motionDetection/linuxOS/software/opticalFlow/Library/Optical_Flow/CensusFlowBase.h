// CensusFlowBase.h


#ifndef __CENSUSFLOWBASE_H__
#define __CENSUSFLOWBASE_H__


// STL:
#include <vector>
//#include <deque>
#include <iostream>
#include <string>
//OpenCV Includes
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
// Eigene:
#include "../global_defines_UOC.h"
#include "./storage64b.h"

#define LENGTH_FLOWVEKTOR 400000
#define WINDOW_WIDTH_X 20
#define WINDOW_HEIGTH_Y 20
#define MAX_DIFF_WX 10
#define MAX_DIFF_HY 15
#define WINDOW_WIDTH_X_HW 3
#define WINDOW_HEIGTH_Y_HW 3
#define MAX_DIFF_WX_HW 1
#define MAX_DIFF_HY_HW 1
#define MAX_DIFF_ROUGH_WX 45
#define MAX_DIFF_ROUGH_HY 55
#define LENGTH_LONGVEC 2


class CensusFlowBase
{
public:
    CensusFlowBase(int image_height, int image_widthStep, int image_width,
                   int roi_h1, int roi_w1, int roi_h2, int roi_w2);
    ~CensusFlowBase(void);
    //virtual ~CensusFlowBase(void);
    
    // Parameter:
    int m_image_height, m_image_widthStep, m_image_width;  //allgemeine parameter
    int m_roi_h1, m_roi_w1, m_roi_h2, m_roi_w2;  // ROI : Region Of Interest
    IplImage * m_filtered_image;

    IplImage *m_vx_image, *m_vy_image, *m_val_image; // Representation1a: vector_3image 
    IplImage *m_vx_vy_val_image;                     // Representation1b: vector_image3x

    int m_window_width_x, m_window_heigth_y;  //fuer Postprocessing
    int m_no_windows_wx, m_start_wx, m_end_point_wx;
    int m_no_windows_hy, m_start_hy, m_end_point_hy;
    IplImage *m_vx_vy_val_mean_image;

	//Representation 2
    int size_flow_vector_array,  /// Size of tje flow_vector_array 
		zaeler_flow_vector_array; /// Number of pixel for which a valid match has been found.
    flow_vector *flow_vector_array; /// Initial position and final position of each pixel for which a match has been found
    std::vector<CvScalar> color_LUT;

    IplImage* SmoothImage1(IplImage* pocess_img, int sizeblur=3);
    IplImage* SmoothImage2(IplImage* pocess_img);

    int PostProcess_fast(int max_diff_wx = MAX_DIFF_WX, int max_diff_hy = MAX_DIFF_HY);
    int PostProcess_HW (int w_search_postprocessing = WINDOW_WIDTH_X_HW, int h_search_postprocessing = WINDOW_HEIGTH_Y_HW, int max_diff_wx = MAX_DIFF_WX_HW, int max_diff_hy = MAX_DIFF_HY_HW);
    void PostProcess(IplImage* _vx_vy_val_image=NULL, flow_vector *flow_vector_arr=NULL, int* zaeler_flow_vector_arr=NULL, int* size_flow_vector_arr=NULL);
    void ComputeRaster(int roi_wh1, int roi_wh2, int window_wh, int& start_wh, int& no_windows, int& end_wh);
	//! Initialisiert weitere Elemente
    void Initialize(void);
    void Init_Cycle(void);
    void vis_flow1(IplImage* target_im);// Pfeile
    void vis_flow2(IplImage* target_im, flow_vector* _flow_vector_array=NULL, int _zaeler_flow_vector_array=0);// Linen
    void vis_flow3(IplImage* target_im, int color=0, flow_vector* _flow_vector_array=NULL, int _zaeler_flow_vector_array=0);// Linen, einfarbig
    void vis_flow4(IplImage* target_im, flow_vector* _flow_vector_array=NULL, int _zaeler_flow_vector_array=0);// Punkte
    void vis_grid( IplImage* target_im );
    void vis_raster( IplImage* target_im );
    void vis_meanflow1(IplImage* target_im);

    void vector_array_2_vector_3image3x();
    void vector_3image_2_vector_array(flow_vector *flow_vector_arr=NULL, int* zaeler_flow_vector_arr=NULL, int* size_flow_vector_arr=NULL,
        IplImage* Ipl_vx=NULL, IplImage* Ipl_vy=NULL, IplImage* Ipl_val=NULL);
    void vector_image3x_2_vector_array(flow_vector *flow_vector_arr=NULL, int* zaeler_flow_vector_arr=NULL,
                                       int* size_flow_vector_arr=NULL, IplImage* m_vx_vy_val_image=NULL);
    //void vector_cvmat_2_vector_image3x();
    //void vector_image3x_2_vector_cvmat();
    void vector_image3x_2_vector_3image();
    void vector_3image_2_vector_image3x();

};


#endif /*__CENSUSFLOWBASE_H__*/


