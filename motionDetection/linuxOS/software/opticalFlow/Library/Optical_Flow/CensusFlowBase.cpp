// CensusFlowBase.h

//#include "stdafx.h"
#include "./CensusFlowBase.h"


CensusFlowBase::CensusFlowBase(int image_height, int image_widthStep, int image_width,
                               int roi_h1, int roi_w1, int roi_h2, int roi_w2 )
{
    //allgemeine parameter
    m_image_height = image_height;
    m_image_widthStep = image_widthStep;
    m_image_width = image_width;
    m_roi_h1 = roi_h1;    m_roi_w1 = roi_w1;
    m_roi_h2 = roi_h2;    m_roi_w2 = roi_w2;
    //std::cout<<"Konstructor : Base"<<std::endl;
    //fuer Postprocessing
    m_window_width_x = WINDOW_WIDTH_X;
    m_window_heigth_y = WINDOW_HEIGTH_Y;
    int dummy1=0;
    ComputeRaster(m_roi_w1, m_roi_w2, m_window_width_x,  m_start_wx, m_no_windows_wx, dummy1);
    ComputeRaster(m_roi_h1, m_roi_h2, m_window_heigth_y, m_start_hy, m_no_windows_hy, dummy1);
    
    m_filtered_image = cvCreateImage( cvSize(m_image_width,m_image_height),IPL_DEPTH_8U,1);
    //   Visualize
    zaeler_flow_vector_array = 0;
    size_flow_vector_array = LENGTH_FLOWVEKTOR;
    flow_vector_array = new flow_vector[LENGTH_FLOWVEKTOR];

    m_vx_vy_val_image = cvCreateImage( cvSize(m_image_width,m_image_height),IPL_DEPTH_8U,3);
    m_vx_image = cvCreateImage( cvSize(m_image_width,m_image_height),IPL_DEPTH_8U,1); 
    m_vy_image = cvCreateImage( cvSize(m_image_width,m_image_height),IPL_DEPTH_8U,1);
    m_val_image= cvCreateImage( cvSize(m_image_width,m_image_height),IPL_DEPTH_8U,1);
    m_vx_vy_val_mean_image = cvCreateImage( cvSize(m_no_windows_wx,m_no_windows_hy),IPL_DEPTH_8U,3);
    Initialize();
}

CensusFlowBase::~CensusFlowBase(void)
{
    //std::cout<<"Destructor : Base"<<std::endl;
    cvReleaseImage(&m_filtered_image);
    cvReleaseImage(&m_vx_vy_val_image); 
    cvReleaseImage(&m_vx_image); cvReleaseImage(&m_vy_image); cvReleaseImage(&m_val_image);
    cvReleaseImage(&m_vx_vy_val_mean_image);
    delete [] flow_vector_array;
}


// Visuelle Aufbereitung der Flußvektoren mit OpenCv: Erzeugung der Regenbogen-Farben, rainbow color
void CensusFlowBase::Initialize(void) {
    CvScalar line_color1,line_color; 
    IplImage* tmpim = cvCreateImage( cvSize(3,3), IPL_DEPTH_8U, 3 );
    IplImage* tmpim2 = cvCreateImage( cvSize(3,3), IPL_DEPTH_8U, 3 );
    int color_no = 1;
    color_LUT.clear();
    for (int ii1=0; ii1<FLOW_VECTOR_EXTREMELENGTH; ii1++) {
        line_color1 = CV_RGB(255,255, color_no);
        cvSet2D( tmpim, 2, 2, line_color1 );
        cvCvtColor(tmpim, tmpim2, CV_HSV2BGR );
        line_color = cvGet2D( tmpim2, 2, 2 );
        color_LUT.push_back(line_color);
        color_no += 5;//15; bei postprocess
    }
    cvReleaseImage(&tmpim);  cvReleaseImage(&tmpim2);

}

void CensusFlowBase::Init_Cycle( void )
{
    zaeler_flow_vector_array = 0;
    cvSetZero(m_vx_vy_val_image);
    cvSet(m_vx_image,cvScalar(0));
    cvSet(m_vy_image,cvScalar(0)); 
    cvSetZero(m_val_image); 
    cvSetZero(m_vx_vy_val_mean_image);
}

IplImage* CensusFlowBase::SmoothImage1(IplImage* pocess_img, int sizeblur)
{
    cvSmooth( pocess_img, m_filtered_image, CV_BLUR, sizeblur, sizeblur);
    return m_filtered_image;
}

