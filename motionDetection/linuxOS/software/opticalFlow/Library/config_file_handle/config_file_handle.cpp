/******************************
 *
 *
 * for yaml-spec  see http://www.yaml.org/spec/
 *
 *  Beispiel:
 *  m_b_DoLogg = (0==m_p_conf->get_int("m_b_DoLogg") ? 0 : 1);
 */

//#include "stdafx.h"
#include "./config_file_handle.h"
//Debug>#include <stdio.h>



config_file_handle::config_file_handle(bool writeable, bool verbose) {
    path_to_file    = "";
    filestorage_ptr = NULL;
    file_writeable = writeable;
    m_b_verbose = verbose;
    m_b_file_exists =false;
};


config_file_handle::config_file_handle(std::string path_string, bool writeable, bool verbose) {
    filestorage_ptr = NULL;
    file_writeable = writeable;
    path_to_file = path_string;
    m_b_verbose = verbose;
    m_b_file_exists =false;
    init(); // Actual initialization
};


config_file_handle::config_file_handle(const char* path_arr, bool writeable, bool verbose) {
    filestorage_ptr = NULL;
    file_writeable = writeable;
    std::string temp_str(path_arr);
    path_to_file = temp_str;
    m_b_verbose = verbose;
    m_b_file_exists =false;
    init(); // Actual initialization
};


config_file_handle::~config_file_handle()
{
    cvReleaseFileStorage( &filestorage_ptr );
    if(m_b_verbose)
        std::cout << "File Released: " << path_to_file << std::endl;
};

/////////////////////

int config_file_handle::get_int(std::string key_int, int postfix_no)  // evtl für char[] ueberladen
{
std::string name_int;
int temp_value = 0;
char *temp_str_ptr =0;

name_int = append_postfix(key_int, postfix_no);
if (0==file_writeable) {
    if (1) {
        temp_str_ptr = new char[(name_int.length()+1)];
        strcpy(temp_str_ptr, name_int.c_str());
        temp_value = cvReadIntByName( filestorage_ptr, 0,  temp_str_ptr );
        delete[] temp_str_ptr;
    }
    else
        if(m_b_verbose)
            std::cout << "Variable not found: " << name_int << std::endl;
}
else
    if(m_b_verbose)
        std::cout << "File not readable" << std::endl;
return temp_value;
};


double config_file_handle::get_double(std::string key_double, int postfix_no) // evtl für char[] ueberladen
{
std::string name_double;
double temp_value = 0;
char *temp_str_ptr =0;

name_double = append_postfix(key_double, postfix_no);
if (0==file_writeable)
    if (1) {
        temp_str_ptr = new char[(name_double.length()+1)];
        strcpy(temp_str_ptr, name_double.c_str());
        temp_value = cvReadRealByName( filestorage_ptr, 0,  temp_str_ptr );
        delete[] temp_str_ptr;
    }
    else
        if(m_b_verbose)
            std::cout << "Variable not found: " << name_double << std::endl;
else
    if(m_b_verbose)
        std::cout << "File not readable" << std::endl;
return temp_value;
};


std::string config_file_handle::get_string(std::string key_string, int postfix_no) // evtl für char[] ueberladen
{
std::string name_string;
char *temp_str_ptr =0;

name_string = append_postfix(key_string, postfix_no);
if (0==file_writeable)
    if (1) {
        temp_str_ptr = new char[(name_string.length()+1)];
        strcpy(temp_str_ptr, name_string.c_str());
        //Debug>puts ( cvReadStringByName(filestorage_ptr, 0,  temp_str_ptr )  );
        std::string temp_value( cvReadStringByName(filestorage_ptr, 0,  temp_str_ptr ) );
        delete[] temp_str_ptr;
        return temp_value;
    }
    else {
        std::string temp_value = "";
        if(m_b_verbose)
            std::cout << "Variable not found: " << name_string << std::endl;
        return temp_value;
    }
else {
    if(m_b_verbose)
        std::cout << "File not readable" << std::endl;
    std::string temp_value = "";
    return temp_value;
}
};


