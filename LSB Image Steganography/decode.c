#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>

/* Function defination*/

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	//validate for .bmp file.
	if(strcmp(strstr(argv[2], "."), ".bmp") != 0)
	{
		printf("Error : Pass Decodable file with extention as .bmp\n");
		return e_failure;
	}
	decInfo -> dec_src_image_fname = argv[2];

	if(argv[3] != NULL)
	{

		if(strchr(argv[3], 46) == NULL)
		{
			strcpy(decInfo -> dec_secret_fname, argv[3]);
		}
		else
		{
			printf("Error : Do not pass any extention for OUTPUT file.\n");
			return e_failure;
		}
	}
	else
	{
		strcpy(decInfo -> dec_secret_fname, "File_secret_info");
	}

	return e_success;
}

/* Decoding of the image file*/
Status do_decoding(DecodeInfo *decInfo)
{
	//function call for open source file
	if(open_src_file(decInfo) == e_success)
	{
		printf("Opening source file........\n");

		//read the password.
		printf("Enter the file PASSWORD : ");
		scanf("%s", decInfo -> password);
		printf("\n");

		//function call decode magic string length
		if(decode_magic_string_len(decInfo) == e_success)
		{
			printf("--------Magic string length DECODED SUCCCESSFULLY.\n\n");

			//compare magic len and password len
			if(decInfo -> d_magic_len == strlen(decInfo -> password))
			{
				printf("--------Magic string length and PASSWORD length MATCHED SUCCESSFULLY.\n\n");

				//Decode magic string
				if(decode_magic_string(decInfo) == e_success)
				{
					printf("--------Magic string DECODED SUCCESSFULLY.\n\n");


					if(strcmp(decInfo -> d_magic_string, decInfo -> password) == 0)
					{
						printf("--------Magic string and PASSWORD MATCHED SUCCESSFULLY.\n\n");

						//Decode Extention size
						if(decode_secret_file_extn_size(decInfo) == e_success)
						{
							printf("--------Extention size DECODED SUCCESSFULLY.\n\n");

							//Decode file extention
							if(decode_secret_file_extn(decInfo) == e_success)
							{
								printf("--------File Extention DECODED SUCCESSFULLY.\n\n");
								
								//Do STRCAT to SecretInfo file
								strcat(decInfo -> dec_secret_fname, decInfo -> file_extn);
							
								//open the secret_info file
								if(open_dest_file(decInfo) == e_success)
								{
									printf("%s Opeing Destination File........\n\n", decInfo -> dec_secret_fname);

									//function call for decode file size
									if(decode_size_secret_file(decInfo) == e_success)
									{
										printf("--------Secret File size DECODED SUCCCESSFULLY.\n\n");

										//Decode Secret file data
										if(decode_secret_file_data(decInfo) == e_success)
										{
											printf("--------Secret file DECODED SUCCCESSFULLY.\n\n");
										}
										else
										{
											printf("Error : Secret file DECODING FAILS\n");
											return e_failure;
										}
									}
									else
									{
										printf("Error : Secret File size DECODING FAILS.\n");
									}
								}
								else
								{
									return e_failure;
								}	
							}
							else
							{
								printf("Error : File Extention DECODING FAILS.\n");
								return e_failure;
							}
						}
						else
						{
							printf("Error : Extention size DECODING FAILS.\n");
							return e_failure;
						}
					}
					else
					{
						printf("####@ WRONG PASSWORD @####\n\n");
						return e_failure;
					}

				}
				else
				{
					printf("Error : Magic string DECODING FAILS.\n");
					return e_failure;	
				}	
			}
			else
			{
				printf("####@ WRONG PASSWORD @####\n\n");
				return e_failure;
			}
		}
		else
		{
			printf("Error : Magic string length DECODING FAILS.\n");
			return e_failure;
		}
		return e_success;

	}
	else
	{
		return e_failure;
	}
}


/* Get file pointer for i/p*/
Status open_src_file(DecodeInfo *decInfo)
{
	//image .bmp file
	decInfo -> fptr_dec_src_image = fopen(decInfo -> dec_src_image_fname, "r");

	//error checking
	if(decInfo -> fptr_dec_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open SOURCE file %s\n", decInfo -> dec_src_image_fname);

		return e_failure;
	}

	//No failure return success
	return e_success;
}

/* Get file pointer for o/p*/
Status open_dest_file(DecodeInfo *decInfo)
{
	//image .txt, .c or .sh file
	decInfo -> fptr_secret_file = fopen(decInfo -> dec_secret_fname, "w");

	//error checking
	if(decInfo -> fptr_secret_file == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open DESTINATION file %s\n", decInfo -> dec_secret_fname);

		return e_failure;
	}

	//No failure return success
	return e_success;
}