IplImage* CensusFlowBase::SmoothImage2(IplImage* pocess_img)
{
    for (int jjh=1; jjh<pocess_img->height-1; jjh++) {
        for (int jjw=1; jjw<pocess_img->width-1; jjw++) {
            int zz = pocess_img->width*jjh + jjw;
            m_filtered_image->imageData[zz] =( (unsigned char)pocess_img->imageData[zz-1-pocess_img->width] + 2*(unsigned char)(pocess_img->imageData[zz-pocess_img->width]) +   (unsigned char)pocess_img->imageData[zz+1-pocess_img->width] + 
                                             2*(unsigned char)pocess_img->imageData[zz-1]                   + 4*(unsigned char)pocess_img->imageData[zz]                     + 2*(unsigned char)pocess_img->imageData[zz+1] + 
                                               (unsigned char)pocess_img->imageData[zz-1+pocess_img->width] + 2*(unsigned char)(pocess_img->imageData[zz+pocess_img->width]) +   (unsigned char)pocess_img->imageData[zz+1+pocess_img->width] )/16; 
        }
    }
    return m_filtered_image;
}

void CensusFlowBase::ComputeRaster(int roi_wh1, int roi_wh2, int window_wh, int& start_wh, int& no_windows, int& end_wh) {
    int rem_wh;
    int roi_diff;

    roi_diff = roi_wh2-roi_wh1;
    rem_wh = roi_diff%window_wh;  
    no_windows  = roi_diff/window_wh;
    start_wh = rem_wh/2;
    end_wh = start_wh+no_windows*window_wh;
}

int CensusFlowBase::PostProcess_fast(int max_diff_wx, int max_diff_hy)
{

    int end_point_wx, end_point_hy;
    end_point_wx = m_start_wx+m_no_windows_wx*m_window_width_x;
    end_point_hy = m_start_hy+m_no_windows_hy*m_window_heigth_y;

    int accum_wx, accum_hy, ctr, mean_wx, mean_hy, cur_wx, cur_hy;
    int accum_wx2, accum_hy2, ctr2;
    int kk=0;
    int diff_hy, diff_wx, ctr_remove=0;
    int ctr_window_wx=0, ctr_window_hy=0;
    for (int cur_start_wx=m_start_wx; cur_start_wx<end_point_wx; cur_start_wx+=m_window_width_x,ctr_window_wx++) {
        ctr_window_hy=0;
        for(int cur_start_hy=m_start_hy; cur_start_hy<end_point_hy; cur_start_hy+=m_window_heigth_y,ctr_window_hy++) {
            accum_wx = 0;
            accum_hy = 0;
            ctr = 0;
            for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                    //if( 0!=m_val_image->imageData[kkwx+kkhy *m_val_image->widthStep] ){
                    if( 0!=m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+2] ){
                        ctr++;
                        //accum_wx += (((uchar)m_vx_image->imageData[kkwx + m_vx_image->widthStep * kkhy])-128);
                        //accum_hy += (((uchar)m_vy_image->imageData[kkwx + m_vy_image->widthStep * kkhy])-128);
                        accum_wx += (((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3])-128); 
                        accum_hy += (((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+1])-128);
                    }
                }
            }
            if(ctr>0) {
                if (ctr<3) {
                    for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                        for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                            if( 0!=m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+2] ) {
                                diff_wx = abs(((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3])-128) +
                                    abs(((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+1])-128);                
                                if(diff_wx > LENGTH_LONGVEC) {
                                    //m_val_image->imageData[kkwx+kkhy*(m_val_image->widthStep)]=0;
                                    m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+2] = ((uchar) 0);
                                    ctr_remove++;
                                    ctr--;
                                }
                            }
                        }
                    }    
                } 
                else {
                    accum_wx2 = accum_wx;
                    accum_hy2 = accum_hy;
                    ctr2 = ctr; 
                    for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                        for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                            if( 0!=((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+2]) ) {
                                //cur_wx = (((uchar)m_vx_image->imageData[kkwx + m_vx_image->widthStep * kkhy])-128); 
                                //cur_hy = (((uchar)m_vy_image->imageData[kkwx + m_vy_image->widthStep * kkhy])-128);
                                cur_wx = (((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3])-128); 
                                cur_hy = (((uchar)m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+1])-128);
                                diff_wx = abs(cur_wx - (accum_wx2-cur_wx)/(ctr2-1) );
                                diff_hy = abs(cur_hy - (accum_hy2-cur_hy)/(ctr2-1) );
                                if( diff_wx > max_diff_wx ||  diff_hy > max_diff_hy ) {
                                    //m_val_image->imageData[kkwx+kkhy*(m_val_image->widthStep)]=0;
                                    //m_vx_image->imageData[kkwx+kkhy*(m_vx_image->widthStep)]=0;
                                    //m_vy_image->imageData[kkwx+kkhy*(m_vy_image->widthStep)]=0;
                                    m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3] = ((uchar) 0);
                                    m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+1]=((uchar) 0);
                                    m_vx_vy_val_image->imageData[kkhy*m_vx_vy_val_image->widthStep+kkwx*3+2]=((uchar) 0);
                                    ctr_remove++;
                                    accum_wx-=cur_wx;
                                    accum_hy-=cur_hy;
                                    ctr--;
                                }
                            }
                        }
                    }
                    if(ctr>0){
                        mean_wx = accum_wx/ctr;
                        mean_hy = accum_hy/ctr;  
                    } else {
                        mean_wx = 0;
                        mean_hy = 0;
                    }
                    m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy]   = (uchar)( mean_wx+128);
                    m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy+1] = (uchar)( mean_hy+128);
                    m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy+2] = (uchar)( ctr);
                    //m_abs_angle_val_mean_image->imageData[3*ctr_window_wx + m_abs_angle_val_mean_image->widthStep * ctr_window_hy]  
                    //= ((char) 10*mean_wx+128);
                    //m_abs_angle_val_mean_image->imageData[3*ctr_window_wx + m_abs_angle_val_mean_image->widthStep * ctr_window_hy+1] 
                    //= ((char) 10*mean_hy/ctr+128);
                    //m_abs_angle_val_mean_image->imageData[3*ctr_window_wx + m_abs_angle_val_mean_image->widthStep * ctr_window_hy+2] 
                    //= ((char) 0);
                }
            }
        }
    }
    vector_image3x_2_vector_array();
    //std::cout << "-> ctr_remove " << ctr_remove << std::endl;
    return ctr_remove;
}