bool config_file_handle::read_vector_int( std::string vector_int_key, std::vector<int>* vector_int_value, int postfix_no)
{
std::string vector_int_key1;
bool status=1;
int *temp_int_arr_ptr = 0;
char *temp_str_ptr =0;
CvFileNode* root_node = 0;
CvFileNode* current_node = 0;
CvSeqReader reader;

vector_int_key1 = append_postfix(vector_int_key, postfix_no);
if (0==file_writeable) {
    root_node = cvGetRootFileNode( filestorage_ptr );
    temp_str_ptr = new char[(vector_int_key1.length()+1)];
    strcpy(temp_str_ptr, vector_int_key1.c_str());
    current_node = cvGetFileNodeByName( filestorage_ptr, root_node, temp_str_ptr );
    delete[] temp_str_ptr; temp_str_ptr=0;
    if ( 0!=current_node ) {
        int total = current_node->data.seq->total;
        if (0!=total) {
            cvStartReadRawData( filestorage_ptr, current_node, &reader );
            temp_int_arr_ptr = new int[total];
            cvReadRawDataSlice( filestorage_ptr, &reader, total, temp_int_arr_ptr,"i" );
             //!!!!!!!!!!!!!!!!
            vector_int_value->clear();
            for (int ii=0; ii<total; ii++)
                vector_int_value->push_back( temp_int_arr_ptr[ii] );
            delete[] temp_int_arr_ptr; temp_int_arr_ptr=0;
        }
        status=0;
    }
    else
        if(m_b_verbose)
            std::cout << "Variable not found: " << vector_int_key1 << std::endl;
}
else
    if(m_b_verbose)
        std::cout << "File not readable" << std::endl;
return status;
};

bool config_file_handle::read_vector_double( std::string vector_double_key, std::vector<double>* vector_double_value, int postfix_no)
{
std::string vector_double_key1;
bool status=1;
double *temp_double_arr_ptr = 0;
char *temp_str_ptr =0;
CvFileNode* root_node = 0;
CvFileNode* current_node = 0;
CvSeqReader reader;

vector_double_key1 = append_postfix(vector_double_key, postfix_no);
if (0==file_writeable) {
    root_node = cvGetRootFileNode( filestorage_ptr );
    temp_str_ptr = new char[(vector_double_key1.length()+1)];
    strcpy(temp_str_ptr, vector_double_key1.c_str());
    current_node = cvGetFileNodeByName( filestorage_ptr, root_node, temp_str_ptr );
    delete[] temp_str_ptr; temp_str_ptr=0;
    if ( 0!=current_node ) {
        int total = current_node->data.seq->total;
        if(0!=total) {
            cvStartReadRawData( filestorage_ptr, current_node, &reader );
            temp_double_arr_ptr = new double[total];
            cvReadRawDataSlice( filestorage_ptr, &reader, total, temp_double_arr_ptr,"d" );
             //!!!!!!!!!!!!!!!!
            vector_double_value->clear();
            for (int ii=0; ii<total; ii++)
                vector_double_value->push_back( temp_double_arr_ptr[ii] );
            delete[] temp_double_arr_ptr; temp_double_arr_ptr=0;
        }
        status=0;
    }
    else
        if(m_b_verbose)
            std::cout << "Variable not found: " << vector_double_key1 << std::endl;
}
else
    if(m_b_verbose)
        std::cout << "File not readable" << std::endl;
return status;
};

bool config_file_handle::read_vector_string( std::string vector_string_key, std::vector<std::string>* vector_string_value, int postfix_no)
{
std::string vector_string_key1;
bool status=1;
///double *temp_double_arr_ptr = 0;
char *temp_str_ptr =0;
CvFileNode* root_node = 0;
CvFileNode* current_node = 0;
CvSeqReader reader;

vector_string_key1 = append_postfix(vector_string_key, postfix_no);
if (0==file_writeable) {
    root_node = cvGetRootFileNode( filestorage_ptr );
    temp_str_ptr = new char[(vector_string_key1.length()+1)];
    strcpy(temp_str_ptr, vector_string_key1.c_str());
    current_node = cvGetFileNodeByName( filestorage_ptr, root_node, temp_str_ptr );
    delete[] temp_str_ptr; temp_str_ptr=0;
    if ( 0!=current_node ) {
        if( CV_NODE_IS_SEQ(current_node->tag) ) {
            int i, total;
            CvSeq* seq = current_node->data.seq;
            cvStartReadSeq( seq, &reader, 0 );
            total = seq->total;
            vector_string_value->clear();
            for( i = 0; i < total; i++ ) {
                CvFileNode* pt = (CvFileNode*)reader.ptr;
                vector_string_value->push_back( cvReadString(pt) );
                cvSetSeqReaderPos( &reader, i+1 , 0 );
            }
        status=0;
        }
    }
    else
        if(m_b_verbose)
            std::cout << "Variable not found: " << vector_string_key1 << std::endl;
}
else
    if(m_b_verbose)
        std::cout << "File not readable" << std::endl;
return status;
};

