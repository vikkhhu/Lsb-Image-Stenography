#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <stdarg.h>
#include <string.h>
int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		int res = check_operation_type(argv);

		//creating structure variable
		EncodeInfo encInfo;
		DecodeInfo decInfo;

		if(res == e_encode)
		{
			if(argc > 3)
			{
				printf("---------------SELECTED ENCODE---------------\n\n");

				//function call for validation
				if(read_and_validate_encode_args(argv, &encInfo) == e_success)
				{
					printf("--------VALIDATION SUCCESSFUL.\n\n");

					//function call for encoding
					if(do_encoding(&encInfo) == e_success)
					{
						printf("---------------ENCODING SUCCESSFUL---------------\n");
					}
					else
					{
						printf("Error : ENCODING FAILS.\n");
					}    
				}
				else
				{
					printf("Error : VALIDATION FAILS.\n");
				}
			}
			else
			{
				printf("Error : Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
			}
		}
		else if(res == e_decode)
		{
			if(argc > 2)
			{
				printf("---------------SELECTED DECODING---------------\n");
		
				//fuction call for read and validate.
				if(read_and_validate_decode_args(argv, &decInfo) == e_success)
				{
					printf("--------VALIDATION SUCCESSFULL.\n\n");

					//function call for decoding
					if(do_decoding(&decInfo) == e_success)
					{
						printf("---------------DECODING SUCCESSFUL---------------\n\n");
					}
					else
					{
						printf("Error : DECODING FAILS.\n");
					}
				}
				else
				{
					printf("Error : VALIDATION FAILS.\n");
				}
			}
			else
			{
				printf("Error : Decoding : ./a.out -d <.bmp file> [output file]\n");
			}
						
		}
		else
		{
			printf("Error : For Encoding : -e \n\tFor Decoding : -d\n");
		}
	}
	else
	{
		printf("Error : Pass the ARGUMENTS through COMMAND LINE.\nUsage :\nFor Encoding : ./a.out -e <.bmp file> <secret file> [output file]\nFor Decoding : ./a.out -d <.bmp file> [output file]\n");
	}
		
		return 0;
}