// Postprocessing: Aufsummierung von x und y und Bildung des Mittelwertes innerhalb des definierten Fensters
int CensusFlowBase::PostProcess_HW(int w_search_postprocessing, int h_search_postprocessing, int max_diff_wx, int max_diff_hy )
{
    int width, height, min_w, max_w, min_h, max_h, z, w_search, h_search;
    int accum_wx, accum_hy, ctr, cur_wx, cur_hy, diff_wx, diff_hy, ctr_remove=0;
    width = m_vx_vy_val_image->width;  height = m_vx_vy_val_image->height; 
    IplImage* m_vx_vy_val_image_orig = cvCloneImage(m_vx_vy_val_image);
    w_search = w_search_postprocessing;
    h_search = h_search_postprocessing;
    min_w = w_search;
    max_w = width - (w_search+1);
    min_h = h_search;
    max_h = height - (h_search+1);
    z=-1;  // Index Zentrales Pixel
    for (int iih=min_h; iih<=max_h; iih++) {
        for(int iiw=min_w; iiw<=max_w; iiw++) {
            z = iih*m_vx_vy_val_image_orig->widthStep + iiw*3;
            accum_wx = 0;
            accum_hy = 0;
            ctr = 0;
            // wenn zentrales Pixel ungültig ist, wird das nächste darauffolgende zentrale Pixel betrachtet
            if( 0==m_vx_vy_val_image_orig->imageData[z+2] )
                continue;
            cur_wx = (((uchar)m_vx_vy_val_image_orig->imageData[z])-128); 
            cur_hy = (((uchar)m_vx_vy_val_image_orig->imageData[z+1])-128);
            for (int kkwx=-h_search; kkwx<h_search; kkwx++) {
                for (int kkhy= -w_search; kkhy<w_search; kkhy++) {
                    //if( 0!=m_val_image->imageData[kkwx+kkhy *m_val_image->widthStep] ){
                    // Aufsummierung der gültigen Pixel innerhalb des definieten Bereichs
                    if( 0!=m_vx_vy_val_image_orig->imageData[z+kkhy*m_vx_vy_val_image->widthStep+kkwx*3+2] ){
                        ctr++;
                        //accum_wx += (((uchar)m_vx_image->imageData[kkwx + m_vx_image->widthStep * kkhy])-128);
                        //accum_hy += (((uchar)m_vy_image->imageData[kkwx + m_vy_image->widthStep * kkhy])-128);
                        accum_wx += (((uchar)m_vx_vy_val_image_orig->imageData[z+kkhy*m_vx_vy_val_image->widthStep+kkwx*3])-128); 
                        accum_hy += (((uchar)m_vx_vy_val_image_orig->imageData[z+kkhy*m_vx_vy_val_image->widthStep+kkwx*3+1])-128);
                    }
                }
            }
            if(ctr>1) { //Differenz größer als Schwelle, ja Bewegungsvektor löschen
                diff_wx = abs(cur_wx - (accum_wx-cur_wx)/(ctr-1) );
                diff_hy = abs(cur_hy - (accum_hy-cur_hy)/(ctr-1) );
                if( diff_wx > max_diff_wx ||  diff_hy > max_diff_hy ) {
                    m_vx_vy_val_image->imageData[z] = ((uchar) 0);
                    m_vx_vy_val_image->imageData[z+1]=((uchar) 0);
                    m_vx_vy_val_image->imageData[z+2]=((uchar) 0);
					ctr_remove++;
                }
            } else {// Bewegungsvektor tritt nur einmal auf -> löschen
                    m_vx_vy_val_image->imageData[z] = ((uchar) 0);
                    m_vx_vy_val_image->imageData[z+1]=((uchar) 0);
                    m_vx_vy_val_image->imageData[z+2]=((uchar) 0);
					ctr_remove++;
            }
        }
    }
    cvReleaseImage(&m_vx_vy_val_image_orig);
    vector_image3x_2_vector_array();
	return ctr_remove;
}

