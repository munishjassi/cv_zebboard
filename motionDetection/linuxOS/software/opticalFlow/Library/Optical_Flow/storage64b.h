//storage64.h

#ifndef __STORAGE64B_H__
#define __STORAGE64B_H__

#ifndef STORAGE_HISTORY
#define STORAGE_HISTORY 5
#define STORAGE_LENGTH 3
#define TOTAL_LENGTH 15  //!! ==STORAGE_HISTORY*STORAGE_LENGTH !!
#endif

// STL:
//#include <vector>
//#include <deque>
//#include <iostream>
//#include <string>
//#include <map>

// Eigene:
#include "../global_defines_UOC.h"


#define INIT_STORAGE64B_STRUCT( p_storage64b_struct )                   \
    p_storage64b_struct->index = 0;                                     \
    for (int ii=0; ii<STORAGE_HISTORY;ii++) {                           \
        p_storage64b_struct->counter[ii] = 0;                           \
        for (unsigned kk=0; kk<STORAGE_LENGTH;kk++) {                   \
            p_storage64b_struct->correspondency_to_prior[(ii*STORAGE_LENGTH+kk)] = -1;  \
        }                                                               \
    }

#define SET_INDEX(p_storage64b_struct, _index)       \
    p_storage64b_struct->index = _index;             \
    for (int ii=0; ii<STORAGE_HISTORY;ii++) {        \
        p_storage64b_struct->counter[ii] = 0;        \
    }

#define ADD_ELEMENT(p_storage64b_struct, modus, x, y)                                                                     \
    if(p_storage64b_struct->counter[modus]<STORAGE_LENGTH) {                                                              \
        p_storage64b_struct->pos_x[ (modus*STORAGE_LENGTH+p_storage64b_struct->counter[modus]) ] = x;                     \
        p_storage64b_struct->pos_y[ (modus*STORAGE_LENGTH+p_storage64b_struct->counter[modus]) ] = y;                     \
        p_storage64b_struct->correspondency_to_prior[ (modus*STORAGE_LENGTH+p_storage64b_struct->counter[modus]) ] = -1;  \
    }                                                                                                                     \
    p_storage64b_struct->counter[modus]++;

//#define GET_POS_X(pos_x, p_storage64b_struct, modus, rang)  \
//        if(rang<STORAGE_LENGTH) {  \
//            pos_x = p_storage64b_struct->pos_x[ (modus*STORAGE_LENGTH+rang) ]; \
//        } else           \
//            pos_x = 0; 
//
//#define GET_POS_Y(pos_y, p_storage64b_struct, modus, rang)  \
//        if(rang<STORAGE_LENGTH) {  \
//            pos_y = p_storage64b_struct->pos_y[ (modus*STORAGE_LENGTH+rang) ]; \
//        } else           \
//            pos_y = 0; 

#define GET_POS_X(position_x, p_storage64b_struct, modus, rang)                  \
    position_x = p_storage64b_struct->pos_x[ (modus*STORAGE_LENGTH+rang) ];

#define GET_POS_Y(position_y, p_storage64b_struct, modus, rang)                  \
    position_y = p_storage64b_struct->pos_y[ (modus*STORAGE_LENGTH+rang) ];

#define GET_COUNTER(counter, p_storage64b_struct, modus)                    \
        counter = p_storage64b_struct->counter[modus];

#define GET_CORRESPONDENCY_TO_PRIOR(correspondency_to_prior, p_storage64b_struct, modus, rang)                       \
            correspondency_to_prior = p_storage64b_struct->correspondency_to_prior[ (modus*STORAGE_LENGTH+rang) ];

#define SET_CORRESPONDENCY_TO_PRIOR(p_storage64b_struct, modus, rang, correspondency_rang)                           \
            p_storage64b_struct->correspondency_to_prior[ (modus*STORAGE_LENGTH+rang) ] = correspondency_rang;

#define DELETE_MODUS(p_storage64b_struct, modus)                                            \
        p_storage64b_struct->counter[modus]=0;                                              \
        for (unsigned kk=0; kk<STORAGE_LENGTH;kk++) {                                       \
            p_storage64b_struct->correspondency_to_prior[(modus*STORAGE_LENGTH+kk)] = -1;   \
        }

struct storage64b_struct {
    unsigned long index;
    unsigned long counter[STORAGE_HISTORY];
    short pos_x[STORAGE_HISTORY * STORAGE_LENGTH];
    short pos_y[STORAGE_HISTORY * STORAGE_LENGTH];
    short correspondency_to_prior[STORAGE_HISTORY * STORAGE_LENGTH];
};

class storage64b
{
public:
    unsigned long index;
    unsigned long counter[STORAGE_HISTORY];
    short pos_x[STORAGE_HISTORY * STORAGE_LENGTH];
    short pos_y[STORAGE_HISTORY * STORAGE_LENGTH];
    short correspondency_to_prior[STORAGE_HISTORY * STORAGE_LENGTH];

