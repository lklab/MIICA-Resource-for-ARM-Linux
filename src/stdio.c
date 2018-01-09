#include "io.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct io_value_t
{
	void* variable;
	int size;
	char* name;
} io_value_t;

static int input_count = 0;
static int output_count = 0;
static io_value_t* input_list = NULL;
static io_value_t* output_list = NULL;

static int exchange_count = 0;
static int skip_count = 0;

int io_init(void)
{
	input_count = 0;
	output_count = 0;

	return 0;
}

int io_mapping(io_mapping_info_t* mapping_list, int mapping_count)
{
	int i;
	int ic = 0;
	int oc = 0;
	io_value_t* temp_target = NULL;

	if(input_count != 0 || output_count != 0)
		return 1;

	for(i = 0; i < mapping_count; i++)
	{
		switch(mapping_list[i].size)
		{
			case 1 :
			case 2 :
			case 4 : break;
			default :
				input_count = 0;
				output_count = 0;
				return 1;
		}

		if(mapping_list[i].direction == 1)
			input_count++;
		else if(mapping_list[i].direction == 0)
			output_count++;
	}

	input_list = (io_value_t*)malloc(sizeof(io_value_t) * input_count);
	output_list = (io_value_t*)malloc(sizeof(io_value_t) * output_count);

	for(i = 0; i < mapping_count; i++)
	{
		if(mapping_list[i].direction == 1)
			temp_target = &input_list[ic++];
		else if(mapping_list[i].direction == 0)
			temp_target = &output_list[oc++];

		temp_target -> variable = mapping_list[i].model_addr;
		temp_target -> size = mapping_list[i].size;
		temp_target -> name = mapping_list[i].network_addr;
	}

	return 0;
}

int io_activate(unsigned long long interval)
{
	return 0;
}

int io_exchange(void)
{
	int i;

	printf("exchange count = %d\n\n", exchange_count++);

	for(i = 0; i < output_count; i++)
	{
		switch(output_list[i].size)
		{
			case 1 :
				printf("output value of [%s] : %d\n", output_list[i].name,
					*((char*)(output_list[i].variable)));
				break;
			case 2 :
				printf("output value of [%s] : %d\n", output_list[i].name,
					*((short*)(output_list[i].variable)));
				break;
			case 4 :
				printf("output value of [%s] : %d\n", output_list[i].name,
					*((int*)(output_list[i].variable)));
				break;
		}
	}
	printf("\n");

	if(skip_count > 0)
	{
		skip_count--;
		printf("===========================================================\n");
		return 0;
	}

	for(i = 0; i < input_count; i++)
	{
		printf("input value of [%s] : ", input_list[i].name);
		switch(input_list[i].size)
		{
			case 1 :
				scanf("%hhd", (char*)(input_list[i].variable));
				break;
			case 2 :
				scanf("%hd", (short*)(input_list[i].variable));
				break;
			case 4 :
				scanf("%d", (int*)(input_list[i].variable));
				break;
		}
	}
	printf("\nskip count : ");
	scanf("%d", &skip_count);
	fflush(stdin);

	printf("\n===========================================================\n");

	return 0;
}

int io_cleanup(void)
{
	if(input_list != NULL)
	{
		free(input_list);
		input_list = NULL;
	}
	if(output_list != NULL)
	{
		free(output_list);
		output_list = NULL;
	}
	
	return 0;
}