void CensusFlowBase::PostProcess( IplImage* _vx_vy_val_image, flow_vector *flow_vector_arr/*=NULL*/, int* zaeler_flow_vector_arr/*=NULL*/, int* size_flow_vector_arr/*=NULL*/ )
{
    if (NULL==_vx_vy_val_image) {
        _vx_vy_val_image = m_vx_vy_val_image;
    }

    if (NULL==size_flow_vector_arr) size_flow_vector_arr = &size_flow_vector_array;
    if (NULL==zaeler_flow_vector_arr) zaeler_flow_vector_arr = &zaeler_flow_vector_array;
    if (NULL==flow_vector_arr) flow_vector_arr = flow_vector_array;


    int max_diff_wx = MAX_DIFF_WX;
    int max_diff_hy = MAX_DIFF_HY;
    int max_diff_rough_wx = MAX_DIFF_ROUGH_WX;
    int max_diff_rough_hy = MAX_DIFF_ROUGH_HY;

    int end_point_wx, end_point_hy;
    end_point_wx = m_start_wx+m_no_windows_wx*m_window_width_x;
    end_point_hy = m_start_hy+m_no_windows_hy*m_window_heigth_y;

    int accum_wx, accum_hy, ctr, mean_wx, mean_hy, cur_wx, cur_hy;
    int accum_wx2, accum_hy2, ctr2;
    int kk=0;
    int diff_hy, diff_wx, ctr_remove=0;
    int ctr_window_wx=0, ctr_window_hy=0;
    for (int cur_start_wx=m_start_wx; cur_start_wx<end_point_wx; cur_start_wx+=m_window_width_x,ctr_window_wx++) {
        ctr_window_hy=0;
        for(int cur_start_hy=m_start_hy; cur_start_hy<end_point_hy; cur_start_hy+=m_window_heigth_y,ctr_window_hy++) {
            accum_wx = 0;
            accum_hy = 0;
            ctr = 0;
            for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                    //if( 0!=m_val_image->imageData[kkwx+kkhy *m_val_image->widthStep] ){
                    if( 0!=_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2] ){
                        ctr++;
                        //accum_wx += (((uchar)m_vx_image->imageData[kkwx + m_vx_image->widthStep * kkhy])-128);
                        //accum_hy += (((uchar)m_vy_image->imageData[kkwx + m_vy_image->widthStep * kkhy])-128);
                        accum_wx += (((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3])-128); 
                        accum_hy += (((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1])-128);
                    }
                }
            }
            // Grobes ausfiltern
            if (ctr<3) {
                for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                    for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                        if( 0!=_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2] ) {
                            diff_wx = abs(((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3])-128) +
                                abs(((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1])-128);                
                            if(diff_wx > LENGTH_LONGVEC) {
                                //m_val_image->imageData[kkwx+kkhy*(m_val_image->widthStep)]=0;
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2] = ((uchar) 0);
                                ctr_remove++;
                                ctr--;
                            }
                        }
                    }
                }    
            } 
            else {
                accum_wx2 = accum_wx;
                accum_hy2 = accum_hy;
                ctr2 = ctr; 
                for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                    for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                        if( 0!=((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2]) ) {
                            //cur_wx = (((uchar)m_vx_image->imageData[kkwx + m_vx_image->widthStep * kkhy])-128); 
                            //cur_hy = (((uchar)m_vy_image->imageData[kkwx + m_vy_image->widthStep * kkhy])-128);
                            cur_wx = (((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3])-128); 
                            cur_hy = (((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1])-128);
                            diff_wx = abs(cur_wx - (accum_wx2-cur_wx)/(ctr2-1) );
                            diff_hy = abs(cur_hy - (accum_hy2-cur_hy)/(ctr2-1) );
                            if( diff_wx > max_diff_rough_wx ||  diff_hy > max_diff_rough_hy ) {
                                //m_val_image->imageData[kkwx+kkhy*(m_val_image->widthStep)]=0;
                                //m_vx_image->imageData[kkwx+kkhy*(m_vx_image->widthStep)]=0;
                                //m_vy_image->imageData[kkwx+kkhy*(m_vy_image->widthStep)]=0;
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3] = ((uchar) 0);
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1]=((uchar) 0);
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2]=((uchar) 0);
                                ctr_remove++;
                                accum_wx-=cur_wx;
                                accum_hy-=cur_hy;
                                ctr--;
                            }
                        }
                    }
                }
            }
            // Feines ausfiltern
            if (ctr<2) {
                for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                    for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                        if( 0!=_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2] ) {
                            diff_wx = abs(((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3])-128) +
                                abs(((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1])-128);                
                            if(diff_wx > LENGTH_LONGVEC) {
                                //m_val_image->imageData[kkwx+kkhy*(m_val_image->widthStep)]=0;
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2] = ((uchar) 0);
                                ctr_remove++;
                                ctr--;
                            }
                        }
                    }
                }    
            } 
            else {
                accum_wx2 = accum_wx;
                accum_hy2 = accum_hy;
                ctr2 = ctr; 
                for (int kkwx=cur_start_wx; kkwx<cur_start_wx+m_window_width_x; kkwx++) {
                    for (int kkhy=cur_start_hy; kkhy<cur_start_hy+m_window_heigth_y; kkhy++) {
                        if( 0!=((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2]) ) {
                            //cur_wx = (((uchar)m_vx_image->imageData[kkwx + m_vx_image->widthStep * kkhy])-128); 
                            //cur_hy = (((uchar)m_vy_image->imageData[kkwx + m_vy_image->widthStep * kkhy])-128);
                            cur_wx = (((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3])-128); 
                            cur_hy = (((uchar)_vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1])-128);
                            diff_wx = abs(cur_wx - (accum_wx2-cur_wx)/(ctr2-1) );
                            diff_hy = abs(cur_hy - (accum_hy2-cur_hy)/(ctr2-1) );
                            if( diff_wx > max_diff_wx ||  diff_hy > max_diff_hy ) {
                                //m_val_image->imageData[kkwx+kkhy*(m_val_image->widthStep)]=0;
                                //m_vx_image->imageData[kkwx+kkhy*(m_vx_image->widthStep)]=0;
                                //m_vy_image->imageData[kkwx+kkhy*(m_vy_image->widthStep)]=0;
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3] = ((uchar) 0);
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+1]=((uchar) 0);
                                _vx_vy_val_image->imageData[kkhy*_vx_vy_val_image->widthStep+kkwx*3+2]=((uchar) 0);
                                ctr_remove++;
                                accum_wx-=cur_wx;
                                accum_hy-=cur_hy;
                                ctr--;
                            }
                        }
                    }
                }
            }
            if(ctr>0){
                mean_wx = accum_wx/ctr;
                mean_hy = accum_hy/ctr;  
            } else {
                mean_wx = 0;
                mean_hy = 0;
            }
            m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy]   = (uchar)( mean_wx+128);
            m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy+1] = (uchar)( mean_hy+128);
            m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy+2] = (uchar)( ctr);
            //m_abs_angle_val_mean_image->imageData[3*ctr_window_wx + m_abs_angle_val_mean_image->widthStep * ctr_window_hy]  
            //= ((char) 10*mean_wx+128);
            //m_abs_angle_val_mean_image->imageData[3*ctr_window_wx + m_abs_angle_val_mean_image->widthStep * ctr_window_hy+1] 
            //= ((char) 10*mean_hy/ctr+128);
            //m_abs_angle_val_mean_image->imageData[3*ctr_window_wx + m_abs_angle_val_mean_image->widthStep * ctr_window_hy+2] 
            //= ((char) 0);
        }
    }
    vector_image3x_2_vector_array(flow_vector_arr, zaeler_flow_vector_arr, size_flow_vector_arr, _vx_vy_val_image );
    //std::cout << "-> ctr_remove " << ctr_remove << std::endl;
}


