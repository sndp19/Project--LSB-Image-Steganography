#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types



#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /*Source image file name*/
    char *stsrc_image_fname;
    FILE *fptr_src_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /*Decoding file
    char *decoded_fname;
    long *dec_file;
    long *dec_sec_file_size;*/

    /*Secret file info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    char dec_sec_file_ext[MAX_SECRET_BUF_SIZE];
    long dec_file_size[MAX_SECRET_BUF_SIZE];
    char dec_data[MAX_SECRET_BUF_SIZE];
    FILE *fptr_dec_data;
    
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    FILE *fptr_dec_mag_str;
    char magic_data[MAX_SECRET_BUF_SIZE];
    char dec_mag_str[MAX_SECRET_BUF_SIZE];
    char dec_mag_data[MAX_SECRET_BUF_SIZE];
    char dec_sec_file_ext_size[MAX_SECRET_BUF_SIZE];

} DecodeInfo;

/* Get File pointers for i/p and o/p files */
Status open_files_dec(DecodeInfo *decinfo);


/*Perfom Decoding */
Status do_decoding(DecodeInfo *decinfo);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decinfo);

 

/* Checking magic string exist or not for further proceeding */
Status decode_magic_string(const char *magic_string,DecodeInfo *decinfo);

/*Opening secret file */
Status open_secret_file(DecodeInfo *decinfo);

/*Decoding extension size*/
Status decode_sec_ext_size_fromlsb(char *decoding,char *dec_sec_ext_size,DecodeInfo *decinfo);

/*Decode secret file size from lsb*/
Status decode_sec_file_size_fromls(char *dec_sec_file_size,long *dec_file_size);


/*Decoding secret file size*/
Status decode_secret_file_size(DecodeInfo *decinfo);

/*Decoding secret file extension size*/
Status decode_secret_file_ext_size(DecodeInfo *decinfo);

/*Decoding bytes from lsb*/
Status decode_byte_fromlsb(char *stego_img_data,char *decode_magic_string,long int position,DecodeInfo *decinfo);

/*Decoding magic string bytes from image*/
Status decode_magic_str_bytes_from_image(const char *data,long int size,FILE *fptr_stego_img, DecodeInfo *decinfo);


/*Decoding file data from lsb*/
Status decode_file_data_fromlsb(char *data,char *dec_data,int position,DecodeInfo *decinfo);

/*decoding secret file*/
Status decode_secret_file(DecodeInfo *decinfo);

#endif