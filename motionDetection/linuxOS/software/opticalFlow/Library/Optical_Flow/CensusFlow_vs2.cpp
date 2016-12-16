// CensusFlow_vs2.cpp

//#include "stdafx.h"
#include "./CensusFlow_vs2.h"

//Constructor
CensusFlower2::CensusFlower2(int epsilon, int image_height, int image_widthStep, int image_width,
                             int roi_h1, int roi_w1, int roi_h2, int roi_w2) :
                CensusFlowBase(image_height, image_widthStep, image_width,
                                roi_h1, roi_w1, roi_h2, roi_w2)
{
    size_zeiger_array  = SIZE_ARRAY; //=43046721+2     /////// = 16777216;
    size_storage64b_verwaltung = SIZE_VERWALTUNG;
    zaeler_storage64b_verwaltung=0;
    m_epsilon = epsilon;
    m_slot_no = 0;
//Anlegen der Verwaltung 
    storage64b_verwaltung = new storage64b*[size_storage64b_verwaltung];
    for (long ii=0; ii<size_storage64b_verwaltung; ii++) {
        storage64b_verwaltung[ii] = new storage64b();
    }
//Anlegen des Zeiger Arrays
    zeiger_array = new storage64b*[size_zeiger_array];
    for (long ii=0; ii<size_zeiger_array; ii++) {
        zeiger_array[ii]=NULL;
    }
	//colorLUT = Parent::color_LUT;
};

//Destruktor: L�schen der angelegten Arrays und somit Speicherfreigabe
CensusFlower2::~CensusFlower2(void) {
    delete [] zeiger_array;
    for (long ii=0; ii<size_storage64b_verwaltung; ii++)
        delete storage64b_verwaltung[ii];
    delete [] storage64b_verwaltung;
    //delete [] flow_vector_array;
};

// Berechnung der Slotnummer und Z�hler des Flu�vektors auf 0 setzen 
void CensusFlower2::Init_Cycle(int counter) {
    CensusFlowBase::Init_Cycle();
    m_slot_no = (counter+STORAGE_HISTORY)%STORAGE_HISTORY;
};