// Zeichnen der Flußvektoren unter Berüecksichtigung der Länge der Flußvekoren.
void CensusFlowBase::vis_flow1(IplImage* target_im)
{
    const double pi = 3.14159265358979323846;
    CvPoint p,q;
    int line_thickness = 1;
    int lengthening_factor = 1;
    double angle; 
    double hypotenuse; 
    CvScalar line_color;
    for(int i = 0; i < zaeler_flow_vector_array; i++) {
        // Letzter Punkt
        q.x = flow_vector_array[i].x1;
        q.y = flow_vector_array[i].y1;
        // Aktueller Punkt
        p.x = flow_vector_array[i].x2;
        p.y = flow_vector_array[i].y2;
        angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
        hypotenuse = sqrt( (double)( (p.y - q.y)*(p.y - q.y)+(p.x - q.x)*(p.x - q.x) ) );
        q.x = (int) (p.x - lengthening_factor * hypotenuse * cos(angle));
        q.y = (int) (p.y - lengthening_factor * hypotenuse * sin(angle));

        if( ((unsigned)hypotenuse+1) < color_LUT.size() ) {
            line_color = color_LUT.at( ((int)hypotenuse) );
        } else {
            line_color = color_LUT.at( color_LUT.size()-1) ;
        }
        cvLine( target_im, p, q, line_color, line_thickness, CV_AA, 0 );
        if( hypotenuse>2.0) {
            q.x = (int) (p.x + hypotenuse*0.2*cos(angle + 7*pi/8 ));
            q.y = (int) (p.y + hypotenuse*0.2*sin(angle + 7*pi/8));
            cvLine( target_im, p, q, line_color, line_thickness, CV_AA, 0 );
            q.x = (int) (p.x + hypotenuse*0.2*cos(angle - 7*pi/8));
            q.y = (int) (p.y + hypotenuse*0.2*sin(angle - 7*pi/8));
            cvLine( target_im, p, q, line_color, line_thickness, CV_AA, 0 );
        }
    }
}

