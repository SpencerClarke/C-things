#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define DATA_COUNT 160

struct Node 
{
	double split;
	double *outputs;
	double *inputs;
	int length;
	struct Node *left;
	struct Node *right;
};
struct DecisionTree
{
	struct Node *head;
};

double cost(double *data, int length);
void gen_children(struct Node *parent, int depth_remaining);

struct DecisionTree create_tree(double *inputs, double *outputs, int length);
void fit(struct DecisionTree *tree, int depth);
double predict(struct DecisionTree *tree, double input);

int main(void)
{

	struct DecisionTree tree;
	struct DecisionTree tree2;
	struct DecisionTree tree3;

	double *inputs;
	double *outputs;
	double *predictions;
	double *residuals;

	int i;
	
	inputs = malloc(sizeof(double) * DATA_COUNT);
	outputs = malloc(sizeof(double) * DATA_COUNT);
	predictions = malloc(sizeof(double) * DATA_COUNT);
	residuals = malloc(sizeof(double) * DATA_COUNT);

	srand(time(NULL));
	for(i = 0; i < DATA_COUNT; i++)
	{
		inputs[i] = ((float)(rand()) / RAND_MAX) * 10 - 5;
		outputs[i] = sin(inputs[i])*0.5 + ((double)(rand()) / RAND_MAX) * 0.2 - 0.1;
	}

	tree = create_tree(inputs, outputs, DATA_COUNT);
	fit(&tree, 4);
	for(i = 0; i < DATA_COUNT; i++)
	{
		predictions[i] = predict(&tree, inputs[i]);
	}
	for(i = 0; i < DATA_COUNT; i++)
	{
		residuals[i] = outputs[i] - predictions[i];
	}

	tree2 = create_tree(inputs, residuals, DATA_COUNT);
	fit(&tree2, 3);
	for(i = 0; i < DATA_COUNT; i++)
	{
		predictions[i] = predictions[i] + predict(&tree2, inputs[i]);
	}
	for(i = 0; i < DATA_COUNT; i++)
	{
		residuals[i] = outputs[i] - predictions[i];
	}

	tree3 = create_tree(inputs, residuals, DATA_COUNT);
	fit(&tree3, 3);
	for(i = 0; i < DATA_COUNT; i++)
	{
		predictions[i] = predictions[i] + predict(&tree3, inputs[i]);
	}
	
	for(i = 0; i < DATA_COUNT; i++)
		printf("%lf;", inputs[i]);
	printf("\n");
	for(i = 0; i < DATA_COUNT; i++)
		printf("%lf;", outputs[i]);
	printf("\n");
	for(i = 0; i < DATA_COUNT; i++)
		printf("%lf;", predictions[i]);
	printf("\n");

	return 0;
}

double cost(double *data, int length)
{
	double global_mean;
	double global_cost;

	double r;

	#pragma omp parallel
	{
		int i;
		double sum;
		int thread_num;
		int num_threads;

		thread_num = omp_get_thread_num();
		num_threads = omp_get_num_threads();
		sum = 0;
		for(i = thread_num; i < length; i += num_threads)
			sum += data[i];

		#pragma omp critical
		{
			global_mean += sum;
		}

	}
	global_mean /= length;

	#pragma omp parallel
	{
		int i;
		double cost;
		int thread_num;
		int num_threads;

		thread_num = omp_get_thread_num();
		num_threads = omp_get_num_threads();
		cost = 0;
		for(i = thread_num; i < length; i += num_threads)
		{
			r = global_mean - data[i];
			cost += r * r;
		}
		#pragma omp critical
		{
			global_cost += cost;
		}
	}
	return global_cost / length;
}

