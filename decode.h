#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
   
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    // to store output file
     char output_fname[10];
     FILE *fptr_output_image;
     char secret_file_extn[10];
     int secret_file_extn_size;
     int secret_file_data_size;

    

} DecodeInfo;




/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);


//open file
Status open_file_decode(DecodeInfo *decInfo);


// decoding
 Status do_decode( DecodeInfo *decInfo);

 // decode byte from lsb
 char decode_byte_from_lsb( char *buffer);

 //decode int from lsb
 int decode_int_from_lsb( char *buffer);

 // decode magic string
Status decode_magic_string(  DecodeInfo *decInfo);

//decode secret file extention
Status decode_secret_file_extn_size( DecodeInfo *decInfo);

// decode secret file extention
Status decode_secret_file_extn(DecodeInfo *decInfo);

// decode secret file data size

Status decode_secret_file_data_size( DecodeInfo *decInfo);


// decode data from secret file
Status decode_secret_file_data( DecodeInfo *decInfo);
#endif
