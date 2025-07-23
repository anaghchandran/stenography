#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>

int main( int argc,char *argv[])
{
    int result;                          			//used to store the result after encode/decode
    result = check_operation_type(argv);			//function calling
    if( result == 0)
    {
	printf("\nEncoding selected\n");
	EncodeInfo encInfo;
	int result1 = read_and_validate_encode_args( argv, &encInfo);
	if( result1 == 0)
	{
	    printf("\nextention validation success\n");
	}
	else if( result1 == 1)
	{
	    printf("extention validation failed\n");
	    printf("provide cla as:\n./a.out file_name.bmp secert.txt <defualt.bmp>\n");
	    return -1;
	}
	if( do_encoding(&encInfo) == e_success)
	{
	    printf("\nencoding successfull\n\n");
	}
	else
	{
	    printf("\nencoding failed\n");
	}
    }
    else if( result == 1)
    {
	printf("\nDecoding selected\n");
	DecodeInfo decInfo;

	if( read_and_validate_decode_args ( argv, &decInfo) == d_success)
	{
	    printf("\nextention validation success\n");
	    do_decode( &decInfo);
	}
	else
	{
	    printf("\nextention validation failed\n");
	    return -1;
	}

    }
    else
    {
	printf("\nInvalid cla \n");
	printf("pass cla as follow:\n./a.out -e/-d file_name.bmp secret.txt <default.bmp>\n");
    }
    return 0;
}
OperationType check_operation_type(char *argv[] )
{
    if( (strcmp( argv[1],"-e") == 0) )				//using strcmp to check after ./a.out is -e 
    {
	return e_encode;                                       // it will return 0
    }
    else
	if( (strcmp( argv[1],"-d") ==0) )			//checking its -d
	{
	    return e_decode;
	}
	else							//other then -e or -d 
	{
	    return e_unsupported;
	}
}