// Zeichnen der Flußvektoren unter Berüecksichtigung der Länge der Flußvekoren.
void CensusFlowBase::vis_flow2(IplImage* target_im, flow_vector* _flow_vector_array, int _zaeler_flow_vector_array)
{
    if (NULL==_flow_vector_array) {
        _flow_vector_array = flow_vector_array;
        _zaeler_flow_vector_array = zaeler_flow_vector_array;
    }
    CvPoint p,q;
    int line_thickness = 1; //int line_thickness = 3;
    double hypotenuse; 
    CvScalar line_color;
    for(int i = 0; i < _zaeler_flow_vector_array; i++) {
        // Letzter Punkt
        q.x = _flow_vector_array[i].x1;  q.y = _flow_vector_array[i].y1;
        // Aktueller Punkt
        p.x = _flow_vector_array[i].x2;  p.y = _flow_vector_array[i].y2;
        
		// Determine in which color the vector has to be draw according to its lenght
        hypotenuse = sqrt( (double)( (p.y - q.y)*(p.y - q.y)+(p.x - q.x)*(p.x - q.x) ) );
        if( ((unsigned)hypotenuse+1) < color_LUT.size() ) {
            line_color = color_LUT.at( ((int)hypotenuse) );
        } else {
            line_color = color_LUT.at( color_LUT.size()-1 ) ;
        }
        cvLine( target_im, p, q, line_color, line_thickness, 0, 0 );
    }
}

// Zeichnen der Flußvektoren unter Berüecksichtigung der Länge der Flußvekoren.
void CensusFlowBase::vis_flow3(IplImage* target_im, int color, flow_vector* _flow_vector_array, int _zaeler_flow_vector_array)
{
    if (NULL==_flow_vector_array) {
        _flow_vector_array = flow_vector_array;
        _zaeler_flow_vector_array = zaeler_flow_vector_array;
    }
    CvPoint p,q;
    int line_thickness = 1; //int line_thickness = 3;
    double hypotenuse; 
    CvScalar line_color;
    for(int i = 0; i < _zaeler_flow_vector_array; i++) {
        // Letzter Punkt
        q.x = _flow_vector_array[i].x1;  q.y = _flow_vector_array[i].y1;
        // Aktueller Punkt
        p.x = _flow_vector_array[i].x2;  p.y = _flow_vector_array[i].y2;

        line_color = color_LUT.at( color ) ;
        cvLine( target_im, p, q, line_color, line_thickness, 0, 0 );
    }
}

// Zeichnen der Flußvektoren unter Berüecksichtigung der Länge der Flußvekoren.
void CensusFlowBase::vis_flow4(IplImage* target_im, flow_vector* _flow_vector_array, int _zaeler_flow_vector_array)
{
    if (NULL==_flow_vector_array) {
        _flow_vector_array = flow_vector_array;
        _zaeler_flow_vector_array = zaeler_flow_vector_array;
        }
    CvPoint p,q;
    //int line_thickness = 1; 
    int line_thickness = 3;
    double hypotenuse; 
    CvScalar line_color;
    for(int i = 0; i < _zaeler_flow_vector_array; i++) {
        // Letzter Punkt
        q.x = _flow_vector_array[i].x1;  q.y = _flow_vector_array[i].y1;
        // Aktueller Punkt
        p.x = _flow_vector_array[i].x2;  p.y = _flow_vector_array[i].y2;
        hypotenuse = sqrt( (double)( (p.y - q.y)*(p.y - q.y)+(p.x - q.x)*(p.x - q.x) ) );
        if( ((unsigned)hypotenuse+1) < color_LUT.size() ) {
            line_color = color_LUT.at( ((int)hypotenuse) );
            } else {
                line_color = color_LUT.at( color_LUT.size()-1 ) ;
            }
        cvLine( target_im, p, p, line_color, line_thickness, 0, 0 );
    }
}

