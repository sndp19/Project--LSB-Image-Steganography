#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/*
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
*/
/* Function Definitions */


Status open_files_dec(DecodeInfo *decinfo)
{
    decinfo->fptr_stego_image = fopen(decinfo->stego_image_fname, "r"); //fopen((*encInfo).src_image_fname,r)
    // Do Error handling
    if (decinfo->fptr_stego_image == NULL) //If File doesn't exist
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->stego_image_fname);

    	return e_failure;
    }
     
    return e_success;

}


/*====================*/



Status read_and_validate_decode_args(char *argv[], DecodeInfo *decinfo)
{     
    if(strcmp(strstr(argv[2], "."),".bmp") == 0)
    {
        decinfo->stego_image_fname=argv[2];
         
    }
    else 
    {
        return e_failure;
    }
    if(argv[3] != NULL)
    {
        if(strcmp(strstr(argv[3],"."),".txt") == 0)
        {
            decinfo->secret_fname =argv[3];
             ;
        }
        else
        {
            decinfo->secret_fname = "decode.txt";
             
        }
    }
    else if(argv[3] == NULL)
    {
        decinfo->secret_fname = "decode.txt";
        
    }
    return e_success; 

}
 
 /*=============================================================*/

Status decode_file_data_fromlsb(char *data,char *dec_data,int position,DecodeInfo *decinfo)
{
    int mask = 0x01;
    *dec_data = 0;
    for(int i=0; i < 8; i++)
    {
        *dec_data = *dec_data | ((data[i] & mask) << (7-i));
    }
    //decoded_data = *dec_data;
    return e_success;
}

/*=============================================================*/

Status decode_secret_file(DecodeInfo *decinfo)
{
    int size,i;
    size = decinfo -> dec_file_size[0];
    char data[8];
    for(i=0; i<size; i++)
    {
        fread(data,sizeof(char), 8, decinfo -> fptr_stego_image);
        decode_file_data_fromlsb(data,decinfo -> dec_data,i,decinfo);
        fwrite(decinfo ->dec_data, sizeof(char), 1,decinfo->fptr_secret);

    }
    return e_success;
}

/*=============================================================*/

Status decode_sec_file_size_fromlsb(char *dec_sec_file_size,long *dec_file_size)
{
    int mask = 0x1;
    *dec_sec_file_size = 0;
    for(int i =0; i< 32; i++)     
        *dec_file_size = * dec_sec_file_size | ((dec_sec_file_size[i] & mask));
    
    return e_success;
}

/*=============================================================*/

Status decode_secret_file_size(DecodeInfo *decinfo)
{
    char dec_sec_file_size[32];
    fread(dec_sec_file_size, sizeof(char), 32, decinfo -> fptr_stego_image);
    decode_sec_file_size_fromlsb(dec_sec_file_size, decinfo -> dec_file_size);
    return e_success;
}

/*=============================================================*/

Status open_secret_file(DecodeInfo *decinfo)
{
  
    char *ext = malloc(50);
    char *out = malloc(50);
    char *name = malloc(50);
    strcpy(name,decinfo->secret_fname);
    strcpy(ext,decinfo->dec_sec_file_ext);
    out = strcat(name,ext);
    decinfo->secret_fname = out;
    decinfo->fptr_secret = fopen(decinfo->secret_fname,"w");

    //Do Error handling
    if (decinfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "{ERROR:Unable to open file %s\n",out);
        return e_failure;
    }
    
    free(ext);
    free(name);
    free(out);
    return e_success;
}

/*=============================================================*/

Status decode_sec_ext_size_fromlsb(char *decoding,char *dec_sec_ext_size,DecodeInfo *decinfo)
{
    int mask = 0x1;
    *dec_sec_ext_size = 0;
    for(int i=0; i < 32; i++)
    {
        *dec_sec_ext_size = *dec_sec_ext_size | ((decoding[i] & mask) << (31-i));
    }
    return e_success;
}

/*=============================================================*/

Status decode_secret_file_ext_size(DecodeInfo *decinfo)
{
    char dec_sec_ext_size[32];
    fread(dec_sec_ext_size, sizeof(char), 32,decinfo -> fptr_stego_image);
    decode_sec_ext_size_fromlsb(dec_sec_ext_size,decinfo->dec_sec_file_ext_size,decinfo);
    if((decinfo->dec_sec_file_ext_size[0]) == 4)
        return e_success;
}

/*=============================================================*/

Status decode_magic_str_bytes_from_image(const char *data,long int size,FILE *fptr_stego_image, DecodeInfo *decinfo)
{
    printf("size = %ld",size);
    fseek(decinfo->fptr_stego_image,54,SEEK_SET);
    for(int i=0; i<size;i++)
    {
        fread(decinfo->magic_data,sizeof(char), 8,fptr_stego_image);
        decode_byte_fromlsb(decinfo->magic_data,decinfo->dec_mag_str,i,decinfo);
    }
    return e_success;
}

