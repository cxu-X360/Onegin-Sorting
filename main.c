#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <sys\stat.h>

#define  NULL_PTR_INP_ERROR(arr) if (arr == NULL) do {fprintf(stderr, "NO array\n");return -1;} while(0)
#define  ZERO_SIZE_ERROR(size) if (size == 0) do {fprintf(stderr, "ZERO size\n");return -2;} while(0)
#define  NULL_FILE_ERROR(file) if (file == NULL) do {fprintf(stderr, "NULL file\n");return -3;} while(0)
#define  NULL_FUNC_ERROR(func_ptr) if (func_ptr == 0) do {fprintf(stderr, "NULL func ptr\n");return -4;} while(0)
#define  EMPTY_STRING_ERROR(str) if (strlen(str) == 0) do {fprintf(stderr, "EMPTY string YO\n");return -5;} while(0)


enum COMPARES
{
	LESS = -1,
	EQUAL = 0,
	MORE = 1,
	ERROR = 2056
};

const int MAX_STRING_SIZE = 100;

void fprint_str(FILE* stream, char* string)
{
	assert(string);

	int i = 0;

	while (string[i] != '\0' && string[i] != '\n' && string[i] != EOF)
	{
		fputc(string[i], stream);
		i++;
	}

	fputc('\n', stream);
}


void swap_void(void* char_aptr, void* b_ptr, size_t size_el)
{
	void* temp = calloc(1, size_el);

	for (size_t i = 0; i < size_el; i++)
	{
		*((char*) temp + i) = *((char*) char_aptr + i);
		*((char*) char_aptr + i) = *((char*) b_ptr + i);
		*((char*) b_ptr + i) = *((char*) temp + i);
	}

	free(temp);
	return;
}

int bubble_sort(void* array, size_t size, size_t size_el, int (*comparator)(void* char_aptr, void* b_ptr)/*, void (*printer)(void* array, int size)*/) //comparator returns -1 if a < b
 																											//   0 if a==b
 																											//	 1 if a > b
{
	NULL_PTR_INP_ERROR(array);
	ZERO_SIZE_ERROR(size);
	ZERO_SIZE_ERROR(size_el);

	NULL_FUNC_ERROR(comparator);
	/*NULL_FUNC_ERROR(printer);*/

	bool is_final = true;

	int num_iters = (int) size;

	do
	{
		printf("num_iters = %d\n", num_iters);
		//printer(array, size);

		is_final = true;

		for (int i = 1; i < num_iters; i++)
		{
			//printf("i = %d\n", i);
			if (comparator(array + (i-1) * size_el, array + i * size_el) == MORE)  //compare PRED and CUR 
			{
				is_final = false;

				swap_void(array + (i-1) * size_el, array + i * size_el, size_el);
			}
		}

		num_iters--;

	} while (!is_final);

	printf("bubble sort has been ended\n");

	return 0;
}



int fprint_array_str(FILE* stream, void* array, size_t size)
{
	NULL_PTR_INP_ERROR(array);
	NULL_FILE_ERROR(stream);
	ZERO_SIZE_ERROR(size);

	char** arr_str = (char** ) array; 

	for (int i = 0; i < size; ++i)
	{
		//printf("1) <%s> ", arr_str[i]);
		fprint_str(stream, arr_str[i]);
	}
	fputc('\n', stream);
}

int print_array_ptr(void* array, size_t size)
{
	char** arr_ptr = (char**) array;

	for (int i = 0; i < size; i++)
	{
		printf("%p - char = <%c>\n", arr_ptr[i], *arr_ptr[i]);
	}

	putchar('\n');
}

int text_beater1(char* text, char* string_pointers[])
{
	size_t pos = 0;

	int i = 0;

	string_pointers[0] = text;

	while (text[pos] != '\0' && text[pos] != EOF)
	{
		//printf("char in text in pos <%d> = <%c>\n", pos, text[pos]);
		if (text[pos] == '\n')
		{
			while (isspace(text[pos]) || (!isalpha(text[pos]) && text[pos] != '`'))
			{
				if (text[pos] == '\0')
				{
					return i + 1;
				}

				pos++;
			}

			i++;
			string_pointers[i] = &text[pos];

			//printf("PTR added to the str_ptrs into i = %d\n", i);
		}
		pos++;
	}
	return i + 1;
}




