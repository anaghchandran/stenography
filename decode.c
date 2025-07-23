#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"
#include "types.h"


Status read_and_validate_decode_args( char *argv[] ,DecodeInfo *decInfo )
{
    if( strstr( argv[2] ,".bmp") )		//checking cla after ./a.out the file extention is .bmp
    {
	decInfo->stego_image_fname =argv[2];   // if .bmp the store it into stego_image_fname
    }
    else
    {
	return d_failure;        		//else stop
    }
    if( argv[3] ) 				//check output file passed by user
    {
	strcpy(decInfo->output_fname,argv[3]);     //store it in output fileneme
    }
    else
    {
	strcpy(decInfo->output_fname,"output"); //else use default name output
    }
    return d_success;
}
Status do_decode( DecodeInfo *decInfo)
{
    if(( open_file_decode( decInfo) == d_success))     //opening file  
    {
	printf("file open is success\n");
    }
    else
    {
	printf("file open failed\n");
    }


    if( decode_magic_string(decInfo) == d_success)    // decoding magic string
    {
	printf("decoding magic string success\n");
    }
    else
    {
	printf("decoding magic string failed\n");
	return d_failure;
    }


    if( decode_secret_file_extn_size(decInfo) == d_success)             //decoding secret file extention size
    {
	printf("decoding secret file extention size success\n");
    }
    else
    {
	printf("decoding secret file extention size failed\n");
    }

    if( decode_secret_file_extn( decInfo) == d_success)            //decoding secret file extention
    {
	printf("decoding secret file extention success\n");
    }
    else
    {
	printf("decoding secret file extetion failed\n");
    }

    if( decode_secret_file_data_size( decInfo) == d_success)      //decding secret file data size
    {
	printf("decoding secret file data size success\n");
    }
    else
    {
	printf("decoding secret file data size failed\n");
    }


    if( decode_secret_file_data( decInfo) == d_success)           //decoding secrect file data
    {
	printf("decoding secret data success\n");
    }
    else
    {
	printf("decoding secret data failed\n");
    }








    return d_success;
}
char decode_byte_from_lsb( char *buffer)    //to decode char 
{
    char data=0;                    
    for( int i=0;i<8;i++)      
    {
	if( buffer[i] &1)     //getting lsb 
	{
	    data=data|(1<<(7-i) );  // if lsb is 1 then only shift to required position
	}

    }
    return data;                  //return data 

}

int decode_int_from_lsb( char *buffer)    //used for int
{
    char data=0;
    for( int i=0;i<32;i++)
    {
	if(  buffer[i] &1)                      //getting lsb  
	{
	    data=data|(1<<(31-i) );               //if lsb is 1 then shift
	}
    }
    return data;
}
Status decode_secret_file_data_size( DecodeInfo *decInfo)   //decoding secret file data size
{

    char buffer[32];
    int size=0;
    fread( buffer,32,1,decInfo->fptr_stego_image);
    decInfo->secret_file_data_size=decode_int_from_lsb( buffer);  // function calling ->store it into secret file data size 

    printf("size of secret file :%d\n",decInfo->secret_file_data_size);    
    return d_success;
}
Status decode_secret_file_data( DecodeInfo *decInfo)   			//decoding secret file data 
{
    int size=(decInfo->secret_file_data_size);                        //storing size of secret data size
    char buffer[ size];
    char a[29];
    int i=0;
    for( i=0;i<size;i++)
    {
	fread(buffer,8,1,decInfo->fptr_stego_image);
	a[i]=decode_byte_from_lsb( buffer);    //calling and store it in a
    }
    a[i]='\0';
    fwrite(a,size,1,decInfo->fptr_output_image);   //copying to output image
    return d_success;
}


Status decode_secret_file_extn_size( DecodeInfo *decInfo)     // decoding secret file extention size
{
    //it is int value so size is 4
    char buffer[32];
    fread( buffer,32,1,decInfo->fptr_stego_image);                     //store bytes from stego image to buffer
    decInfo->secret_file_extn_size=decode_int_from_lsb( buffer);       //function callind and result is storing in secret file extention size
    printf("secret file extention size: %d \n",decInfo->secret_file_extn_size);
    return d_success;
}
Status decode_secret_file_extn( DecodeInfo *decInfo)    //decoding secret file extention
{
    int len=decInfo->secret_file_extn_size;     //taking secret file extention size
    char buffer[20];                            //buffer 
    int i=0;
    for( i=0;i<len;i++)
    {

	fread( buffer,8,1,decInfo->fptr_stego_image);                             //store directly to secret file extention
	decInfo->secret_file_extn[i]=decode_byte_from_lsb( buffer);
    }

    printf("secret file extention: %s\n",decInfo->secret_file_extn);                             //printing  extention
    strcat( decInfo->output_fname, decInfo->secret_file_extn);			//adding extention to output file
    printf("output file name: %s\n",decInfo->output_fname); 			//printing with extention
    decInfo->fptr_output_image=fopen( decInfo->output_fname,"w");             //open output file in write mode
    if( decInfo->fptr_output_image == NULL)
    {
	perror("fopen");
	return d_failure;
    }
    return d_success;
}


Status decode_magic_string( DecodeInfo *decInfo)   //decoding magic string
{
    char string[100];  				//used to store the magic string from user for validation	
    int magic_str_size=strlen(string);         //length of the input magic string
    char buffer[8];                            //to store the ecoded magic string  from stego
    char array[magic_str_size];               //used to store the decode magic string after function call 
    int i=0;					//for loop i 
    printf("enter the magic_string: ");        //reading magic string
    scanf("%s",string);					
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);  //moving file pointer
    for( i=0;i<strlen(string);i++)
    {
	fread( buffer,8,1,decInfo->fptr_stego_image);      //storing to buffer
	array[i]=decode_byte_from_lsb(buffer);           //after decode store it to array
    }
    array[i]='\0';                                       //adding null char
    if( strcmp(array,string) ==0)                   	//comparing the string and magicstring are same
    {
	printf("magic string validation success\n");
    }
    else
    {
	printf("magic string validation failed\n");
	return d_failure;

    }
    return d_success;
}

Status open_file_decode( DecodeInfo *decInfo)       //opening file  input file (stego)
{
    decInfo->fptr_stego_image= fopen(decInfo->stego_image_fname,"r");    // open in read mode and check it empty
    if( ( decInfo ->fptr_stego_image ) == NULL )
    {
	printf("error in opening\n");           
	return d_failure;
    }
    return d_success;
}