    inline storage64b(void) {
        index = 0;
        for (int ii=0; ii<STORAGE_HISTORY;ii++) {
            counter[ii] = 0;
            for (unsigned kk=0; kk<STORAGE_LENGTH;kk++) {
                //pos_x[(ii*STORAGE_LENGTH+kk)] = 0;
                //pos_y[(ii*STORAGE_LENGTH+kk)] = 0;
                correspondency_to_prior[(ii*STORAGE_LENGTH+kk)] = -1;
            }
        }
    };
    inline storage64b(unsigned long _index) {
        index = _index;
        for (int ii=0; ii<STORAGE_HISTORY;ii++) {
            counter[ii] = 0;
            for (unsigned kk=0; kk<STORAGE_LENGTH;kk++) {
                //pos_x[(ii*STORAGE_LENGTH+kk)] = 0;
                //pos_y[(ii*STORAGE_LENGTH+kk)] = 0;
                correspondency_to_prior[(ii*STORAGE_LENGTH+kk)] = -1;
            }
        }
    };
    inline storage64b(const storage64b &ref) {
        unsigned ii=0;
        index = ref.index;
        for (ii=0; ii<STORAGE_HISTORY;ii++) {
            counter[ii] = ref.counter[ii];
        }
        for (ii=0; ii<STORAGE_HISTORY;ii++) {
            for (unsigned kk=0; kk<counter[ii];kk++) {
                pos_x[(ii*STORAGE_LENGTH+kk)] = ref.pos_x[(ii*STORAGE_LENGTH+kk)];
                pos_y[(ii*STORAGE_LENGTH+kk)] = ref.pos_y[(ii*STORAGE_LENGTH+kk)];
                correspondency_to_prior[(ii*STORAGE_LENGTH+kk)] = ref.correspondency_to_prior[(ii*STORAGE_LENGTH+kk)];
            }
        }

    };
    inline ~storage64b(void) {
    };
    inline storage64b& operator= (const storage64b &ref) {
        unsigned ii=0;
        index = ref.index;
        for (ii=0; ii<STORAGE_HISTORY;ii++) {
            counter[ii] = ref.counter[ii];
        }
        for (ii=0; ii<STORAGE_HISTORY;ii++) {
            for (unsigned kk=0; kk<counter[ii];kk++) {
                pos_x[(ii*STORAGE_LENGTH+kk)] = ref.pos_x[(ii*STORAGE_LENGTH+kk)];
                pos_y[(ii*STORAGE_LENGTH+kk)] = ref.pos_y[(ii*STORAGE_LENGTH+kk)];
                correspondency_to_prior[(ii*STORAGE_LENGTH+kk)] = ref.correspondency_to_prior[(ii*STORAGE_LENGTH+kk)];
            }
        }
        return *this;
    };
    inline void set_index(unsigned long _index) {
        index = _index;
        for (int ii=0; ii<STORAGE_HISTORY;ii++) {
            counter[ii] = 0;
        }
    }
    inline void add_element(const int &modus, const short &x,const short &y) {
        //if(modus<STORAGE_HISTORY) {
            if(counter[modus]<STORAGE_LENGTH) {
                pos_x[ (modus*STORAGE_LENGTH+counter[modus]) ] = x;
                pos_y[ (modus*STORAGE_LENGTH+counter[modus]) ] = y;
                correspondency_to_prior[ (modus*STORAGE_LENGTH+counter[modus]) ] = -1;
            }
            counter[modus]++;
        //}
    };
    inline short get_pos_x(const int &modus, const int &rang) {
        //if(modus<STORAGE_HISTORY) {
            if(rang<STORAGE_LENGTH) {
                return pos_x[ (modus*STORAGE_LENGTH+rang) ];
            } else 
                return 0;
        //} else 
        //    return 0;
    };
    inline short get_pos_y(const int &modus, const int &rang) {
        //if(modus<STORAGE_HISTORY) {
            if(rang<STORAGE_LENGTH) {
                return pos_y[ (modus*STORAGE_LENGTH+rang) ];
            } else 
                return 0;
        //} else 
        //    return 0;
    };
    inline unsigned long get_counter(const int &modus) {
        //if(modus<STORAGE_HISTORY) {
            return counter[modus];
        //} else 
        //    return 0;
    };
    inline short get_correspondency_to_prior(const int &modus, const int &rang) {
        //if(modus<STORAGE_HISTORY) {
            if(rang<STORAGE_LENGTH) {
                return correspondency_to_prior[ (modus*STORAGE_LENGTH+rang) ];
            } else 
                return -1;
        //} else 
        //    return -1;
    };
    inline void set_correspondency_to_prior(const int &modus, const int &rang, const short &correspondency_rang) {
        //if(modus<STORAGE_HISTORY) {
            if(rang<STORAGE_LENGTH) {
                correspondency_to_prior[ (modus*STORAGE_LENGTH+rang) ] = correspondency_rang;
            }
        //} 
    };
    inline void delete_modus(const int &modus) {
        //if(modus<STORAGE_HISTORY) {
            counter[modus]=0;
            for (unsigned kk=0; kk<STORAGE_LENGTH;kk++) {
                //pos_x[(ii*STORAGE_LENGTH+kk)] = 0;
                //pos_y[(ii*STORAGE_LENGTH+kk)] = 0;
                correspondency_to_prior[(modus*STORAGE_LENGTH+kk)] = -1;
            }
        //}
    };
    inline void reset(void) {
        index = 0;
        for (int ii=0; ii<STORAGE_HISTORY;ii++) {
            counter[ii] = 0;
            for (unsigned kk=0; kk<STORAGE_LENGTH;kk++) {
                //pos_x[(ii*STORAGE_LENGTH+kk)] = 0;
                //pos_y[(ii*STORAGE_LENGTH+kk)] = 0;
                correspondency_to_prior[(ii*STORAGE_LENGTH+kk)] = -1;
            }
        }
    };

};


#endif /*__STORAGE64B_H__*/