///////////////////////////////////////////////////////////////////////////////////
// Berechnung der Census-Signatur und Speicherung
void CensusFlower2::AggregateSignatures(IplImage* image1)
{
    // 16c: census operatoren; 8p: Ausdehnung 8 pixel; 1s: Abstand der Betrachteten Pixel: 1 pixel
    int width,height,w,min_w, max_w, min_h, max_h,z;
    int o2l, o2m, o2r, m2l, m2r, u2l, u2m, u2r, o1l, o1m, o1r, m1l, m1r, u1l, u1m, u1r; 
    int test = 0;
    uchar e;
    int slot_no =m_slot_no;
    register uchar c_low, c_high;
    storage64b* p_tmp_storage;
    unsigned long *c;
    register unsigned long res;
    unsigned char* i;
    char* itmp;

    width = image1->width;  height = image1->height;  w = image1->widthStep;
    itmp = image1->imageData;
    i = reinterpret_cast<unsigned char*>(itmp);
    e=(uchar)m_epsilon;
    min_w = m_roi_w1 + WD;  max_w = m_roi_w2 -(WD+1);
    min_h = m_roi_h1 + WD;  max_h = m_roi_h2 -(WD+1);
    z=-1;  // Index Zentrales Pixel
    c = &res;
// Berechnung der Indezies der einzelnen Pixel 
    o2l= -WD*w-WD; o2m=-WD*w; o2r=-WD*w+WD;
    m2l= -WD;                 m2r= WD;
    u2l=  WD*w-WD; u2m= WD*w; u2r= WD*w+WD;
    o1l= -ND*w-ND; o1m=-ND*w; o1r=-ND*w+ND;
    m1l= -ND;                 m1r= ND;
    u1l=  ND*w-ND; u1m= ND*w; u1r= ND*w+ND;
    
    if(STORAGE_HISTORY<=slot_no || slot_no<0)
        return;
// MUNI debug
//    int count = 0;
//    int temp = 0;
//    for (int tt = 0; tt<=5000000; tt++) {
//    	int temp = i[tt];
//    	count++;
//        std::cout << "count " << count << std::endl;
//    } // 557024

    for (int iih=min_h; iih<=max_h; iih++) {
        for (int iiw=min_w; iiw<=max_w; iiw++) {
// MUNI debug
//        	++count;
//        	if (count < 66415 ){
//            std::cout << "count " << count << std::endl;

            z = w*iih + iiw;
// Zentrales Pixel > Nachbarpixel -> 2
            c_low = i[z] - e;
// Zentrales Pixel < Nachbarpixel -> 1
            c_high = i[z] + e;
            // Census Transformation: Berechnung der Signatur
// MUNI debug
//            if (count < 50 || count > 181450) {
//            std::cout << "res: " << res << std::endl;
//            std::cout << "c_high: " << c_high << std::endl;
//            std::cout << "c_low: " << c_low << std::endl;
//            std::cout << "z: " << z << std::endl;
//            std::cout << "u2r: " << u2r << std::endl;
//            std::cout << "i[z]: " << i[z] << std::endl;
//            std::cout << "e: " << e << std::endl;
//            std::cout << "z+u2r " << z+u2r << std::endl;
//            std::cout << "i[z+u2r]: " << i[z+u2r] << std::endl;
//            std::cout << "count: " << count << std::endl;
//            std::cout << "=======" << std::endl;
//            }

//            if (count < 65005) { // JIAN ADDED 07.04
//             std::cout << "z+u2r " << z+u2r << std::endl;

            res=0;
            if(i[z+o2l]>c_high) res +=       1;   else if(i[z+o2l]<c_low) res +=       2;
            if(i[z+u2r]>c_high) res +=14348907;   else if(i[z+u2r]<c_low) res +=28697814;
//            std::cout << res << std::endl;
            if(i[z+o2r]>c_high) res +=       9;   else if(i[z+o2r]<c_low) res +=      18;
            if(i[z+u2l]>c_high) res += 1594323;   else if(i[z+u2l]<c_low) res += 3188646;
            if (0==res)        // Es wird �berpr�ft, ob die �u�eren Pixel mit dem Zentralen identisch sind, wenn ja dann wird die Census Transformation 
                continue;   // nicht mehr weiter geschrieben, sondern f�r das n�chste zentrale Pixel die Signatur berechnet.
            if(i[z+o2m]>c_high) res +=       3;   else if(i[z+o2m]<c_low) res +=       6;
            if(i[z+u2m]>c_high) res += 4782969;   else if(i[z+u2m]<c_low) res += 9565938;

            if(i[z+o1l]>c_high) res +=      27;   else if(i[z+o1l]<c_low) res +=      54;
            if(i[z+o1m]>c_high) res +=      81;   else if(i[z+o1m]<c_low) res +=     162;//  3^4
            if(i[z+o1r]>c_high) res +=     243;   else if(i[z+o1r]<c_low) res +=     486;

            if(i[z+m2l]>c_high) res +=     729;   else if(i[z+m2l]<c_low) res +=    1458;
            if(i[z+m1l]>c_high) res +=    2187;   else if(i[z+m1l]<c_low) res +=    4374;
            if(i[z+m1r]>c_high) res +=    6561;   else if(i[z+m1r]<c_low) res +=   13122;//  3^8
            if(i[z+m2r]>c_high) res +=   19683;   else if(i[z+m2r]<c_low) res +=   39366;

            if(i[z+o1l]>c_high) res +=   59049;   else if(i[z+o1l]<c_low) res +=  118098;
            if(i[z+o1m]>c_high) res +=  177147;   else if(i[z+o1m]<c_low) res +=  354294;
            if(i[z+o1r]>c_high) res +=  531441;   else if(i[z+o1r]<c_low) res += 1062882;// 3^12
//            }
            // 43046721 = 3^16
// Signatur schon vorhanden, so wird zum bestehenden Eintrag xy-Koordinaten der Signatur hinzugef�gt (max. 3x). 
// Danach wird nur noch der Counter inkrementiert.
// Noch nicht vorhanden, dann wird neuer Eintrag in storage64b_verwaltung angelegt.
            if(NULL!=zeiger_array[res]) {
                p_tmp_storage = zeiger_array[res];
                p_tmp_storage->add_element(slot_no, iiw, iih);
            } else {
                if ( zaeler_storage64b_verwaltung<SIZE_VERWALTUNG ) {
                    p_tmp_storage = storage64b_verwaltung[zaeler_storage64b_verwaltung];
                    p_tmp_storage->set_index(res);
                    p_tmp_storage->add_element(slot_no, iiw, iih);  //w=x; y=h
                    zaeler_storage64b_verwaltung++;
                    zeiger_array[res] = p_tmp_storage;
                }
            }

//        }
        }

        test=1;
    }
}

