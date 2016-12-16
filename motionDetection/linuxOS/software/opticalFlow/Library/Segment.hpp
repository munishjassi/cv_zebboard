#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <vector>

const double MIN_CONFIDENCE = 0.0;
const double MAX_CONFIDENCE = 1.0;

class Segment
{
public:
    int x1;
	int y1;
	int x2;
	int y2;
    int schwerpunkt_x;
	int schwerpunkt_y;
	int halbebreite_x;
	int halbehoehe_y;
    int predicted_schwerpunkt_x;
	int predicted_schwerpunkt_y;
    int pmd_x1;
	int pmd_y1;
	int pmd_x2;
	int pmd_y2;
    double timestamp;
    int gueltigkeit;
    int segment_id;
    int TrackID;
    int original_SegmentId;
    int prev_seg_idx;
    int class_no;
    std::vector<double> prob_est;

	// Original class_no as computed by classifier.
	int original_class_no;

	// Original prob_est as computed by classifier.
	std::vector<double> original_prob_est;

	// Measure of the quality of the prob_est.
	double confidence;

	// Number of search segments for this segment.
	// num_search_segments > 0: this is an original segment.
	// num_search_segments = 0: not split yet or search segment.
	size_t num_search_segments;

	// Index of the corresponding search / original segment in the vector of segments of the current frame.
	// This is an original segment: Index of the first search segment.
	// This is a search segment: Index of the original segment.
	int ref_idx;

	// True if this is a search segment and is considered by Engine_Merge_ROIs::Merge_Search_Segs.
	bool is_selected;

    double SegHeight;
    double SegWidth;
    double SegDepth;
    double SegMeanDistance;
    double SegStdDistance;
    double SegMeanIntensity;
    double SegStdIntensity;

    int no_points;
    std::vector<int> points_x;
    std::vector<int> points_y;

	// Default constructor. Same as init_segment(segment*).
	Segment();

	bool operator ==(const Segment& other) const;

	// Same as fit_segment_in_image(segment&).
	bool Fit_In_Image();

	// Same as update_segment_schwerpunkt_breite(segment&).
	void Update_Schwerpunkt_Breite();

	// Same as update_segment_x1_y1_x2_y2(segment&).
	void Update_X_Y();

	// Set schwerpunkt, breite and update x, y.
	void From_Schwerpunkt_Breite(int swp_x, int swp_y, int hb_x, int hh_y);

	// Set x, y and update schwerpunkt, breite.
	void From_X_Y(int x1, int y1, int x2, int y2);

	/*// Increase(diff > 0) or decrease(diff < 0) size.
	// Schwerpunkt won't be moved.
	void Resize(int diff);

	// Increase(x_diff, y_diff > 0) or decrease(x_diff, y_diff < 0) size.
	// Schwerpunkt won't be moved.
	void Resize(int x_diff, int y_diff);

	// Increase(factor > 1.0) or decrease(0.0 < factor < 1.0) size.
	// Schwerpunkt won't be moved.
	void Scale(double factor);

	// Increase(x_factor, y_factor > 1.0) or decrease(0.0 < x_factor, y_factor < 1.0) size.
	// Schwerpunkt won't be moved.
	void Scale(double x_factor, double y_factor);

	// Move in x and y direction.
	void Move(int x, int y);*/
};

inline Segment::Segment() :
	x1(0),
	y1(0),
	x2(10000),  
    y2(10000),
    pmd_x1(0),
    pmd_y1(0),
    pmd_x2(10000),  
    pmd_y2(10000),
    schwerpunkt_x(100000),
    schwerpunkt_y(100000),
    halbebreite_x(0), 
    halbehoehe_y(0),
    predicted_schwerpunkt_x(0),
    predicted_schwerpunkt_y(0),
    timestamp(-1.0),
    gueltigkeit(0),
    segment_id(-1),
    TrackID(-1),
    original_SegmentId(-1),
    no_points(0),
	points_x(),
	points_y(),
    prev_seg_idx(-1),
    class_no(-1),
	prob_est(),
	original_class_no(-1),
	original_prob_est(),
	num_search_segments(0),
	ref_idx(-1),
	is_selected(false),
    confidence(1.0),
    SegHeight(0.0),
    SegWidth(0.0),
    SegDepth(0.0),
    SegMeanDistance(0.0),
    SegStdDistance(0.0),
    SegMeanIntensity(0.0),
    SegStdIntensity(0.0)
{
}

