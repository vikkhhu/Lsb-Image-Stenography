#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

/* Function Definitions */
OperationType check_operation_type(char *argv[])
{
	//checking argv 1 is -e or -d
	if(strcmp(argv[1], "-e") == 0)
	{
		//return e_encode
		return e_encode;
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		//retuen e_decode
		return e_decode;
	}
	else
	{
		//no match return e_unsupported
		return e_unsupported;
	}
}
/*Read and validate encode args
 *input: argv, Struct variable address
 *output: assigning file name to pointer
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	//validate for .bmp file
	if(strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		encInfo->src_image_fname = argv[2];
	}
	else
	{
		printf("Error : Pass encodeable file with extention as .bmp\n");
		return e_failure;
	}

	//validate for .txt, .c and .sh file
	if(strcmp(strstr(argv[3], "."), ".txt") == 0)
	{
		encInfo->secret_fname = argv[3];
	}
	else if(strcmp(strstr(argv[3], "."), ".c") == 0)
	{
		encInfo->secret_fname = argv[3];
	}
	else if(strcmp(strstr(argv[3], "."), ".sh") == 0)
	{
		encInfo->secret_fname = argv[3];
	}
	else
	{
		printf("Error : Pass secret file with extention .c, .txt or .sh\n");
		return e_failure;
	}

	//chacking for 4th argument 
	if(argv[4] != NULL)
	{
		//validating .bmp file
		if(strcmp(strstr(argv[4], "."), ".bmp") == 0)
		{
			encInfo->stego_image_fname = argv[4];
		}
		else
		{
			printf("Error : Pass output file with extention as .bmp\n");
			return e_failure;
		}
	}
	else
	{
		//o/p file default name
		encInfo->stego_image_fname = "Default.bmp";
	}

	return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
	{
		printf("Opening File........\n\n");

		//read the magic string
		printf("Enter the Magic string : ");
		scanf("%s", encInfo->magic_string);
		printf("\n");

		printf("------------------------------------------------------------------------\n");
		//check for the source file capacity
		if(check_capacity(encInfo) == e_success)
		{
			printf("--------Capacity checking SUCCESSFUL, HAVE ENOUGH CAPACITY.\n");
			printf("------------------------------------------------------------------------\n\n");

			//copy the header of the bmp file
			if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
			{
				printf("--------Coping SOURCE header file is SUCCESSFUL.\n\n");

				//Encode magic string length
				if(encode_magic_string_len(encInfo -> m_string_len, encInfo)== e_success)
				{
					printf("--------Magic string length ENCODED SUCCESSFUL.\n\n");

					//encode magic string
				   	if(encode_magic_string(encInfo -> magic_string, encInfo) == e_success)
					{
						printf("--------Magic string ENCODED SUCCESSFUL.\n\n");

						// Encode secret file extn size.
						if(encode_secret_file_extn_size(encInfo -> extn_size, encInfo) == e_success)
						{
							printf("--------Secret file extantion size ENCODED SUCCESSFUL.\n\n");

							//Encode secret file extn.
							if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
							{
								printf("--------Secret file extention ENCODED SUCCESSFUL.\n\n");

								//Encode secret file size.
								if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
								{
									printf("--------Secret file size ENCODED SUCCESSFUL.\n\n");

									//Encode secret file data.
									if(encode_secret_file_data(encInfo) == e_success)
									{
										printf("--------Secret file data ENCODED SUCCESSFUL.\n\n");

										//Copy remaining data of source file
										if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
										{
											printf("--------Coping SOURCE FILE remaining DATA SUCCESSFUL.\n\n");
										}
										else
										{
											printf("Error : Coping SOURCE FILE remaining DATA FAILS.\n");
										}
									}
									else
									{
										printf("Error : Secret file data ENCODING FAILS.\n");
									}
								}
								else
								{
									printf("Error : Secret file size ENCODING FAILS.\n");
								}
							}
							else
							{
								printf("Error : Secret file extention ENCODING FAILS.\n");
							}
						}
						else
						{
							printf("Error : Secret file extantion size ENCODING FAILS.\n");
						}
					}	
					else
					{
						printf("Error : Magic string ENCODING FAILS.\n");
					}
				}
				else
				{
					printf("Error : Magic string length ENCODING FAILS.\n");
				}
			}
			else
			{
				printf("Error : Coping SOURCE HEADER FILE FAILS.\n");
			}
		}
		else
		{
			printf("Error : Capacity is NOT ENOUGH SPACE TO ENCODE.\n");

		}
		return e_success;
	}
	else
	{
		return e_failure;
	}

}

Status check_capacity(EncodeInfo *encInfo)
{
	//size of the image .bmp file
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_secret);
	printf("image_capacity = %u\n", encInfo -> image_capacity);

	//find the length of magic string
		encInfo -> m_string_len = strlen(encInfo -> magic_string);
	printf("m_string_len = %u\n", encInfo -> m_string_len);

	//find the secret file extn size
	encInfo -> extn_size = strlen(strstr(encInfo -> secret_fname, "."));
	printf("Extention size = %u\n", encInfo -> extn_size);

	//store secret file extention name
	strcpy(encInfo -> extn_secret_file, strstr(encInfo -> secret_fname, "."));

	printf("Secret file extn : %s\n", encInfo -> extn_secret_file);

	//find secret file size
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
	printf("Size of secret file = %ld\n", encInfo -> size_secret_file);

	//return e_success or e_failure
	return (encInfo -> image_capacity > (54 + (sizeof(int) * 8) * 2 + (encInfo -> m_string_len * 8) + (encInfo -> size_secret_file * 8))) ? e_success : e_failure;

}

/*get the secret file size
 *input: secret file ptr
 *output: file size in bytes
 */
