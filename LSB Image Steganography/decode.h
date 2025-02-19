#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

#define MAX_FILE_SUFFIX 4
#define MAX_FILE_NAME 30

typedef struct _DecodeInfo
{
	/*Decode src file Info*/
	char *dec_src_image_fname; //store the .bmp file name.
	FILE *fptr_dec_src_image;

	/*Magic string Info*/
	char password[10];
	char d_magic_string[10];
	int d_magic_len;

	/*secret File info*/
	char dec_secret_fname[MAX_FILE_NAME]; //stores secret file name.
	FILE *fptr_secret_file;
	int d_extn_file_size;
	char file_extn[MAX_FILE_SUFFIX];
	long size_file;


}DecodeInfo;

/*Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Decoding function*/
Status do_decoding(DecodeInfo *decInfo);

/* Open the source file*/
Status open_src_file(DecodeInfo *decInfo);

/* Open the destination file*/
Status open_dest_file(DecodeInfo *decInfo);

/* Decode magic string lenght*/
Status decode_magic_string_len(DecodeInfo *decInfo);

/* Decode magic string*/
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extention size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extention*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode size of secret file*/
Status decode_size_secret_file(DecodeInfo *decInfo);

/* Decode secret file DATA*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte of data*/
char decode_lsb_to_byte(char *image_buffer);

/* Decode 4 bytes of data*/
int decode_lsb_to_int(DecodeInfo *decInfo);

#endif
