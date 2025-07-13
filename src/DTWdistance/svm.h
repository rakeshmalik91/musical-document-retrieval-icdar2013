#ifndef _LIBSVM_H
#define _LIBSVM_H

#define LIBSVM_VERSION 286

#ifdef __cplusplus
extern "C" {
#endif

typedef struct svm_node{
	int index;
	double value;
}svm_node;

typedef struct svm_problem{
	int l;
	double *y;
	svm_node **x;
}svm_problem;

enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR };	/* svm_type */
enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */

typedef struct svm_parameter{
	int svm_type;
	int kernel_type;
	int degree;	/* for poly */
	double gamma;	/* for poly/rbf/sigmoid */
	double coef0;	/* for poly/sigmoid */

	/* these are for training only */
	double cache_size; /* in MB */
	double eps;	/* stopping criteria */
	double C;	/* for C_SVC, EPSILON_SVR and NU_SVR */
	int nr_weight;		/* for C_SVC */
	int *weight_label;	/* for C_SVC */
	double* weight;		/* for C_SVC */
	double nu;	/* for NU_SVC, ONE_CLASS, and NU_SVR */
	double p;	/* for EPSILON_SVR */
	int shrinking;	/* use the shrinking heuristics */
	int probability; /* do probability estimates */
}svm_parameter;

struct svm_model *svm_train(const svm_problem *prob, const svm_parameter *param);
void svm_cross_validation(const svm_problem *prob, const svm_parameter *param, int nr_fold, double *target);

int svm_save_model(const char *model_file_name, const svm_model *model);
struct svm_model *svm_load_model(const char *model_file_name);

int svm_get_svm_type(const svm_model *model);
int svm_get_nr_class(const svm_model *model);
void svm_get_labels(const svm_model *model, int *label);
double svm_get_svr_probability(const svm_model *model);

void svm_predict_values(const svm_model *model, const svm_node *x, double* dec_values);
double svm_predict(const svm_model *model, const svm_node *x);
double svm_predict_probability(const svm_model *model, const svm_node *x, double* prob_estimates);

void svm_destroy_model(svm_model *model);
void svm_destroy_param(svm_parameter *param);

const char *svm_check_parameter(const svm_problem *prob, const svm_parameter *param);
int svm_check_probability_model(const svm_model *model);
svm_model *read_problem(const char *filename, svm_problem *prob);
int **do_cross_validation(int nr_fold, const char *filename, int *data_size);

#ifdef __cplusplus
}
#endif
#endif /* _LIBSVM_H */