uint get_file_size(FILE  *fptr)
{
	fseek(fptr, 0, SEEK_END);
	return ftell(fptr);
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
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

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
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

/* 
 * Get the header file data
 * Inputs: Src Image file and Stego Image file
 * Output: copying to stego_image_fname
 * Return Value: e_success or e_failure
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{

	//to move file pointer to 0th position 
	rewind(fptr_src_image);
	char buffer[54];

	//reading from src file storing in buffer
	fread(buffer, 54, 1, fptr_src_image);

	//writting to destnation file
	fwrite(buffer, 54, 1, fptr_dest_image);
	

	//return of the function
	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n",ftell(fptr_src_image), ftell(fptr_dest_image));
	return ftell(fptr_dest_image) == ftell(fptr_src_image) ? e_success : e_failure;
}

/* 
 * Encode 1 byte of data to lsb
 * Inputs: character of 1 byte of data and image_buffer
 * Output: Change LSB of image_buffer
 * Return Value: e_success
 */

Status encode_byte_to_lsb(char data, char *image_buffer)
{
	//initilization
	int index = 0;

	//loop for 8 times
	for(int i = 7; i >= 0; i--)
	{
		//check bit is 0 or 1
		if(data & (1 << i))
		{
			image_buffer[index] |= 1;
		}
		else
		{
			image_buffer[index] &= ~1;
		}
		//increment 
		index++;
	}

	//return e_success
	return e_success;
}


/* 
 * Encode 4 byte of data to LSB
 * Inputs: int of 4 byte of data and image_buffer
 * Output: Change LSB of image_buffer
 * Return Value: e_success
 */
Status encode_int_to_lsb(int data, char *image_buffer)
{
	//initilazation
	int index = 0;

	//loop for int data type
	for(int i = ((sizeof(int) * 8) - 1); i >= 0; i--)
	{
		//check bit is 0 or 1
		if(data & (1 << i))
		{
			image_buffer[index] |= 1;
		}
		else
		{
			image_buffer[index] &= ~1;
		}
		//increment
		index++;
	}

	//return e_success
	return e_success;
}


/* 
 * Encode magic string length
 * Inputs: int of 1 byte of data and struct var address
 * Output: Encode the sizeof (int) byte data and send to o/p file
 * Return Value: e_success or e_failure
 */
Status encode_magic_string_len(int data, EncodeInfo *encInfo)
{
	//creat buffer
	char buffer[(sizeof(int) * 8)];

	//read data from source file
	fread(buffer, (sizeof(int)*8), 1, encInfo -> fptr_src_image);

	//encode 4 byte of data
	encode_int_to_lsb(data, buffer);

	//push to destination file
	fwrite(buffer, (sizeof(int)*8), 1, encInfo -> fptr_stego_image);

	//print the file pointer
	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(encInfo -> fptr_src_image), ftell(encInfo -> fptr_stego_image));

	//return status.	
	return (ftell(encInfo -> fptr_src_image) == ftell(encInfo -> fptr_stego_image))? e_success : e_failure;
}



