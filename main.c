#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys\stat.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//МАКРОСЫ выдающие ошибки различные ошибки и останавливающие программу

#define  NULL_PTR_INP_ERROR(arr) if (arr == NULL) do {fprintf(stderr, "NO array\n");return -1;} while(0)
#define  ZERO_SIZE_ERROR(size) if (size == 0) do {fprintf(stderr, "ZERO size\n");return -2;} while(0)
#define  NULL_FILE_ERROR(file) if (file == NULL) do {fprintf(stderr, "NULL file\n");return -3;} while(0)
#define  NULL_FUNC_ERROR(func_ptr) if (func_ptr == 0) do {fprintf(stderr, "NULL func ptr\n");return -4;} while(0)
#define  EMPTY_STRING_ERROR(str) if (strlen(str) == 0) do {fprintf(stderr, "EMPTY string YO\n");return -5;} while(0)

//#define #DBG if (!debug) - TODO FIX


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//КОНСТАНТЫ и ГЛОБАЛЬНЫЕ переменные

enum COMPARES //Значние вывода для компаратора
{
	LESS = -1,
	EQUAL = 0,
	MORE = 1,
	ERROR = 2056
};

bool debug = false; //Глобальная переменная для включения режима дебага сразу во всем файле

const int MAX_STRINGS_NUM = 20000; //Максимальное кол-во строчек (заведомо больше, чем в Онегине)

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//ФУНКЦИИ вывода строк

int  fprint_str(FILE* stream, char* string) //Вывод строки string в поток stream
{
	NULL_PTR_INP_ERROR(string);

	int i = 0;

	while (string[i] != '\0' && string[i] != '\n' && string[i] != EOF)
	{
		fputc(string[i], stream);
		i++;
	}

	fputc('\n', stream);
}

int fprint_array_str(FILE* stream, void* array, size_t size) //Вывод массива строк array размера size в поток stream
{
	NULL_PTR_INP_ERROR(array);
	NULL_FILE_ERROR(stream);
	ZERO_SIZE_ERROR(size);

	char** arr_str = (char** ) array; 

	for (int i = 0; i < size; ++i)
	{
		//#DBG printf("1) <%s> ", arr_str[i]);
		fprint_str(stream, arr_str[i]);
	}
	fputc('\n', stream);
}

int print_array_ptr(void* array, size_t size) // Вывод массива указателей array размера size
{
	char** arr_ptr = (char**) array;

	for (int i = 0; i < size; i++)
	{
		printf("%p - char = <%c>\n", arr_ptr[i], *arr_ptr[i]);
	}

	putchar('\n');
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//ФУНКЦИИ сортировок и для них


void swap_void(void* char_aptr, void* b_ptr, size_t size_el) //Смена значений двух переменных по void указателям
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

int bubble_sort(void* array, size_t size, size_t size_el, int (*comparator)(void* a_ptr, void* b_ptr)/*, void (*printer)(void* array, int size)*/) //comparator returns -1 if a < b
 //Алгоритм сортировки пузырьком: array - массив неизвестного типа
 //								  size - размер масссива
 //								  size_el - размер элемента в байтах
 //								  comparator - указатель на функцию компоратор 																      //   0 if a==b
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
		//#DBG printf("num_iters = %d\n", num_iters);
		//#DBG printer(array, size);

		is_final = true;

		for (int i = 1; i < num_iters; i++)
		{
			//#DBG printf("i = %d\n", i);
			if (comparator(array + (i-1) * size_el, array + i * size_el) == MORE)  //compare PRED and CUR 
			{
				is_final = false;

				swap_void(array + (i-1) * size_el, array + i * size_el, size_el);
			}
		}

		num_iters--;

	} while (!is_final);

	//#DBG printf("bubble sort has been ended\n");

	return 0;
}