void CensusFlowBase::vis_raster( IplImage* target_im )
{
    CvPoint wp;
    int end_point_wx, end_point_hy;
    end_point_wx = m_start_wx+m_no_windows_wx*m_window_width_x;
    end_point_hy = m_start_hy+m_no_windows_hy*m_window_heigth_y;
    for (int cur_start_wx=m_start_wx; cur_start_wx<=end_point_wx; cur_start_wx+=4) {
        for(int cur_start_hy=m_start_hy; cur_start_hy<=end_point_hy; cur_start_hy+=4) {
            wp.x = cur_start_wx;
            wp.y = cur_start_hy;
            cvLine(target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
        }
    }
}

void CensusFlowBase::vis_grid(IplImage* target_im)
{
    CvPoint wp;
    int end_point_wx, end_point_hy;
    end_point_wx = m_start_wx+m_no_windows_wx*m_window_width_x;
    end_point_hy = m_start_hy+m_no_windows_hy*m_window_heigth_y;
    for (int cur_start_wx=m_start_wx; cur_start_wx<=end_point_wx; cur_start_wx+=m_window_width_x) {
        for(int cur_start_hy=m_start_hy; cur_start_hy<=end_point_hy; cur_start_hy+=m_window_heigth_y) {
            wp.x = cur_start_wx;
            wp.y = cur_start_hy;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
            wp.x = cur_start_wx+ m_window_width_x/4;
            wp.y = cur_start_hy;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
            wp.x = cur_start_wx+ m_window_width_x/2;
            wp.y = cur_start_hy;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
            wp.x = cur_start_wx+ 3*m_window_width_x/4;
            wp.y = cur_start_hy;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
            wp.x = cur_start_wx;
            wp.y = cur_start_hy+ m_window_heigth_y/4;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
            wp.x = cur_start_wx;
            wp.y = cur_start_hy+ m_window_heigth_y/2;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
            wp.x = cur_start_wx;
            wp.y = cur_start_hy+ 3*m_window_heigth_y/4;
            cvLine( target_im,wp,wp, cvScalar(255,255,255), 1, 0, 0 );
        }
    }
}


void CensusFlowBase::vis_meanflow1( IplImage* target_im )
{
    vis_grid(target_im);
    int end_point_wx, end_point_hy;
    end_point_wx = m_start_wx+m_no_windows_wx*m_window_width_x;
    end_point_hy = m_start_hy+m_no_windows_hy*m_window_heigth_y;
    CvPoint p,q;
    int line_thickness = 3;
    int ctr_window_wx=0, ctr_window_hy=0;
    int mean_wx,mean_hy,ctr;
    CvScalar line_color;

    ctr_window_wx=0;
    for (int cur_start_wx=m_start_wx+m_window_width_x/2; cur_start_wx<end_point_wx; cur_start_wx+=m_window_width_x,ctr_window_wx++) {
        ctr_window_hy=0;
        for(int cur_start_hy=m_start_hy+m_window_heigth_y/2; cur_start_hy<end_point_hy; cur_start_hy+=m_window_heigth_y,ctr_window_hy++) {
            // Letzter Punkt
            mean_wx = (uchar)m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy]  -128;
            mean_hy = (uchar)m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy+1]-128;
            ctr     = (uchar)m_vx_vy_val_mean_image->imageData[3*ctr_window_wx + m_vx_vy_val_mean_image->widthStep * ctr_window_hy+2];
            p.x = cur_start_wx;
            p.y = cur_start_hy;            
            if (0==ctr){
                cvLine( target_im,p,p, cvScalar(0,0,0),3, 0, 0 );
                continue;
            }
            q.x = cur_start_wx - mean_wx;
            q.y = cur_start_hy - mean_hy;
            // Aktueller Punkt
            line_thickness = (int)(log10((double)ctr)*5);
            int hypotenuse;
            hypotenuse = (int)sqrt( (double)( (p.y - q.y)*(p.y - q.y)+(p.x - q.x)*(p.x - q.x) ) );
            //if (-1==hypotenuse)
            //    hypotenuse=0;
            if( ((unsigned)hypotenuse+1) < color_LUT.size() ) {
                line_color = color_LUT.at( ((int)hypotenuse) );
            } else {
                line_color = color_LUT.at( color_LUT.size()-1 ) ;
            }
            cvLine( target_im, p, q, line_color, line_thickness, 0, 0 );
        }
    }
}


void CensusFlowBase::vector_array_2_vector_3image3x() {
    int x1,x2,y1,y2;
    cvSet(m_vx_image,cvScalar(0)); cvSet(m_vy_image,cvScalar(0)); cvSetZero(m_val_image); 
    for(int kk=0;kk<zaeler_flow_vector_array;kk++) {
        x1 =   flow_vector_array[kk].x1; 
        y1 =   flow_vector_array[kk].y1;
        x2 =   flow_vector_array[kk].x2;
        y2 =   flow_vector_array[kk].y2;
        m_vx_image->imageData[y2*m_vx_image->widthStep+x2] = ((uchar) (x2-x1)+128);
        m_vy_image->imageData[y2*m_vy_image->widthStep+x2] = ((uchar) (y2-y1)+128);
        m_val_image->imageData[y2*m_val_image->widthStep+x2] = ((uchar) 255);
        m_vx_vy_val_image->imageData[y2*m_vx_vy_val_image->widthStep+x2*3] = ((uchar) ((x2-x1)+128));
        m_vx_vy_val_image->imageData[y2*m_vx_vy_val_image->widthStep+x2*3+1]=((uchar) ((y2-y1)+128));
        m_vx_vy_val_image->imageData[y2*m_vx_vy_val_image->widthStep+x2*3+2]=((uchar) 255);
    }
}