/* 
 * Encode Magic string
 * Inputs: encInfo -> magic_string of data and image_buffer
 * Output: Change LSB of image_buffer
 * Return Value: e_success or e_failure
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	//create buffer.
	char buffer[8];

	for(int i = 0; i < encInfo -> m_string_len; i++)
	{		
		//take buffer data from source file.
		fread(buffer, 8, 1, encInfo -> fptr_src_image);

		//encode 1 byte of data to lsb.
		encode_byte_to_lsb(magic_string[i], buffer);

		//puch the data to destination file.
		fwrite(buffer, 8, 1, encInfo -> fptr_stego_image);
	}
	
	//print the file pointer
	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(encInfo -> fptr_src_image), ftell(encInfo -> fptr_stego_image));

	//return the status
	return (ftell(encInfo -> fptr_src_image) == ftell(encInfo -> fptr_stego_image))? e_success : e_failure;
}


/* 
 * Encode extention size
 * Inputs: int of 4 byte of data and address of structure variable
 * Output: Change LSB of image_buffer 
 * Return Value: e_success or e_failure
 */
Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo)
{
	//create buffer
	char buffer[(sizeof(int) * 8)];

	//read the data from source file
	fread(buffer, (sizeof(int) * 8), 1, encInfo -> fptr_src_image);

	//encode 4 bytes of data.
	encode_int_to_lsb(data, buffer);

	//Push the data to Destination file.
	fwrite(buffer, (sizeof(int) * 8), 1, encInfo -> fptr_stego_image);

	//print the file pointer
	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(encInfo -> fptr_src_image), ftell(encInfo -> fptr_stego_image));

	//return the status
	return (ftell(encInfo -> fptr_src_image) == ftell(encInfo -> fptr_stego_image))? e_success : e_failure;

}


/* 
 * Encode Extention
 * Inputs: encInfo -> extn_secret_file and &encInfo
 * Output: Change LSB of image_buffer
 * Return Value: e_success or e_failure
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	//creat buffer
	char buffer[8];

	for(int i = 0; i < encInfo -> extn_size; i++)
	{		
		//read the data from source file.
		fread(buffer, 8, 1, encInfo -> fptr_src_image);

		//Encode 1 byte of data to LSB.
		encode_byte_to_lsb(file_extn[i], buffer);

		//Push the data to Destination file.
		fwrite(buffer, 8, 1, encInfo -> fptr_stego_image);
	}

	//print the file pointer
	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(encInfo -> fptr_src_image), ftell(encInfo -> fptr_stego_image));

	//return the status
	return (ftell(encInfo -> fptr_src_image) == ftell(encInfo -> fptr_stego_image))? e_success : e_failure;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char buffer[(sizeof(long) * 8)];

	fread(buffer, (sizeof(long) * 8), 1, encInfo -> fptr_src_image);
 
	//this too encode long int variable.
	int index = 0;
	for(int i = (sizeof(long)*8)-1; i >= 0; i--)
	{
		if(file_size & ((long)1 << i))
		{
			buffer[index] |= 1;
		}
		else
		{
			buffer[index] &= (~1);
		}
		index++;
	}

	fwrite(buffer, (sizeof(long) * 8), 1, encInfo -> fptr_stego_image);

	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(encInfo -> fptr_src_image), ftell(encInfo -> fptr_stego_image));

	//return the status
	return (ftell(encInfo -> fptr_src_image) == ftell(encInfo -> fptr_stego_image))? e_success : e_failure;

}

/**/
//ftell(fptr_fptr)
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char secret_buffer[encInfo -> size_secret_file];

	rewind(encInfo -> fptr_secret);

	fread(secret_buffer, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);

	char src_buffer[8];

	for(int i = 0; i < encInfo -> size_secret_file; i++)
	{

		fread(src_buffer, 8, 1, encInfo -> fptr_src_image);

		encode_byte_to_lsb(secret_buffer[i], src_buffer);

		fwrite(src_buffer, 8, 1, encInfo -> fptr_stego_image);
	}

	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(encInfo -> fptr_src_image), ftell(encInfo -> fptr_stego_image));

	//return the status
	return (ftell(encInfo -> fptr_src_image) == ftell(encInfo -> fptr_stego_image))? e_success : e_failure;

}
//ftell(returns size 1)

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;

	while(fread(&ch, 1, 1, fptr_src) > 0)
	{
		fwrite(&ch, 1, 1, fptr_dest);
	}

	printf("fptr_src_image = %ld, fptr_stego_image = %ld\n", ftell(fptr_src), ftell(fptr_dest));

	//return the status
	return (ftell(fptr_src) == ftell(fptr_dest))? e_success : e_failure;
}