///////////////////////////////////////////////////////////////////////////////////
// Berechnung der Census-Signatur und Speicherung
void CensusFlower2::AggregateSignatures2(IplImage* image1,IplImage* image2)// modified by Jian 29.06.2016
{
    // 16c: census operatoren; 8p: Ausdehnung 8 pixel; 1s: Abstand der Betrachteten Pixel: 1 pixel
    int width,height,w,min_w, max_w, min_h, max_h,z;
    int o2l, o2m, o2r, m2l, m2r, u2l, u2m, u2r, o1l, o1m, o1r, m1l, m1r, u1l, u1m, u1r; 
    int test = 0;
    uchar e;
    int slot_no =m_slot_no;
    register uchar c_low, c_high;
    storage64b* p_tmp_storage;
    unsigned long *c;
    register unsigned long res;
    unsigned char* i;
    char* itmp;

    width = image1->width;  height = image1->height;  w = image1->widthStep;
    itmp = image1->imageData;
    i = reinterpret_cast<unsigned char*>(itmp);
    e=(uchar)m_epsilon;
    min_w = m_roi_w1 + WD;  max_w = m_roi_w2 -(WD+1);
    min_h = m_roi_h1 + WD;  max_h = m_roi_h2 -(WD+1);
    z=-1;  // Index Zentrales Pixel
    c = &res;
// Berechnung der Indezies der einzelnen Pixel 
    o2l= -WD*w-WD; o2m=-WD*w; o2r=-WD*w+WD;
    m2l= -WD;                 m2r= WD;
    u2l=  WD*w-WD; u2m= WD*w; u2r= WD*w+WD;
    o1l= -ND*w-ND; o1m=-ND*w; o1r=-ND*w+ND;
    m1l= -ND;                 m1r= ND;
    u1l=  ND*w-ND; u1m= ND*w; u1r= ND*w+ND;
    
    if(STORAGE_HISTORY<=slot_no || slot_no<0)
        return;
// MUNI debug
//    int count = 0;
//    int temp = 0;
//    for (int tt = 0; tt<=5000000; tt++) {
//    	int temp = i[tt];
//    	count++;
//        std::cout << "count " << count << std::endl;
//    } // 557024

    for (int iih=min_h; iih<=max_h; iih++) {
        for (int iiw=min_w; iiw<=max_w; iiw++) {
// MUNI debug
//        	++count;
//        	if (count < 66415 ){
//            std::cout << "count " << count << std::endl;

            z = w*iih + iiw;
// Zentrales Pixel > Nachbarpixel -> 2
            c_low = i[z] - e;
// Zentrales Pixel < Nachbarpixel -> 1
            c_high = i[z] + e;
            // Census Transformation: Berechnung der Signatur
// MUNI debug
//            if (count < 50 || count > 181450) {
//            std::cout << "res: " << res << std::endl;
//            std::cout << "c_high: " << c_high << std::endl;
//            std::cout << "c_low: " << c_low << std::endl;
//            std::cout << "z: " << z << std::endl;
//            std::cout << "u2r: " << u2r << std::endl;
//            std::cout << "i[z]: " << i[z] << std::endl;
//            std::cout << "e: " << e << std::endl;
//            std::cout << "z+u2r " << z+u2r << std::endl;
//            std::cout << "i[z+u2r]: " << i[z+u2r] << std::endl;
//            std::cout << "count: " << count << std::endl;
//            std::cout << "=======" << std::endl;
//            }

//            if (count < 65005) { // JIAN ADDED 07.04
//             std::cout << "z+u2r " << z+u2r << std::endl;

            res=0;
            if(i[z+o2l]>c_high) res +=       1;   else if(i[z+o2l]<c_low) res +=       2;
            if(i[z+u2r]>c_high) res +=14348907;   else if(i[z+u2r]<c_low) res +=28697814;
//            std::cout << res << std::endl;
            if(i[z+o2r]>c_high) res +=       9;   else if(i[z+o2r]<c_low) res +=      18;
            if(i[z+u2l]>c_high) res += 1594323;   else if(i[z+u2l]<c_low) res += 3188646;
            if (0==res)        // Es wird �berpr�ft, ob die �u�eren Pixel mit dem Zentralen identisch sind, wenn ja dann wird die Census Transformation 
                continue;   // nicht mehr weiter geschrieben, sondern f�r das n�chste zentrale Pixel die Signatur berechnet.
            if(i[z+o2m]>c_high) res +=       3;   else if(i[z+o2m]<c_low) res +=       6;
            if(i[z+u2m]>c_high) res += 4782969;   else if(i[z+u2m]<c_low) res += 9565938;

            if(i[z+o1l]>c_high) res +=      27;   else if(i[z+o1l]<c_low) res +=      54;
            if(i[z+o1m]>c_high) res +=      81;   else if(i[z+o1m]<c_low) res +=     162;//  3^4
            if(i[z+o1r]>c_high) res +=     243;   else if(i[z+o1r]<c_low) res +=     486;

            if(i[z+m2l]>c_high) res +=     729;   else if(i[z+m2l]<c_low) res +=    1458;
            if(i[z+m1l]>c_high) res +=    2187;   else if(i[z+m1l]<c_low) res +=    4374;
            if(i[z+m1r]>c_high) res +=    6561;   else if(i[z+m1r]<c_low) res +=   13122;//  3^8
            if(i[z+m2r]>c_high) res +=   19683;   else if(i[z+m2r]<c_low) res +=   39366;

            if(i[z+o1l]>c_high) res +=   59049;   else if(i[z+o1l]<c_low) res +=  118098;
            if(i[z+o1m]>c_high) res +=  177147;   else if(i[z+o1m]<c_low) res +=  354294;
            if(i[z+o1r]>c_high) res +=  531441;   else if(i[z+o1r]<c_low) res += 1062882;// 3^12
//            }
            // 43046721 = 3^16
// Signatur schon vorhanden, so wird zum bestehenden Eintrag xy-Koordinaten der Signatur hinzugef�gt (max. 3x). 
// Danach wird nur noch der Counter inkrementiert.
// Noch nicht vorhanden, dann wird neuer Eintrag in storage64b_verwaltung angelegt.
            if(NULL!=zeiger_array[res]) {
                p_tmp_storage = zeiger_array[res];
                p_tmp_storage->add_element(slot_no, iiw, iih);
            } else {
                if ( zaeler_storage64b_verwaltung<SIZE_VERWALTUNG ) {
                    p_tmp_storage = storage64b_verwaltung[zaeler_storage64b_verwaltung];
                    p_tmp_storage->set_index(res);
                    p_tmp_storage->add_element(slot_no, iiw, iih);  //w=x; y=h
                    zaeler_storage64b_verwaltung++;
                    zeiger_array[res] = p_tmp_storage;
                }
            }

		if (image2 != NULL) {
			int* ptr=(int*) (image2->imageData+iih*image2->widthStep);
			ptr[iiw] = res;
		} // modified by Jian 29.06.2016
//        }
        }

        test=1;
    }
}

