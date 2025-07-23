#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

/* Function Definitions */
Status read_and_validate_encode_args( char *argv[] , EncodeInfo *encInfo)
{

    if( strstr( argv[2] , ".bmp") )     // checking the cla of 2 file extention is .bmp 
    {
	encInfo->src_image_fname=argv[2];   // if its is .bmp store it into src_imaage 
    }
    else
    {
	return e_failure; 			//else stop
    }
    if( strstr( argv[3] , ".txt") )		//checking the clas of 3 is .txt or not.
    {
	encInfo->secret_fname=argv[3];           // if is .txt store it into secret file
	strcpy(encInfo->extn_secret_file,".txt");   //its extention also store to find secret file extention and extention size. 
    }
    else
	if( strstr( argv[3] , ".sh")  )			//for .sh file
	{
	    encInfo->secret_fname=argv[3];
	    strcpy(encInfo->extn_secret_file,".sh");
	}
	else
	    if( strstr( argv[3] , ".c") )		//for .c
	    {
		encInfo->secret_fname=argv[3];
		strcpy(encInfo->extn_secret_file,".c");
	    }
	    else
	    {
		return e_failure;				//else stop
	    }
    if( argv[4] )					// checking user given output file name 
    {
	if( strstr( argv[4] , ".bmp") )			// if yes the check it contain .bmp or not
	{
	    encInfo->stego_image_fname=argv[4];        //if yes then store to stego image.

	}
	else
	{
	    return e_failure;                            // if input file dont containn .bmp then stop.
	}
    }
    else
    {
	encInfo->stego_image_fname="default.bmp";         // if no file is provide  by the user the take "default.bmp".

    }
    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image) 		//function is used for bmp file for
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);              

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size( FILE *fptr)    		// function used to find the size of the file 
{
    rewind(fptr);                      //reset the file pointer to the first
    fseek(fptr,0,SEEK_END);		// using fseek moving file_pointer to the end
    return ftell(fptr);			//return size.
}

