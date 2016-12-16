/******************************
 *
 *
 */



#ifndef __CONFIG_FILE_HANDLE_H__
#define __CONFIG_FILE_HANDLE_H__


// STL:
#include <string>
#include <fstream>
#include <sstream>
//OpenCV Includes:
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
//
#include <stdio.h>
#include <iostream>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>

//!  A class for file handle. 
/*!
  details.
*/

class config_file_handle
{
private:
    std::string path_to_file;       // default: NULL
    CvFileStorage* filestorage_ptr; // default: NULL
    bool file_writeable;            // default: NULL
    bool m_b_verbose;

    bool dir_exists(const std::string& path);  // !! NOT IMPLEMENTED !!
    bool init();

public:
    config_file_handle(bool writeable=false, bool verbose=false);
    config_file_handle(std::string path_string, bool writeable=false, bool verbose=false);
    config_file_handle(const char* path_arr, bool writeable=false, bool verbose=false);
    ~config_file_handle();

    bool m_b_file_exists;
    bool file_exists(const std::string& path);
    std::string append_postfix(std::string main_string, int postfix);
////
    // Evtl. Abfrage wenn keine Configfile vorhanden
	//!Liefent einen gesuchten Integer aus der yml Datei.
    int         get_int(     std::string key_int,   int postfix_no=-1);    //Evtl ueberladen:  int get_int(const char* path_arr);
    double      get_double(  std::string key_double,int postfix_no=-1);
	//!Liefert einen gesuchten String aus der yml Datei.
    std::string get_string(  std::string key_string,int postfix_no=-1);
    //char*       get_c_str(   std::string name_string );
    //std::vector<int>         get_vector_int(    std::string vector_name_int );
    //std::vector<double>      get_vector_double( std::string vector_name_double );
    //std::vector<std::string> get_vector_string( std::string vector_name_string );

    //bool read_int(    std::string int_key,    int*         int_value);
    //bool read_double( std::string double_key, double*      double_value);
    //bool read_string( std::string string_key, std::string* string_value);
    bool read_vector_int(    std::string vector_int_key,    std::vector<int>*         vector_int_value,   int postfix_no=-1);
    bool read_vector_double( std::string vector_double_key, std::vector<double>*      vector_double_value,int postfix_no=-1);
    //!Fügt in Para2 eine Liste mit allen Frames ein.
	bool read_vector_string( std::string vector_string_key, std::vector<std::string>* vector_string_value,int postfix_no=-1);
    bool read_CvMat(         std::string mat_key, CvMat** mat_value,int postfix_no=-1);
////
    void put_comment( std::string comment_string="");
    void put_int(     std::string int_key,     int int_value,           int postfix_no=-1);
    void put_double(  std::string double_key,  double double_value,     int postfix_no=-1);
    void put_string(  std::string string_key,  std::string string_value,int postfix_no=-1);

    bool write_vector_int(    std::string vector_int_key,    std::vector<int>*         vector_int_value,   int postfix_no=-1);
    bool write_vector_ulong(  std::string vector_ulong_key,  std::vector<unsigned long>*vector_ulong_value,int postfix_no=-1);
    bool write_vector_double( std::string vector_double_key, std::vector<double>*      vector_double_value,int postfix_no=-1);
    bool write_vector_string( std::string vector_string_key, std::vector<std::string>* vector_string_value,int postfix_no=-1);
    bool write_CvMat(         std::string mat_key, CvMat* mat_value,int postfix_no=-1);
};

#endif /*__CONFIG_FILE_HANDLE_H__*/