bool config_file_handle::read_CvMat( std::string mat_key, CvMat** mat_value,int postfix_no/*=-1*/ )
{
    std::string mat_key1;
    char *temp_str_ptr = NULL;
    bool status=false;
    CvFileNode* root_node = NULL;
    CvFileNode* node = NULL;
    mat_key1 = append_postfix(mat_key, postfix_no); 
    if (0==file_writeable)
        if( 0!=filestorage_ptr ) {
            root_node = cvGetRootFileNode( filestorage_ptr );
            temp_str_ptr = new char[(mat_key1.length()+1)];
            strcpy(temp_str_ptr, mat_key1.c_str());
            node = cvGetFileNodeByName( filestorage_ptr, NULL, temp_str_ptr );
            *mat_value = (CvMat*)cvRead( filestorage_ptr, node, 0 );
            //mat_value = (CvMat*)node->info->read( filestorage_ptr, node );
            delete[] temp_str_ptr; temp_str_ptr=NULL;
            status=true;
        }
        else
            if(m_b_verbose)
                std::cout << "Variable not found: " << mat_key1 << std::endl;
    else
        if(m_b_verbose)
            std::cout << "File not readable" << std::endl;
    return status;
};

/////////////////////

void config_file_handle::put_comment(std::string comment_string)
{
if(""==comment_string) {
comment_string = "################################################################";
}
char* temp_str_ptr = new char[(comment_string.length()+1)];
strcpy(temp_str_ptr, comment_string.c_str());

if (1==file_writeable)
    if( 0!=filestorage_ptr )
        cvWriteComment(filestorage_ptr, temp_str_ptr, 0 );
    else
        if(m_b_verbose)
            std::cout <<  comment_string << std::endl;
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr;
};


void config_file_handle::put_int(std::string int_key, int int_value, int postfix_no)
{
std::string int_key1;

int_key1 = append_postfix(int_key, postfix_no);
char* temp_str_ptr = new char[(int_key1.length()+1)];
strcpy(temp_str_ptr, int_key1.c_str());
if (1==file_writeable)
    if( 0!=filestorage_ptr )
        cvWriteInt( filestorage_ptr, temp_str_ptr, int_value );
    else
        if(m_b_verbose)
            std::cout << int_key1 << ": " << int_value << std::endl;
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr;
};


void config_file_handle::put_double(std::string double_key, double double_value, int postfix_no)
{
std::string double_key1;

double_key1 = append_postfix(double_key, postfix_no);
char* temp_str_ptr = new char[(double_key1.length()+1)];
strcpy(temp_str_ptr, double_key1.c_str());
if (1==file_writeable)
    if( 0!=filestorage_ptr )
        cvWriteReal( filestorage_ptr, temp_str_ptr, double_value);
    else
        if(m_b_verbose)
            std::cout << double_key1 << ": " << double_value << std::endl;
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr;
};


void config_file_handle::put_string(std::string string_key, std::string string_value, int postfix_no)
{
std::string string_key1;

string_key1 = append_postfix(string_key, postfix_no);
char* temp_str_ptr = new char[(string_key1.length()+1)];
char* temp_str_ptr2 = new char[(string_value.length()+1)]; //LEAK_removed
strcpy(temp_str_ptr, string_key1.c_str());
strcpy(temp_str_ptr2, string_value.c_str());
if (1==file_writeable)
    if( 0!=filestorage_ptr )
        cvWriteString(filestorage_ptr, temp_str_ptr, temp_str_ptr2, 0);
    else
        if(m_b_verbose)
            std::cout << string_key1 << ": " << string_value << std::endl;
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr;
delete[] temp_str_ptr2;
};