Status do_encoding(EncodeInfo *encInfo)    // it function used to call other function
{
    int openfile=open_files( encInfo);	 //used to check file is exting
    if( openfile == e_success)
    {
	printf("all files are opened\n");
    }
    else
    {
	printf("failed to open provided file\n");
    }
    int capacity = check_capacity( encInfo);    // function used to check the capacity if the secret file size is greater than file size
    if( capacity == e_success)
    {
	printf("check_capacity is success\n");
    }
    else
    {
	printf("failed the check_capacity test\n");
    }
    if( copy_bmp_header( encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)  // copying bmp header from source to stego image
    {
	printf("copying bmp header success\n");
    }
    else
    {
	printf("coping bmp header failed\n");
    }
    if( encode_magic_string( MAGIC_STRING , encInfo) == e_success)			//encoding magic string with src image data after header file
    {
	printf("encoding magic string success\n");
    }
    else
    {
	printf("encoding magic string failed\n");
    }


    if (encode_secret_file_extn_size( strlen( encInfo->extn_secret_file) ,encInfo) == e_success)    //ecoding secret file extention it usefull when decoding .( how many bytes).
    {
	printf("encoding secret file extention size success\n");
    }
    else
    {
	printf("encoding secret file extention size failed\n");
    }


    if (encode_secret_file_extn( encInfo->extn_secret_file, encInfo) == e_success)        //encoding secret file extention 
    {
	printf("encoding secret file extention success\n");
    }
    else
    {
	printf("encoding secret file extention failed\n");
    }

    if(  encode_secret_file_size( encInfo->size_secret_file, encInfo) == e_success)   //encoding secret file size
    {
	printf("encoding secret file size success\n");
    }
    else
    {
	printf("encoding secret file size failed\n");
    }


    if ( encode_secret_file_data(encInfo) == e_success)          //encoding secret file data
    {
	printf("encoding secret file data success\n");
    }
    else
    {
	printf("encoding secret file data failed\n");
    }


    if ( copy_remaining_img_data( encInfo->fptr_src_image , encInfo->fptr_stego_image) == e_success)    // copying all the remaining data 
    {
	printf("copy remaining data success\n");
    }
    else
    {
	printf("copying remaining data failed\n");
    }

    return e_success;

}
Status copy_bmp_header( FILE *fptr_stego_image , FILE *fptr_dest_image)
{

    char array[54];                                         // first 54 byte is header from src it copy to stego
    fseek(fptr_stego_image,0,SEEK_SET);                    // set file pointer to start
    fread( array, 54 , 1, fptr_stego_image);               //using fread to read 54 bytes from src and store it to array
    fwrite( array,54,1, fptr_dest_image);  			// using fwrite to copy all to defalut image
    return e_success;
}

Status encode_byte_to_lsb( char data , char *array)
{
    for( int i=0;i<8;i++)  			 		//run loop for 8 times because it for 1 byte		
    {
	array[i] = array[i] & ~(1);				//clear the lsb of array[i] and store it array[i]
	array[i] = array[i] | (( data >> 7-i) & 1) ;		//storing the data to lsb of array[i] 
    }
}
Status encode_int_to_lsb( int  size , char *array)
{
    for( int i=0; i< 32; i++)					//run loop for 32 times for 4 byte
    {
	array[i] = array[i] & ~(1);                              //clear the lsb bit of array[i]
	array[i] = array[i] | (( size >> 31 -i ) & 1);          //adding data to lsb of array[i]
    }
}
Status encode_magic_string( const char *magic_string, EncodeInfo *encInfo) 		//encoding magic string with lBS of the src image and store it into default 
{

    char array[8];  								//used to store src_image data                                 
    for( int i=0; i< strlen( MAGIC_STRING); i++)
    {
	fread(array,8,1,encInfo->fptr_src_image);				//read byte from src ( to encode 1 byte from magic string 8 byte need to take from src_image
	encode_byte_to_lsb( MAGIC_STRING[i],array);                             // calling byte_to_msb 
	fwrite( array,8,1,encInfo->fptr_stego_image);				//write the result in stegoimage
    }
    return e_success;
}
Status encode_secret_file_extn_size( int size, EncodeInfo *encInfo)
{
    char array[32];                                              //use array to store 32 bytes from src image ( because its a integer to encode integer( 4 byte) need 32 bytes.

    fread( array,32,1,encInfo->fptr_src_image); 		//taking 32 bytes from src image
    encode_int_to_lsb( size, array);				// (size contain secret file extention ) calling int_to_lsb because need to encode 4 byte
    fwrite( array,32,1,encInfo->fptr_stego_image); 		//writeing the array to stego image

    return e_success;

}
Status encode_secret_file_extn(  char *file_extn ,EncodeInfo *encInfo)
{
    // int res=strlen(file_extn);
    //encode_data_to_image( file_extn,strlen(file_extn),encInfo->fptr_src_image, encInfo->fptr_stego_image);
    char buffer[8];    				
    int size=strlen( encInfo->extn_secret_file);           //used to store len as integer
    char data[size];						//used to store extention
    strcpy( data,encInfo->extn_secret_file);			//copying extention
    for( int i=0;i<size;i++)
    {
	fread( buffer,8,1,encInfo->fptr_src_image);		//copying src and store to buffer
	encode_byte_to_lsb( data[i] , buffer);			//callling function
	fwrite(buffer,8,1,encInfo->fptr_stego_image);         //writing it to stegoimage
    }

    return e_success;
}
Status encode_secret_file_size( long file_size, EncodeInfo *encInfo)
{
    char buffer[32];       			
    fread(buffer,32,1,encInfo->fptr_src_image);			
    encode_int_to_lsb( file_size,buffer);			//calling funtion
    fwrite( buffer,32,1,encInfo->fptr_stego_image);		//write it on stego image
    return e_success;
}
Status encode_secret_file_data( EncodeInfo *encInfo)
{

    int size=(encInfo->size_secret_file+1);   //to store size
    char data[size];				
    fseek(encInfo->fptr_secret,0,SEEK_SET); 	//seting filepointer of secret file to starting point
    fread( data,size,1,encInfo->fptr_secret);

    char buff[8];    			
    for( int i=0;i<size;i++)
    {
	fread( buff,8,1,encInfo->fptr_src_image);   
	encode_byte_to_lsb(data[i],buff);
	fwrite(buff,8,1,encInfo->fptr_stego_image);   //storing encode data to stego image
    }

    return e_success;
}

Status copy_remaining_img_data( FILE *fptr_src , FILE *fptr_dest)
{
    char ch;
    while( fread(&ch,1,1,fptr_src) > 0)    
	fwrite(&ch,1,1,fptr_dest);
    return e_success;
}

Status check_capacity( EncodeInfo *encInfo )  			//used to chek the capacity of the file 
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);    //finding size and store it into image capacity
    encInfo->size_secret_file=get_file_size( encInfo->fptr_secret); 		//finding size of secret file using get_file_size and store it into size secret file.
    if(( encInfo-> image_capacity ) > ( 54 + strlen(MAGIC_STRING) + (sizeof(int )*8) + (sizeof(".txt") *8) + (sizeof(int) *8) +( ( encInfo->size_secret_file)*8 )) )   // checking the capacity
    {
	printf("size of image_capacity is -gt  secret_fname\n");
	return e_success;								
    }
    else
    {
	printf("size of image_capacity is -lt secret_fname\n");
	return e_failure;

    }

}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