/* Decode 4 bytes of data*/
int decode_lsb_to_int(DecodeInfo *decInfo)
{
	//creat a buffer read the data from src file
	char buffer[sizeof(int) * 8];
	fread(buffer, sizeof(int) * 8, 1, decInfo -> fptr_dec_src_image);

	//featch 32 bits from the buffer data of32 bytes
	int data = 0;
	for(int i = 0; i < 32; i++)
	{
		data <<= 1;
		if(buffer[i] & 1)
		{
			data |= 1;
		}
	}
	return data;
}

/* Decode 8 bits from src file of 8 bytes*/
char decode_lsb_to_byte(char *image_buffer)
{
	char ch = 0;
	for(int i = 0; i < 8; i++)
	{
		ch = ch << 1;
		if(image_buffer[i] & 1)
		{
			ch |= 1;
		}
	}
	return ch;
}

/*Decode magic string length*/
Status decode_magic_string_len(DecodeInfo *decInfo)
{
	//featch the data from 54th position.
	fseek(decInfo -> fptr_dec_src_image, 54, SEEK_SET);

	//fuction call for int get int value
	decInfo -> d_magic_len = decode_lsb_to_int(decInfo);

	//return e_success or e_failure
	return(ftell(decInfo -> fptr_dec_src_image) == (54 + sizeof(int) * 8)) ? e_success : e_failure;
}

/* Decode magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
	int index = 0;
	for(int i = 0; i < decInfo -> d_magic_len; i++)
	{
		//creat a buffer and featch the 8 bytes of data from .bmp file
		char buffer[8];
		fread(buffer, 8, 1, decInfo -> fptr_dec_src_image);

		//call the function decode 1 byte at a time
		decInfo -> d_magic_string[index] = decode_lsb_to_byte(buffer);
		index++;	
	}
	decInfo -> d_magic_string[index] = '\0';

	//	printf("m_string = %s\n", decInfo -> d_magic_string);

	return (ftell(decInfo -> fptr_dec_src_image) == (54 + sizeof(int)*8 + decInfo -> d_magic_len*8)) ? e_success : e_failure;

}

/* Decode Secret file extention*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	//Function call for decode extention size
	decInfo -> d_extn_file_size = decode_lsb_to_int(decInfo);

	//printf("file extn size = %d\n", decInfo -> d_extn_file_size);
	//return e_success or e_failure
	return (ftell(decInfo -> fptr_dec_src_image) == (54 + (sizeof(int)*8) * 2 + decInfo -> d_magic_len*8)) ? e_success : e_failure;

}

/* Decode File Extention*/
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	int index = 0;
	for(int i = 0; i < decInfo -> d_extn_file_size; i++)
	{
		//creat a buffer and featch the 8 bytes of data from .bmp file
		char buffer[8];
		fread(buffer, 8, 1, decInfo -> fptr_dec_src_image);

		//fuction call for decodeing extention
		decInfo -> file_extn[index] = decode_lsb_to_byte(buffer);
		index++;
	}
	decInfo -> file_extn[index] = '\0';

	//return e_success or e_failure
	return (ftell(decInfo -> fptr_dec_src_image) == (54 + (sizeof(int)*8) * 2 + decInfo -> d_magic_len*8 + decInfo -> d_extn_file_size * 8)) ? e_success : e_failure;

}

/* Decode secret information size*/
Status decode_size_secret_file(DecodeInfo *decInfo) 
{
    // create buffer
    char buffer[sizeof(long) * 8];
    fread(buffer, sizeof(long) * 8, 1, decInfo -> fptr_dec_src_image);

    // Decode long int for file size
    decInfo -> size_file = 0;
    for(int i = 0; i < sizeof(long) * 8; i++) 
	{
		decInfo -> size_file = decInfo -> size_file << 1;

		if(buffer[i] & 1) 
		{
			decInfo -> size_file |= 1;
		} 
	}

	// return e_success or e_failure
	return (ftell(decInfo -> fptr_dec_src_image) == (54 + (sizeof(int) * 8) * 2 + sizeof(long) * 8 + decInfo -> d_magic_len * 8 + decInfo -> d_extn_file_size * 8)) ? e_success : e_failure;
}


/* Decode secret Infomation*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	for(int i = 0; i < decInfo -> size_file; i++)
	{
		char src_buffer[8];
		fread(src_buffer, 8, 1, decInfo->fptr_dec_src_image); 
    	
		// Function call for decoding extension
    	char ch = decode_lsb_to_byte(src_buffer);
    	fputc(ch, decInfo->fptr_secret_file);
	}

	//return e_success or e_failure
	return (ftell(decInfo -> fptr_secret_file) == decInfo -> size_file) ? e_success : e_failure;


}
