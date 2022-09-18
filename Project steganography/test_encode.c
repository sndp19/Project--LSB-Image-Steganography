#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    

    //CHecking the operation type -e or -d
    if(check_operation_type(argv) == e_encode)
    {
        EncodeInfo encinfo;

        printf("-----------------Selected Encoding----------------\n");
        //Read and Validate command line argument
        if(read_and_validate_encode_args(argv,&encinfo) == e_success)
        {
            printf("Read and Validate is successful :\n");
            if(do_encoding(&encinfo) == e_success)
            {
                printf("*Encoding is Completed*\n");
                printf("=======================================================================================================================\n");

            }
            else
                printf("Failure: Encoding is not completed\n");
        }
        else
              printf("Read and Validate is unsuccessful :\n");
    
    }    
    else if(check_operation_type(argv) == e_decode)
    {
        DecodeInfo decinfo;

        printf("----------------Selected Decoding------------------\n"); 
        printf("verify1\n");     
        
        //Read and Validate command line argument
        if(read_and_validate_decode_args(argv,&decinfo) == e_success)
        {

            printf("Read and Validate is successful :\n");
            if(do_decoding(&decinfo) == e_success)
            {
                printf("## Decoding Done Successfully ##\n");
                printf("=======================================================================================================================\n");

            }
            else
            {
                printf("ERROR Decoding is not completed\n");
            }
        }
    }
    else
    {
            printf("Invalid option \n*****Usage*****\n");
            printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
            printf("Decoding: ./a.out -d beautiful.bmp secret.txt stego.bmp\n");
    }

    return 0;
}


OperationType check_operation_type(char *argv[1])
{
    if(strcmp(argv[1],"-e") == 0)
        return e_encode;
    else if(strcmp(argv[1],"-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
