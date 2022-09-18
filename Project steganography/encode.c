#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encinfo )
{
    if(strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
            encinfo -> src_image_fname = argv[2];
    }
    else
        return e_failure;

     if(strcmp(strstr(argv[3],"."),".txt") == 0)
    {
            encinfo -> secret_fname = argv[3];
    }
    else
        return e_failure;
    
    if(argv[4] != NULL)
    {
        encinfo -> stego_image_fname =argv[4];
    }
    else    
        encinfo -> stego_image_fname = "stego.bmp";

    return e_success;
}
 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int) with pointer at 18th byte
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return (width * height * 3);
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_enc(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r"); //fopen((*encInfo).src_image_fname,r)
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r"); //fopen((*encInfo).secret_fname,r)
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w"); //fopen((*encInfo).stego_image_fname,r)
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
uint get_file_size(FILE *fptr_secret)
{
        fseek(fptr_secret, 0 ,SEEK_END);
        return ftell(fptr_secret);
}
//=========================================================================================
Status check_capacity(EncodeInfo *encinfo)
{   
    encinfo->image_capacity = get_image_size_for_bmp(encinfo->fptr_src_image );
    encinfo->size_secret_file =  get_file_size(encinfo-> fptr_secret);

    //If Condition met, returning  e_success
    if(encinfo->image_capacity > (54 +(2+4+4+4+encinfo->size_secret_file)*8))
        return e_success;   
    //If Condition not met, returning  e_failure    
    else
        return e_failure;       

}
//=========================================================================================
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    //fread - .bmp is not human readable
    fseek(fptr_src,0,SEEK_SET);
    char str[54];
    fread(str,sizeof(char),54,fptr_src);
    fwrite(str,sizeof(char),54,fptr_stego);
    return e_success;
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
    //#--0010 0011
    //fetch 1 bit at a time and encode in the byte of image buffer

    unsigned int mask = 1 << 7;
    for(int i = 0; i< 8; i++)
    {
        //data & mask ==will give the MSB bit
        //bring the MSB to LSB
        //(data & mask) >> 7 
        //fetch first byte from image buffer[i]
        //put the 1 bit in LSB of image buffer
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & (mask>> i))>>(7-i));  
    /*  image_buffer[i] = (image_buffer[i] & 0xFE) |((data & mask) >> (7-i));
        mask = mask >> 1;  */

    } 
    return e_success;

}
Status encode_size_to_lsb(char *buffer,int data)
{
    unsigned int mask = 1 << 7;
    for(int i = 0;i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) |((data & mask) >> (7-i));
        mask = mask >> 1;   
    }
    return e_success;
}
Status encode_size(int size, FILE *src, FILE *stego)
{
    //4bytes to encode
    char str[32];
    fread(str,32,sizeof(char), src);
    encode_size_to_lsb(str,size);
    fwrite(str,32,sizeof(char),stego);
    return e_success;
} 
Status encode_data_to_image(const char *data,int size,FILE *fptr_src_img,FILE *fptr_stego_image,EncodeInfo *encinfo)
{
    //call encode byte to lsb
    //each time pass 1byte data along with 8byte beautiful.bmp
    for(int i=0;i<size;i++)
    {   
        //read 8bytes from beautiful.bmp
        fread(encinfo->image_data,sizeof(char),8,fptr_src_img);
        encode_byte_to_lsb(data[i],encinfo->image_data);
        fwrite(encinfo->image_data,sizeof(char),8,fptr_stego_image);     
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string,EncodeInfo *encinfo)
{
    //every encoding needs to call encode_data to image
    encode_data_to_image(magic_string,strlen(magic_string),encinfo->fptr_src_image,encinfo->fptr_stego_image,encinfo);
    return e_success;

}
Status encode_secret_file_extn(const char *file_ext, EncodeInfo *encinfo)
{
    file_ext = ".txt";
    encode_data_to_image(file_ext,strlen(file_ext),encinfo->fptr_src_image,encinfo->fptr_stego_image,encinfo);
    return e_success;
}
Status encode_secret_file_size(long int size, EncodeInfo *encinfo)
{
      //4bytes to encode
    char str[32];
    fread(str,32,sizeof(char), encinfo->fptr_src_image);
    encode_size_to_lsb(str,size);
    fwrite(str,32,sizeof(char),encinfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encinfo)
{
    char ch;
    //bring the secret file pointer to the first position
    fseek(encinfo->fptr_secret,0,SEEK_SET);
    for(int i=0;i<encinfo->size_secret_file;i++)
    {
        fread(encinfo->image_data, 8,sizeof(char), encinfo->fptr_src_image);
        fread(&ch, 1, sizeof(char), encinfo->fptr_secret);
        encode_byte_to_lsb(ch,encinfo->image_data);
        fwrite(encinfo->image_data,8,sizeof(char),encinfo->fptr_stego_image);
        
    }
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_stego)
{
    char ch;
    while(fread(&ch,1,1,fptr_src) > 0)
    {
        fwrite(&ch,1,1,fptr_stego);
    }
    return e_success;

}
Status do_encoding(EncodeInfo *encinfo)
{
    //call rest of the function
    //open the files
    if(open_files_enc(encinfo) == e_success)    //Address of structure encinfo is passed
    {
        printf("Opened file succesfully\n");
        printf("Started Encoding\n");

        if(check_capacity(encinfo) == e_success)
        {
            printf("Secret data can be encoded in .bmp\n");
            //copy 54 bytes header
            if(copy_bmp_header(encinfo->fptr_src_image,encinfo->fptr_stego_image)== e_success)
            {
                printf("Copied Header successfully\n");
                if(encode_magic_string(MAGIC_STRING,encinfo)== e_success)
                {
                        printf("Magic string encoded succesfully\n");
                        //Encode secret file extension
                        if(encode_size(strlen(".txt"),encinfo->fptr_src_image ,encinfo->fptr_stego_image) == e_success)
                        {
                            printf("Encoded secret file size extension succesfully\n");
                            if(encode_secret_file_extn(encinfo->extn_secret_file,encinfo) == e_success)
                            {
                                printf("Encoded extension succesfully\n");
                                if(encode_secret_file_size(encinfo->size_secret_file,encinfo)==e_success)
                                {
                                    printf("Encoded secret file size\n");
                                    if(encode_secret_file_data(encinfo)== e_success)
                                    {
                                        printf("Encoded secret data succesfully: \n");
                                        if(copy_remaining_img_data(encinfo->fptr_src_image,encinfo->fptr_stego_image)==e_success)
                                        {
                                            printf("Copied remaining data...\n");
                                            return e_success;

                                        }
                                        else
                                        {
                                            printf("Failed Copying remaining data..\n");
                                            return e_failure;
                                        }                                       

                                    }
                                    else
                                    {
                                        printf("Failure: Encoded secret data \n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failure: Encoded secret file size\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failure: Encoded extension\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failure: encoding secret file extension\n");
                            return e_failure;
                        }
                }
                else
                {
                        printf("Failure: Encoding magic string\n");
                        return e_failure;
                }
            }
            else
                printf("Failure:copying Header\n");
                return e_failure;
        }
        else
        {
            printf("Encoding is not possible\n");
            return e_failure;
        }
    
    }
    else
    {
        printf("Failure : Opening Files\n");
        return e_failure;
    }
    return e_success;
}