void CensusFlowBase::vector_3image_2_vector_array(flow_vector *flow_vector_arr, int* zaeler_flow_vector_arr, int* size_flow_vector_arr,
                                                 IplImage* Ipl_vx, IplImage* Ipl_vy, IplImage* Ipl_val) 
{
    int image_width, image_height;
    int vx,vy;
    if (NULL==Ipl_val) Ipl_val = m_val_image;
    if (NULL==Ipl_vy) Ipl_vy = m_vy_image;
    if (NULL==Ipl_vx) Ipl_vx = m_vx_image;
    if (NULL==size_flow_vector_arr) size_flow_vector_arr = &size_flow_vector_array;
    if (NULL==zaeler_flow_vector_arr) zaeler_flow_vector_arr = &zaeler_flow_vector_array;
    if (NULL==flow_vector_arr) flow_vector_arr = flow_vector_array;

    image_width = Ipl_vx->width;
    image_height =  Ipl_vx->height;
    *zaeler_flow_vector_arr=0;
    for(int kkwx=0; kkwx<image_width; kkwx++) {
        for (int kkhy=0; kkhy<image_height; kkhy++) {
            if(0!=Ipl_val->imageData[kkwx+kkhy*(Ipl_val->widthStep)] && (*zaeler_flow_vector_arr) < (*size_flow_vector_arr) ) {
                vx= ((uchar)Ipl_vx->imageData[kkwx + Ipl_vx->widthStep * kkhy])-128;
                vy= ((uchar)Ipl_vy->imageData[kkwx + Ipl_vy->widthStep * kkhy])-128;
                flow_vector_arr[*zaeler_flow_vector_arr].x1 = kkwx - vx;
                flow_vector_arr[*zaeler_flow_vector_arr].y1 = kkhy - vy;
                flow_vector_arr[*zaeler_flow_vector_arr].x2 = kkwx;
                flow_vector_arr[*zaeler_flow_vector_arr].y2 = kkhy;
                (*zaeler_flow_vector_arr)++;
            }
        }
    }
}

void CensusFlowBase::vector_image3x_2_vector_array( flow_vector *flow_vector_arr/*=NULL*/, int* zaeler_flow_vector_arr/*=NULL*/, int* size_flow_vector_arr/*=NULL*/, IplImage* vx_vy_val_image/*=NULL*/ )
{
    int image_width, image_height;
    int vx,vy,val;
    if (NULL==vx_vy_val_image) vx_vy_val_image = m_vx_vy_val_image;
    if (NULL==size_flow_vector_arr) size_flow_vector_arr = &size_flow_vector_array;
    if (NULL==zaeler_flow_vector_arr) zaeler_flow_vector_arr = &zaeler_flow_vector_array;
    if (NULL==flow_vector_arr) flow_vector_arr = flow_vector_array;
    (*zaeler_flow_vector_arr)=0;

    image_width = m_vx_vy_val_image->width;
    image_height =  m_vx_vy_val_image->height;
    *zaeler_flow_vector_arr=0;
    for(int kkwx=0; kkwx<image_width; kkwx++) {
        for (int kkhy=0; kkhy<image_height; kkhy++) {
            val = ((uchar)vx_vy_val_image->imageData[kkwx*3+kkhy*(m_vx_vy_val_image->widthStep)+2]);
            if(0!=val && (*zaeler_flow_vector_arr) < (*size_flow_vector_arr) ) {
                vx= ((uchar)vx_vy_val_image->imageData[kkwx*3+kkhy*(m_vx_vy_val_image->widthStep)])-128;
                vy= ((uchar)vx_vy_val_image->imageData[kkwx*3+kkhy*(m_vx_vy_val_image->widthStep)+1])-128;
                flow_vector_arr[*zaeler_flow_vector_arr].x1 = kkwx - vx;
                flow_vector_arr[*zaeler_flow_vector_arr].y1 = kkhy - vy;
                flow_vector_arr[*zaeler_flow_vector_arr].x2 = kkwx;
                flow_vector_arr[*zaeler_flow_vector_arr].y2 = kkhy;
                (*zaeler_flow_vector_arr)++;
            }
        }
    }
}


void CensusFlowBase::vector_image3x_2_vector_3image()
{
    cvSplit(m_vx_vy_val_image, m_vx_image, m_vy_image, m_val_image, NULL);
}

void CensusFlowBase::vector_3image_2_vector_image3x()
{
    cvMerge(m_vx_image, m_vy_image, m_val_image, NULL, m_vx_vy_val_image);
}

