// CensusFlow_vs2.h


#ifndef __CENSUSFLOW_VS2_H__
#define __CENSUSFLOW_VS2_H__

// STL:
#include <vector>
#include <deque>
#include <iostream>
#include <string>
//OpenCV Includes
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
// Eigene:
#include "../global_defines_UOC.h"
#include "./storage64b.h"
#include "./CensusFlowBase.h"

#define SIZE_ARRAY 43046723 //=43046721+2     /////// = 16777216;
#define SIZE_VERWALTUNG 500000
//#define LENGTH_FLOWVEKTOR 100000
#define WD 10
#define ND 5


class CensusFlower2 : public CensusFlowBase
{
public:
    CensusFlower2(int epsilon, int image_height, int image_widthStep, int image_width,
                 int roi_h1=0, int roi_w1=0, int roi_h2=VRMAGIC_HEIGHT, int roi_w2=VRMAGIC_WIDTH);
    ~CensusFlower2(void);
//private:
    int m_epsilon;
    int m_slot_no;
    int size_zeiger_array;
    storage64b** zeiger_array;
    int size_storage64b_verwaltung, zaeler_storage64b_verwaltung;//=p_ende_zeiger_verwaltung
    storage64b** storage64b_verwaltung;
	//std::vector<CvScalar> colorLUT;

public:
    //void Initialize(void);
    void Init_Cycle(int counter);
    //void vis_flow1(IplImage* target_im);
    //void vis_flow2(IplImage* target_im);
	void AggregateSignatures(IplImage* image1);
    void AggregateSignatures2(IplImage* image1,IplImage* image2);// modified by Jian 29.06.2016
    int  MatchSignatures(void);
};

#endif /*__CENSUSFLOW_VS2_H__*/