/*=============================================================*/
Status decode_magic_string(const char *magic_string,DecodeInfo *decinfo)
{
    int i;
    printf("magic string  =  %s",magic_string);
    decode_magic_str_bytes_from_image(magic_string,strlen(magic_string),decinfo->fptr_stego_image,decinfo);
    for(i=0; i< strlen(decinfo->dec_mag_str)-1;i++)
    {
        char temp;
        temp = decinfo->dec_mag_str[i];
        printf("temp=%c",temp);
        decinfo->dec_mag_str[i] = temp;
    }
    decinfo->dec_mag_str[i] ='\0';

    if(strcmp(decinfo->dec_mag_str,"#*") == 0)
   
    return e_success;
}

/*=============================================================*/

Status decode_byte_fromlsb(char *stego_img_data,char *decode_string,long int position,DecodeInfo *decinfo)
{
    int mask =0x1;
    for(int i=0;i<8;i++)
    {
        *decode_string = *decode_string | ((stego_img_data[i] & mask) << (7-i));
    }
    decinfo -> dec_mag_data[(char)position] = *decode_string;
    printf("dec_magic_str: %d", *decode_string);
    for(int i=0;i<2;i++)
    {
        decode_string[position] = *decode_string;
        printf("dec_inside: %d\n",decode_string[position]);
    }

    return e_success;
}

/*=============================================================*/

Status do_decoding(DecodeInfo *decinfo)
{
    printf("INFO: Started Decoding Procedure...\n");
    if(open_files_dec(decinfo) == e_success)
    {
        printf("INFO: Opened file successfully...\n");
        if(decode_magic_string(MAGIC_STRING,decinfo) == e_success)
        {
            printf("INFO: Magic sting is decoded successfully..\n");
            if(decode_secret_file_ext_size(decinfo) == e_success) 
            {
                printf("INFO: Secret file extension is successfully Decoded\n");
                if(open_secret_file(decinfo) == e_success)
                {
                    printf("INFO:Opened File to store decoded content successfully\n");
                    printf("INFO: Opened All Files required for decoding\n");
                    if(decode_secret_file_size(decinfo) == e_success)
                    {
                        printf("INFO: Crypting secret file \n");
                        if(decode_secret_file(decinfo)==e_success)
                        {
                             return e_success;
                        }
                        else
                        {
                            printf("ERROR: Cannot Decode encrypted Data");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Decoding secret file size is unsuccessfull \n");
                        return e_failure;
                    }
                    
                }
            }           
        }
        else 
            return e_failure;
    }
}
/*
/*

Status open_files_dec(DecodeInfo *decinfo)
{
    decinfo->fptr_stego_image = fopen(decinfo->stego_image_fname, "r"); //fopen((*encInfo).src_image_fname,r)
    // Do Error handling
    if (decinfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->stego_image_fname);

    	return e_failure;
    }
    else
        return e_success;
}


Status decode_magic_string(char *magic_string, DecodeInfo *decinfo)
{
    int i;
    decode_magic_str_byte_from_image(magic_string,strlen(magic_string),)
    fseek(decinfo->fptr_stego_image,54,SEEK_SET);
    
   fread(ch,sizeof(char),32,decinfo->fptr_stego_image);
    for(int k=0;k<2;k++)
    {   
        char temp;
        for(int i=0;i<32;i++)
        {
            unsigned int mask = 1 ;
            mask = ch[i] & mask;
            for(int j=0;j<8;j++)
            {
                key[k] = key[k] | (mask <<(7-j));
            }
        }   
    }
    key[2] ='\0';
    
    printf("%s key string in function\n",key);
    
    if(strcmp(key,"#*") == 0)
        return e_success;
    else
        return e_failure;
}
Status decode_magic_string(char *verify,const char *magic_string, DecodeInfo *decinfo)
{
    char ch[32],key[3];
    fseek(decinfo->fptr_stego_image,54,SEEK_SET);
    
   fread(ch,sizeof(char),32,decinfo->fptr_stego_image);
    for(int k=0;k<2;k++)
    {   
        char temp;
        for(int i=0;i<32;i++)
        {
            unsigned int mask = 1 ;
            mask = ch[i] & mask;
            for(int j=0;j<8;j++)
            {
                key[k] = key[k] | (mask <<(7-j));
            }
        }   
    }
    key[2] ='\0';
    
    printf("%s key string in function\n",key);
    
    if(strcmp(key,"#*") == 0)
        return e_success;
    else
        return e_failure;
} 
Status do_decoding(DecodeInfo *decinfo)
{
    printf("verify\n");
    if(open_files_dec(decinfo) == e_success)
    {
        printf("verify\n");
        char verify[3];
        printf("Opened file succesfully\n");
        printf("Enter Key to Decode the message\n");
        scanf(" %s",verify);
        printf("Entered key is %s\n",verify);
        if(decode_magic_string(verify,MAGIC_STRING,decinfo) == e_success)
        {
            printf("Key String Matched\n");
            printf("Started Decoding..\n");            
        }
        else 
            return e_failure;
    }
}


*/