bool config_file_handle::write_vector_int( std::string vector_int_key, std::vector<int>* vector_int_value, int postfix_no)
{
std::string vector_int_key1;
bool status=1;

vector_int_key1 = append_postfix(vector_int_key, postfix_no);
char* temp_str_ptr = new char[(vector_int_key1.length()+1)];
int* temp_int_ptr = 0;
strcpy(temp_str_ptr, vector_int_key1.c_str());

if (1==file_writeable) {
    if( 0!=filestorage_ptr ) {
        temp_int_ptr = new int [(int)vector_int_value->size()];
        copy( vector_int_value->begin(), vector_int_value->end(), temp_int_ptr);
        cvStartWriteStruct(filestorage_ptr, temp_str_ptr, CV_NODE_SEQ+CV_NODE_FLOW );
        cvWriteRawData( filestorage_ptr, temp_int_ptr, (int)vector_int_value->size(), "i" );
        cvEndWriteStruct(filestorage_ptr);
        delete [] temp_int_ptr;
        status=0;
        }
    else {
        if(m_b_verbose)
            std::cout << vector_int_key1 << ": " << std::endl;
        //std::cout << vector_int_key1 << ": " << int_value << std::endl;  //!!!!!!!!!!!!
        }
    }
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr; temp_str_ptr=0;
return status;
};

bool config_file_handle::write_vector_ulong(std::string vector_ulong_key,  std::vector<unsigned long>*vector_ulong_value,int postfix_no)
{
    std::string vector_ulong_key1;
    bool status=1;

    vector_ulong_key1 = append_postfix(vector_ulong_key, postfix_no);
    char* temp_str_ptr = new char[(vector_ulong_key1.length()+1)];
    int* temp_int_ptr = 0;
    strcpy(temp_str_ptr, vector_ulong_key1.c_str());

    if (1==file_writeable) {
        if( 0!=filestorage_ptr ) {
            temp_int_ptr = new int [(int)vector_ulong_value->size()];
            copy( vector_ulong_value->begin(), vector_ulong_value->end(), temp_int_ptr);
            cvStartWriteStruct(filestorage_ptr, temp_str_ptr, CV_NODE_SEQ+CV_NODE_FLOW );
            cvWriteRawData( filestorage_ptr, temp_int_ptr, (unsigned long)vector_ulong_value->size(), "i" );
            cvEndWriteStruct(filestorage_ptr);
            delete [] temp_int_ptr;
            status=0;
        }
        else {
            if(m_b_verbose)
                std::cout << vector_ulong_key1 << ": " << std::endl;
            //std::cout << vector_int_key1 << ": " << int_value << std::endl;  //!!!!!!!!!!!!
        }
    }
    else
        if(m_b_verbose)
            std::cout << "File not writable" << std::endl;
    delete[] temp_str_ptr; temp_str_ptr=0;
    return status;
};


bool config_file_handle::write_vector_double( std::string vector_double_key, std::vector<double>* vector_double_value, int postfix_no)
{
std::string vector_double_key1;
bool status=1;

vector_double_key1 = append_postfix(vector_double_key, postfix_no);
char* temp_str_ptr = new char[(vector_double_key1.length()+1)];
double* temp_double_ptr = 0;
strcpy(temp_str_ptr, vector_double_key1.c_str());

if (1==file_writeable) {
    if( 0!=filestorage_ptr ) {
        temp_double_ptr = new double [(int)vector_double_value->size()];
        copy( vector_double_value->begin(), vector_double_value->end(), temp_double_ptr);
        cvStartWriteStruct(filestorage_ptr, temp_str_ptr, CV_NODE_SEQ );
        cvWriteRawData( filestorage_ptr, temp_double_ptr, (int)vector_double_value->size(), "d" );
        cvEndWriteStruct(filestorage_ptr);
        delete [] temp_double_ptr; temp_double_ptr=0;
        status=0;
        }
    else {
        if(m_b_verbose)
            std::cout << vector_double_key1 << ": " << std::endl;
        //std::cout << vector_int_key1 << ": " << int_value << std::endl;  //!!!!!!!!!!!!
        }
    }
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr; temp_str_ptr=0;
return status;
};