int read_text(const char* filename , FILE* poem_file, char* text, char* string_pointers[])
{
	NULL_PTR_INP_ERROR(string_pointers);
	EMPTY_STRING_ERROR(filename);

	struct stat poem_info = {};

	if (stat(filename, &poem_info)) return -fprintf(stderr, "cant get stat of file\n");

	text = realloc(text, (poem_info.st_size + 1) * sizeof(char));

	fread(text, sizeof(char), poem_info.st_size, poem_file);
	
	text[poem_info.st_size] = '\0';
	printf("We have read a file: %s\n", text);;

	return text_beater1(text, string_pointers);
}

char* lowercase_str(char* str)
{
	int i = 0;

	while (str[i] != '\0')
	{
		str[i] = tolower(str[i]);
		i++;
	}

	return str;

}

int comp_alphabet_str(void* a_ptr, void* b_ptr)
{
	if (a_ptr == NULL || b_ptr == NULL)
	{
		fprintf(stderr, "NULL PTR ERROR\n");
		return ERROR;
	}

	char* a = *((char**) a_ptr);
	char* b = *((char**) b_ptr);

	int i_a = 0;
	int i_b = 0;

	while (a[i_a] != '\0' && b[i_b] != '\0')
	{


		while (!isalpha(a[i_a]) || isspace(a[i_a])) i_a++;
		while (!isalpha(b[i_b]) || isspace(b[i_b])) i_b++;

		if (tolower(a[i_a]) < tolower(b[i_b])) return LESS;
		if (tolower(a[i_a]) > tolower(b[i_b])) return MORE;

		i_a++;
		i_b++;
	}

	if (tolower(a[i_a]) == tolower(b[i_b])) return EQUAL;
	if (tolower(a[i_a]) == '\0') return LESS;
	if (tolower(b[i_b]) == '\0') return MORE;
}

int uint_comparator_up(void* a_ptr, void* b_ptr)
{
	size_t a = *((size_t*) a_ptr);
	size_t b = *((size_t*) b_ptr);

	if (a < b) return LESS;
	if (a == b) return EQUAL;
	if (a > b) return MORE;
}

int main()
{
	const char input_filename[] = "Eugeny Onegin.txt";
	FILE* poem_file = fopen(input_filename, "r");
	NULL_FILE_ERROR(poem_file);

	char* text = (char*) malloc(sizeof(char));

	size_t num_of_strings = 100;
	char** string_pointers = (char**) calloc(num_of_strings, 100);

	num_of_strings = read_text(input_filename, poem_file, text, string_pointers);
	fclose(poem_file);

	printf("BEFORE CHANGING|||||||num_of_strings = <%d>\n------------------------------------------------------------------------\n", num_of_strings);

	fprint_array_str(stdout, string_pointers, num_of_strings);
	//print_array_ptr(string_pointers, num_of_strings);


	bubble_sort(string_pointers, num_of_strings, sizeof(char*), comp_alphabet_str);


	printf("AFTER CHANGING|||||||num_of_strings = <%d>\n------------------------------------------------------------------------\n", num_of_strings);


	fprint_array_str(stdout, string_pointers, num_of_strings);
	print_array_ptr(string_pointers, num_of_strings);

	printf("End of sorting\n");




	const char out_file_name[] = "poem_out.txt"; 

	FILE* out_file = fopen(out_file_name, "w");
	NULL_FILE_ERROR(out_file);

	fputs("----------------------------------------\n\n FIRST SORT \n\n", out_file);

	fprint_array_str(out_file, string_pointers, num_of_strings);




	fputs("----------------------------------------\n\n OLD FILE \n\n", out_file);

	bubble_sort(string_pointers, num_of_strings, sizeof(char*), uint_comparator_up);
	fprint_array_str(out_file, string_pointers, num_of_strings);


	printf("printing old version has been ending\n");




	fclose(out_file);


	free(text);
	free(string_pointers);

	assert(0);

	printf("ALL GOOD ENDING\n");
	return 0;

}