int quick_sort(void* array, size_t size, size_t size_el, int (*comparator) (void* a_ptr, void* b_ptr))
{
	NULL_PTR_INP_ERROR(array);
	ZERO_SIZE_ERROR(size);
	ZERO_SIZE_ERROR(size_el);

	NULL_FUNC_ERROR(comparator);
	
	if (size == 0 || size == 1)
	{
		return 0;
	}

	size_t pivot_indx = size - 1;
	//int pivot_el = array[pivot_indx]; DELETE

	//#DBG printf("Pivot (the first) element: %d\n", pivot_el);

	int right_i = 0;
	int left_i = size - 1;

	bool skip_pivot = 0;


	while (right_i < left_i)
	{
		if (right_i == pivot_indx) {right_i = (right_i == size - 1) ? right_i : right_i + 1;}
		if (left_i == pivot_indx) {left_i = (left_i == 0) ? left_i : left_i - 1;}

		if (comparator(array + right_i * size_el, array + pivot_indx * size_el) == MORE)
		{
			while ((comparator(array + left_i * size_el, array + pivot_indx * size_el) == MORE) && right_i != left_i)
			{
				left_i = (left_i == 0) ? left_i : left_i - 1;
			}

			swap_void(array + right_i * size_el, array + left_i * size_el, size_el);
 	
			left_i = (left_i == 0) ? left_i : left_i - 1;
		}
		
		right_i = (right_i == size - 1) ? right_i : right_i + 1;
		//#DBG print_array_int(array, size);
		//#DBG printf("right = %d, left = %d\n", right_i, left_i);
	}

	//#DBG printf("after big while: ");
	//#DBG print_array_int(array, size);
	//#DBG printf("right = %d, left = %d\n", right_i, left_i);

	swap_void(array + pivot_indx * size_el, array + ((comparator(array + left_i * size_el, array + pivot_indx * size_el) == MORE) ? left_i : ++left_i) * size_el, size_el);
	//#DBG print_array_int(array, size);

	quick_sort(array, left_i, size_el, comparator);
	quick_sort(array + (left_i + 1) * size_el, size - left_i - 1, size_el, comparator);

	return 0;

	//#DBG printf("-------------------------------------------------------\n");
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//ФУНКЦИИ-компораторы

int comp_alphabet_str(void* a_ptr, void* b_ptr) //Компаратор сортировки строк в алфавитном порядке (раньше в алф буква - меньше строка)
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

	while (a[i_a] != '\n' && b[i_b] != '\n')
	{
		//printf("a[i_a] = <%c>, b[i_b] = <%c>\n", a[i_a], b[i_b]);

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

int comp_rev_alphabet_str(void* a_ptr, void* b_ptr) //Компоратор строк в реверс алфавитном порядке (раньше в алфаивите буква - меньше строка)
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

	while (a[i_a] != '\n' && a[i_a] != '\0')
	{
		i_a++;
	}
	while (b[i_b] != '\n' && b[i_b] != '\0')
	{
		i_b++;
	}

	i_a--;
	i_b--;

	while (i_a >= 0 || i_b >= 0)
	{


		while (!isalpha(a[i_a]) || isspace(a[i_a])) i_a--;
		while (!isalpha(b[i_b]) || isspace(b[i_b])) i_b--;

		if (tolower(a[i_a]) < tolower(b[i_b])) return LESS;
		if (tolower(a[i_a]) > tolower(b[i_b])) return MORE;

		i_a--;
		i_b--;
	}

	if (tolower(a[i_a]) == tolower(b[i_b])) return EQUAL;
	if (tolower(a[i_a]) == '\0') return LESS;
	if (tolower(b[i_b]) == '\0') return MORE;
}


int uint_comparator_up(void* a_ptr, void* b_ptr) //Компаратор указателей по возрастанию
{
	size_t a = *((size_t*) a_ptr);
	size_t b = *((size_t*) b_ptr);

	if (a < b) return LESS;
	if (a == b) return EQUAL;
	if (a > b) return MORE;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//ФУНКЦИИ для чтения текста


int text_beater1(char** text_ptr, char* string_pointers[]) //Разделение всего текста  на строки и занесение в массив указателей на строки string_pointers[]
{
	size_t pos = 0;

	int i = 0;

	char* text = *text_ptr;

	string_pointers[0] = text;

	while ((text)[pos] != '\0' && (text)[pos] != EOF) //Идем до конца текста
	{
		printf("char in text in pos <%d> = <%c>\n", pos, text[pos]);
		if (text[pos] == '\n')
		{
			while (isspace((text)[pos]) || (!isalpha((text)[pos]) && (text)[pos] != '`'))
			{
				if ((text)[pos] == '\0')
				{
					return i + 1;
				}

				pos++;
			}

			i++;
			string_pointers[i] = &(text[pos]);

			printf("PTR added to the str_ptrs into i = %d\n", i);
		}
		pos++;
	}
	return i + 1;
}




int read_text(const char* filename , FILE* poem_file, char** text, char* string_pointers[]) //Чтение текста  из исходного файла
{
	NULL_PTR_INP_ERROR(string_pointers);
	EMPTY_STRING_ERROR(filename);

	struct stat poem_info = {};

	if (stat(filename, &poem_info)) return -fprintf(stderr, "cant get stat of file\n");

	*text = (char*) calloc((poem_info.st_size + 2), sizeof(char));

	fread(*text, sizeof(char), poem_info.st_size, poem_file);
	
	(*text)[poem_info.st_size] = '\0';
	printf("We have read a file: %s\n", *text);;

	return text_beater1(text, string_pointers);
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//MAIN

int main()
{

	const char input_filename[] = "Texts\\Eugeny Onegin.txt";  //TODO = argv[1]
	FILE* poem_file = fopen(input_filename, "r");            //Открытие файла с поэмой
	NULL_FILE_ERROR(poem_file);

	char* text = NULL;

	size_t num_of_strings = MAX_STRINGS_NUM;     
	char** string_pointers = (char**) calloc(num_of_strings, sizeof(char*)); //Создание массива указателей на начала строк

	num_of_strings = read_text(input_filename, poem_file, &text, string_pointers);
	fclose(poem_file);

/*
	printf("BEFORE CHANGING|||||||num_of_strings = <%d>\n------------------------------------------------------------------------\n", num_of_strings);
	fprint_array_str(stdout, string_pointers, num_of_strings);   //Дебажный вывод прочитанного текста по массиву указателей
	//print_array_ptr(string_pointers, num_of_strings);


	bubble_sort(string_pointers, num_of_strings, sizeof(char*), comp_alphabet_str); //Первая сортировка в алфавитном порядке


	printf("AFTER CHANGING|||||||num_of_strings = <%d>\n------------------------------------------------------------------------\n", num_of_strings);
	fprint_array_str(stdout, string_pointers, num_of_strings);  //Дебажный вывод отсортированного текста по массиву указателей и сам массив указателей
	print_array_ptr(string_pointers, num_of_strings);

	printf("End of sorting\n");
*/



	const char out_file_name[] = "Texts\\poem_out.txt"; 

	FILE* out_file = fopen(out_file_name, "w");
	NULL_FILE_ERROR(out_file);





	bubble_sort(string_pointers, num_of_strings, sizeof(char*), comp_alphabet_str);

	fputs("----------------------------------------\n\n FIRST SORT \n\n", out_file);  //Сортировка и вывод в output файл отсортированной в алфавитном порядке

	fprint_array_str(out_file, string_pointers, num_of_strings);






	bubble_sort(string_pointers, num_of_strings, sizeof(char*), comp_rev_alphabet_str);

	fputs("----------------------------------------\n\n SECOND SORT \n\n", out_file); //Сортировка и вывод в output файл отсортированной в реверс-алфавитном порядке

	fprint_array_str(out_file, string_pointers, num_of_strings);





	bubble_sort(string_pointers, num_of_strings, sizeof(char*), uint_comparator_up); //Сортировка массива указателей для восстановления изначального порядка

	fputs("----------------------------------------\n\n OLD FILE \n\n", out_file); //Вывод в output файл изначального текста

	fprint_array_str(out_file, string_pointers, num_of_strings);


	printf("printing old version has been ending\n");




	fclose(out_file);
	system(out_file_name); //Открытие output файла перед пользователем


	free(text);
	free(string_pointers);


	printf("ALL GOOD ENDING\n");

	return 0;

}