#ifndef _LIBSVM_H
#define _LIBSVM_H

#include <stdint.h>

#define LOGGER_ID 0
#define LOGGER_FORMAT "%l %d %t: %m\n"

#define LIBSVM_VERSION 318

#define Malloc(type,n) (type *)malloc(((size_t) (n))*sizeof(type))

#ifdef __cplusplus
extern "C" {
#endif

extern int libsvm_version;

typedef struct _gkm_data {
    char *sid;
    int seqid;
    int label;
    int seqlen;
    uint8_t *seq;
    uint8_t *seq_rc;
    uint8_t *wt;
    uint8_t *wt_rc;
    int *kmerids;
    int *kmerids_rc;
    char *seq_string;

    double sqnorm;
} gkm_data;

union svm_data
{
    gkm_data *d;
};

struct svm_problem
{
    int l;
    double *y;
    union svm_data *x;
};

enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR }; /* svm_type */
enum { GKMDATA }; /* data_type */
enum { GKM, EST_FULL, EST_TRUNC, EST_TRUNC_RBF,
       EST_TRUNC_PW, EST_TRUNC_PW_RBF, GKM_RBF}; /* kernel_type */

struct svm_parameter
{
    int svm_type;
    int data_type;
    int kernel_type;
    int L;
    int k;
    int d;
    uint8_t M;
    double H;
    double gamma;

    /* these are for training only */
    double cache_size; /* in MB */
    double eps; /* stopping criteria */
    double C;   /* for C_SVC, EPSILON_SVR and NU_SVR */
    int nr_weight;      /* for C_SVC */
    int *weight_label;  /* for C_SVC */
    double* weight;     /* for C_SVC */
    double nu;  /* for NU_SVC, ONE_CLASS, and NU_SVR */
    double p;   /* for EPSILON_SVR */
    int shrinking;  /* use the shrinking heuristics */
    int probability; /* do probability estimates */
};

//
// svm_model
// 
struct svm_model
{
    struct svm_parameter param; /* parameter */
    int nr_class;       /* number of classes, = 2 in regression/one class svm */
    int l;          /* total #SV */
    union svm_data *SV;        /* SVs (SV[l]) */
    double **sv_coef;   /* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
    double *rho;        /* constants in decision functions (rho[k*(k-1)/2]) */
    double *probA;      /* pariwise probability information */
    double *probB;
    int *sv_indices;        /* sv_indices[0,...,nSV-1] are values in [1,...,num_traning_data] to indicate SVs in the training set */

    /* for classification only */

    int *label;     /* label of each class (label[k]) */
    int *nSV;       /* number of SVs for each class (nSV[k]) */
                /* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
    /* XXX */
    int free_sv;        /* 1 if svm_model is created by svm_load_model*/
                /* 0 if svm_model is created by svm_train */
};

struct svm_model *svm_train(const struct svm_problem *prob, const struct svm_parameter *param);
void svm_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold, int icv, double *target, const char *filename);

/*
int svm_save_model(const char *model_file_name, const struct svm_model *model);
struct svm_model *svm_load_model(const char *model_file_name);
*/

int svm_get_svm_type(const struct svm_model *model);
int svm_get_data_type(const struct svm_model *model);
int svm_get_nr_class(const struct svm_model *model);
void svm_get_labels(const struct svm_model *model, int *label);
void svm_get_sv_indices(const struct svm_model *model, int *sv_indices);
int svm_get_nr_sv(const struct svm_model *model);
double svm_get_svr_probability(const struct svm_model *model);

double svm_predict_values(const struct svm_model *model, const union svm_data x, double* dec_values);
double svm_predict_and_explain_values(
    const struct svm_model *model, const union svm_data x,
    double* dec_values, double **explanation, int mode);
double svm_predict_and_singlebaseexplain_values(
    const struct svm_model *model, const union svm_data x,
    double* dec_values, double *explanation);
double svm_predict(const struct svm_model *model, const union svm_data x);
double svm_predict_probability(const struct svm_model *model, const union svm_data x, double* prob_estimates);

void svm_free_model_content(struct svm_model *model_ptr);
void svm_free_and_destroy_model(struct svm_model **model_ptr_ptr);
void svm_destroy_param(struct svm_parameter *param);

const char *svm_check_parameter(const struct svm_problem *prob, const struct svm_parameter *param);
int svm_check_probability_model(const struct svm_model *model);

void svm_set_print_string_function(void (*print_func)(const char *));

#ifdef __cplusplus
}
#endif

#endif /* _LIBSVM_H */