bool config_file_handle::write_vector_string( std::string vector_string_key, std::vector<std::string>* vector_string_value, int postfix_no)
{
std::string vector_string_key1;
bool status=1;
char* temp_str_ptr = 0;

vector_string_key1 = append_postfix(vector_string_key, postfix_no);
if (1==file_writeable) {
    if( 0!=filestorage_ptr ) {
        temp_str_ptr = new char[(vector_string_key1.length()+1)];
        strcpy(temp_str_ptr, vector_string_key1.c_str());
        cvStartWriteStruct( filestorage_ptr, temp_str_ptr, CV_NODE_SEQ );
        delete[] temp_str_ptr; temp_str_ptr=0;

        std::vector<std::string>::iterator strings_iterator;
        for( strings_iterator= vector_string_value->begin(); strings_iterator != vector_string_value->end(); strings_iterator++ ) {
            temp_str_ptr = new char[( (*strings_iterator).length()+1 )];
            strcpy( temp_str_ptr, (*strings_iterator).c_str() );
            cvWriteString( filestorage_ptr, NULL, temp_str_ptr );
            delete[] temp_str_ptr; temp_str_ptr=0;
            }
        cvEndWriteStruct(filestorage_ptr);
        status=0;
        }
    else {
        if(m_b_verbose)
            std::cout << vector_string_key1 << ": " << std::endl;
        //std::cout << vector_int_key1 << ": " << int_value << std::endl;  //!!!!!!!!!!!!
        }
    }
else
    if(m_b_verbose)
        std::cout << "File not writable" << std::endl;
delete[] temp_str_ptr; temp_str_ptr=0;
return status;
};

bool config_file_handle::write_CvMat( std::string mat_key, CvMat* mat_value,int postfix_no) 
{
    std::string mat_key1;

    mat_key1 = append_postfix(mat_key, postfix_no);
    char* temp_str_ptr = new char[(mat_key1.length()+1)];
    strcpy(temp_str_ptr, mat_key1.c_str());
    if (1==file_writeable)
        if( 0!=filestorage_ptr )
            cvWrite( filestorage_ptr, temp_str_ptr, mat_value, cvAttrList(0,0) );
        else
            if(m_b_verbose)
                std::cout << mat_key1 << ": " << "No Output avaliable!" << std::endl;
            else
                if(m_b_verbose)
                    std::cout << "File not writable" << std::endl;
    delete[] temp_str_ptr;
    return 1;
};


///////////////////////// privater Bereich /////////////////////////


bool config_file_handle::file_exists(const std::string& path)  {
    //char *temp_str_ptr =NULL;
    //struct stat fileinfo;
    bool file_existing = false;

    //temp_str_ptr = new char[(path.length()+1)];
    //strcpy(temp_str_ptr, path.c_str());
    //if( 0==stat(temp_str_ptr, &fileinfo) )
    //    file_existing = true;
    //delete[] temp_str_ptr; temp_str_ptr=0;
    //return file_existing;

    std::fstream fin;
    fin.open(path.c_str(),std::ios::in);
    if( fin.is_open() ) {
        fin.close();
        return file_existing = true;
    }
    fin.close();
    return file_existing;
}

bool config_file_handle::dir_exists(const std::string& path) {

    return true;
}

bool config_file_handle::init() {
    char *temp_str_ptr =0;

    temp_str_ptr = new char[(path_to_file.length()+1)];
    strcpy(temp_str_ptr, path_to_file.c_str());
    if(0==file_writeable) {
        if( 1==file_exists(path_to_file) ) {
            m_b_file_exists=1;
            filestorage_ptr = cvOpenFileStorage( temp_str_ptr, NULL, CV_STORAGE_READ );
        }
        else {
            if(m_b_verbose)
                std::cout << "File not found: " << path_to_file << std::endl;
        }
    }
    else {
        if(1) { //TODO:  check wheter dir exists
            m_b_file_exists = 1;
            filestorage_ptr = cvOpenFileStorage( temp_str_ptr, NULL, CV_STORAGE_WRITE );
        }
        else {
            if(m_b_verbose)
                std::cout << "Dir not found: " << path_to_file << std::endl;
        }
    }
    delete[] temp_str_ptr;
    return true;
};

std::string config_file_handle::append_postfix(std::string main_string, int postfix) {
    std::string out_string;
    if (0>postfix) {
        out_string = main_string;
    }
    else {
        std::stringstream tmp_ss;
        std::string s_tmp;
        tmp_ss << postfix;
        tmp_ss >> s_tmp;
        out_string = main_string + "_" + s_tmp;
    }
    return out_string;
}