void gen_children(struct Node *parent, int depth_remaining)
{
	struct Node *left;
	struct Node *right;

	int i, j;

	int optimal_split_index;

	double *left_input_partition;
	double *left_output_partition;
	double *right_input_partition;
	double *right_output_partition;

	int left_index;
	int right_index;
	int optimal_left_index;
	int optimal_right_index;;

	double test_split;

	double test_cost;
	double optimal_cost;

	if(depth_remaining < 0)
		return;
	if(parent->length < 2)
		return;

	left_output_partition = malloc(sizeof(double) * parent->length);
	right_output_partition = malloc(sizeof(double) * parent->length);

	optimal_cost = -1;
	optimal_split_index = -1;
	for(i = 0; i < parent->length; i++)
	{
		test_split = parent->inputs[i];
		left_index = right_index = 0;
		for(j = 0; j < parent->length; j++)
		{
			if(parent->inputs[j] < test_split)
				left_output_partition[left_index++] = parent->outputs[j];
			else if(parent->inputs[j] > test_split)
				right_output_partition[right_index++] = parent->outputs[j];
			
		}
		if(right_index == 0)
			right_output_partition[right_index++] = test_split;
		else
			left_output_partition[left_index++] = test_split;

		test_cost = cost(left_output_partition, left_index) + cost(right_output_partition, right_index);

		if(optimal_split_index == -1 || test_cost < optimal_cost)
		{
			optimal_split_index = i;
			optimal_cost = test_cost;
			optimal_right_index = right_index;
			optimal_left_index = left_index;
		}
	}
	left_input_partition = malloc(sizeof(double) * optimal_left_index);
	right_input_partition = malloc(sizeof(double) * optimal_right_index);
	left_output_partition = realloc(left_output_partition, sizeof(double) * optimal_left_index);
	right_output_partition = realloc(right_output_partition, sizeof(double) * optimal_right_index);

	left_index = right_index = 0;
	for(i = 0; i < parent->length; i++)
	{
		if(parent->inputs[i] < parent->inputs[optimal_split_index])
		{
			left_input_partition[left_index] = parent->inputs[i];
			left_output_partition[left_index] = parent->outputs[i];
			left_index++;
		}
		else if(parent->inputs[i] > parent->inputs[optimal_split_index])
		{
			right_input_partition[right_index] = parent->inputs[i];
			right_output_partition[right_index] = parent->outputs[i];
			right_index++;
		}
	}
	if(right_index == 0)
	{
		right_output_partition[right_index] = parent->outputs[optimal_split_index];
		right_input_partition[right_index] = parent->inputs[optimal_split_index];
		right_index++;
	}
	else
	{
		left_output_partition[left_index] = parent->outputs[optimal_split_index];
		left_input_partition[left_index] = parent->inputs[optimal_split_index];
		left_index++;
	}

	left = malloc(sizeof(struct Node));
	right = malloc(sizeof(struct Node));
	
	left->inputs = left_input_partition;
	left->outputs = left_output_partition;
	left->length = left_index;
	left->left = left->right = NULL;

	right->inputs = right_input_partition;
	right->outputs = right_output_partition;
	right->length = right_index;
	right->right = right->left = NULL;

	parent->left = left;
	parent->right = right;
	parent->split = parent->inputs[optimal_split_index];


	free(parent->inputs);
	free(parent->outputs);

	gen_children(parent->left, depth_remaining - 1);
	gen_children(parent->right, depth_remaining - 1);
}

struct DecisionTree create_tree(double *inputs, double *outputs, int length)
{
	int i;
	struct DecisionTree out;

	out.head = malloc(sizeof(struct Node));
	
	out.head->left = NULL;
	out.head->right = NULL;

	out.head->inputs = malloc(sizeof(double) * length);
	out.head->outputs = malloc(sizeof(double) * length);
	out.head->length = length;
	for(i = 0; i < length; i++)
	{
		out.head->inputs[i] = inputs[i];
		out.head->outputs[i] = outputs[i];
	}

	return out;
}


void fit(struct DecisionTree *tree, int depth)
{
	gen_children(tree->head, depth);
}

double predict(struct DecisionTree *tree, double input)
{
	struct Node *current;
	int i;
	float mean;

	current = tree->head;
	while(current->left != NULL)
	{
		if(input > current->split)
			current = current->right;
		else
			current = current->left;
	}
	mean = 0;
	for(i = 0; i < current->length; i++)
	{
		mean += current->outputs[i];
	}

	mean /= current->length;
	return mean;
}