inline bool Segment::operator ==(const Segment& other) const
{
	return	x1 == other.x1 &&
			y1 == other.y1 &&
			x2 == other.x2 &&  
			y2 == other.y2 &&
			pmd_x1 == other.pmd_x1 &&
			pmd_y1 == other.pmd_y1 &&
			pmd_x2 == other.pmd_x2 &&  
			pmd_y2 == other.pmd_y2 &&
			schwerpunkt_x == other.schwerpunkt_x &&
			schwerpunkt_y == other.schwerpunkt_y &&
			halbebreite_x == other.halbebreite_x && 
			halbehoehe_y == other.halbehoehe_y &&
			predicted_schwerpunkt_x == other.predicted_schwerpunkt_x &&
			predicted_schwerpunkt_y == other.predicted_schwerpunkt_y &&
			timestamp == other.timestamp &&
			gueltigkeit == other.gueltigkeit &&
			segment_id == other.segment_id &&
			TrackID == other.TrackID &&
			original_SegmentId == other.original_SegmentId &&
			no_points == other.no_points &&
			points_x == other.points_x &&
			points_y == other.points_y &&
			prev_seg_idx == other.prev_seg_idx &&
			class_no == other.class_no &&
			prob_est == other.prob_est &&
			original_class_no == other.original_class_no &&
			original_prob_est == other.original_prob_est &&
			num_search_segments == other.num_search_segments &&
			ref_idx == other.ref_idx &&
			is_selected == other.is_selected &&
			confidence == other.confidence &&
			SegHeight == other.SegHeight &&
			SegWidth == other.SegWidth &&
			SegDepth == other.SegDepth &&
			SegMeanDistance == other.SegMeanDistance &&
			SegStdDistance == other.SegStdDistance &&
			SegMeanIntensity == other.SegMeanIntensity &&
			SegStdIntensity == other.SegStdIntensity;
}

inline bool Segment::Fit_In_Image()
{
	bool no_change = true;

    if (x1 <0 ) { 
        x1 = 0;
        no_change = false;
    }
    if (y1 < 0) {
        y1 = 0;
        no_change = false;
    }
    if (x2 < 0) {
        x2 = 0;
        no_change = false;
    }
    if (y2 < 0) {
        y2 = 0;
        no_change = false;
    }
    if(VRMAGIC_WIDTH <= x1) {  
        x1 = VRMAGIC_WIDTH;
        no_change = false;
    }
    if(VRMAGIC_WIDTH <= x2) { 
        x2 = VRMAGIC_WIDTH;
        no_change=false;
    }
    if(VRMAGIC_HEIGHT <= y1) {
        y1 = VRMAGIC_HEIGHT;
        no_change = false;
    }
    if(VRMAGIC_HEIGHT <= y2) { 
        y2 = VRMAGIC_HEIGHT;
        no_change = false;
    }

    return no_change;
}

inline void Segment::Update_Schwerpunkt_Breite()
{
    halbebreite_x = (int)(((double)(x2-x1))/2.0);
    halbehoehe_y  = (int)(((double)(y2-y1))/2.0);
    schwerpunkt_x = x1 + halbebreite_x;
    schwerpunkt_y = y1 + halbehoehe_y;
}

inline void Segment::Update_X_Y()
{
	x1 = schwerpunkt_x - halbebreite_x;
    y1 = schwerpunkt_y - halbehoehe_y;
    x2 = schwerpunkt_x + halbebreite_x;
    y2 = schwerpunkt_y + halbehoehe_y;
}

inline void Segment::From_Schwerpunkt_Breite(int swp_x, int swp_y, int hb_x, int hh_y)
{
	schwerpunkt_x = swp_x;
	schwerpunkt_y = swp_y;
	halbebreite_x = hb_x;
	halbehoehe_y = hh_y;
	Update_X_Y();
}

inline void Segment::From_X_Y(int x1, int y1, int x2, int y2)
{
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
	Update_Schwerpunkt_Breite();
}

/*inline void Segment::Resize(int diff)
{
	Resize(diff, diff);
}

inline void Segment::Resize(int x_diff, int y_diff)
{

}*/

// For compatibility with old struct segment.
typedef Segment segment;

#endif // __SEGMENT_H__