//************************************
// simple matching, only spatial restrictions, no history   => quite some matches ca 20000
//************************************
int CensusFlower2::MatchSignatures(void)
{
    storage64b* p_tmp;
    int no_matches = 0;
    int slot_no =m_slot_no;
    unsigned long index;
    if(STORAGE_HISTORY<=slot_no || slot_no<0)
        return no_matches;
    int no_next = (slot_no+STORAGE_HISTORY+1)%STORAGE_HISTORY;
    int no_aktuell = (slot_no+STORAGE_HISTORY)%STORAGE_HISTORY;
    int no_letzter = (slot_no+STORAGE_HISTORY-1)%STORAGE_HISTORY;
    zaeler_flow_vector_array =0;

    bool check_del;
    int diff_x, diff_y;
    for (int ii=0; ii<zaeler_storage64b_verwaltung; ii++) {
        p_tmp = storage64b_verwaltung[ii];
        /////////////////////////
        check_del=false;
        p_tmp->counter[no_next] = 0;
/* While Schleife wird solange ausgef�hrt bis das 1.Matching gefunden wurde (counter ungleich 0). Ist counter gleich 0 
   so wird die Signatur (=Index) aus dem Zeigerarray gel�scht (Null) und auch der Eintrag in der storage64b_verwaltung
   durch vertauschen der Pointer. */
        while (false==check_del) {
            if (0==p_tmp->counter[no_aktuell]) {
                index = p_tmp->index;
                zeiger_array[index] = NULL;
                zaeler_storage64b_verwaltung--;
                storage64b_verwaltung[ii] = storage64b_verwaltung[zaeler_storage64b_verwaltung];
                storage64b_verwaltung[zaeler_storage64b_verwaltung]= p_tmp;  
                p_tmp = storage64b_verwaltung[ii];
            } else {
                check_del=true;
            }
        }
/* Wurde eine �bereinstimmung gefunden (nur 1 Match), so wird Verschiebungsvekor berechnet. Dieser muss eine bestimmte 
   L�nge einhalten, sonst wird no_matches inkrementiert. Die Position des Verschiebungsvektor (Anfang & Ende) wird im flow_vector_array gespeichert. */
        if ( 1==p_tmp->counter[no_aktuell] && 1==p_tmp->counter[no_letzter]) {
            diff_x = p_tmp->get_pos_x(no_aktuell,0) - p_tmp->get_pos_x(no_letzter,0);
            diff_y = p_tmp->get_pos_y(no_aktuell,0) - p_tmp->get_pos_y(no_letzter,0);
            if ( abs(diff_x)+abs(diff_y) < FLOW_VECTOR_MAXLENGTH ) {
                if (zaeler_flow_vector_array < size_flow_vector_array) {
                    flow_vector_array[zaeler_flow_vector_array].x1 = p_tmp->get_pos_x(no_letzter,0); 
                    flow_vector_array[zaeler_flow_vector_array].y1 = p_tmp->get_pos_y(no_letzter,0);
                    flow_vector_array[zaeler_flow_vector_array].x2 = p_tmp->get_pos_x(no_aktuell,0);
                    flow_vector_array[zaeler_flow_vector_array].y2 = p_tmp->get_pos_y(no_aktuell,0);
                    zaeler_flow_vector_array++;
                }
                no_matches++;
            }
        }
    }
    return no_matches;
}
