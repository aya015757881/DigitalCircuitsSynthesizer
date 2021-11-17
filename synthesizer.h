#include <stdio.h>

// 结构体
typedef struct in IN;
typedef struct sm SM;
typedef struct rv RV;
typedef struct num NUM;
typedef struct ee1 EE1;
typedef struct ee2 EE2;
typedef struct _not NOT;
typedef struct _and AND;
typedef struct _or OR;
typedef struct ive IVE;
typedef struct de DE;
typedef struct imp IMP;
typedef struct bf BF;
typedef struct tt TT;
typedef struct out OUT;
typedef struct pa PA;
typedef struct bl BL;
typedef struct gc GC;
typedef struct sv SV;
typedef struct er ER;
typedef struct tsg TSG;
typedef struct output OP;


struct in {
	char name[20];
	int num;
	IN *p;
};

struct sm {
	char name[20];
	int start;
    int end;
};

struct rv {
	IN *in_p;
	RV *p;
};

struct num {
	int n;
	NUM *p;
};

struct ee1 {
	IN *in_p;
	int in_st;
    int addr;
	EE1 *p;
};

struct ee2 {
	int val;
    int addr;
	EE2 *p;
};

struct _not {
	int in;
    int num;
	NOT *p;
};

struct _and {
	NUM in_head;
    NUM r_head;
    NUM rn_head;
    NUM not_head;
	int num;
	AND *p;
};

struct _or {
	NUM in_head;
    NUM r_head;
    NUM rn_head;
    NUM not_head;
    NUM and_head;
	int num;
	OR *p;
};

struct ive {
	int num;
    int st;
	IVE *p;
};

struct de {
	int st;
	IVE ive_head;
	DE *p;
};

struct imp {
	IVE ive_head;
	int idx;
    int num;
	IMP *p;
};

struct bf {
	int idx;
	IMP imp_head;
};

struct tt {
	short val;
    short idx;
};

struct out {
	char name[20];
	EE1 ee1_head;
	EE2 ee2_head;
	RV rv_head;
	DE de_head;
	TT *tt;
	BF bf;
	int num;
    int idx;
    int dnum;
    int clear_type;
    int clock_edge;
    int clear_edge;
	struct out *p;
};

struct pa {
	char name[20];
	int start;
    int end;
	NUM val_head;
	PA *p;
};

struct bl {
	int start;
    int end;
	BL *p;
};

struct gc {
	IN *in_p;
	int st;
    int start;
    int end;
	GC *p;
};

struct sv {
	char name[20];
	SV *p;
};

struct er {
	int st;
    int start;
    int end;
	ER *p;
};

struct tsg {
	int ena;
    int data;
    int d_idx;
    int num;
	char out_name[20];
	TSG *p;
};

struct output {
	char name[20];
	int num;
	OP *p;
};


//全局变量
FILE *fp;
int n, n1, n2, ee_name_len, count, count_in, count_out, st, *p, qua, allcov, sign;
char a[10000], file[128], filectt[128], ee_name[20], v_name[20], clock_not[20];
char reset_not[20], reg_string[20];
IN in_head, *in, *clock, *reset;
OUT out_head, *out, *out1;
NUM num_head, *num, *val, *iu, *ru, *rnu, *nu, *au;
PA pa_head, *pa;
BL bl_head, *bl, al_head, *al;
GC gc_head, *gc, *gc1;
SV sv_head, *sv, *sv1;
EE1 *ee1;
EE2 *ee2;
RV *rv, *rv1;
SM sm;
DE *de;
IVE *ive, *ive1;
TT *tt;
IMP *imp;
NOT not_head, *not_, *not1;
AND and_head, *and_, *and1;
OR or_head, *or_;
ER er_head, *er;
TSG tsg_head, *tsg;
OP op_head, *op;



//函数
void extract();
void find_var();
void find_sm();
void find_pa();
void find_bl();
void find_gc();
void find_smc();
void find_sv();
void find_ee();
void form_rv();
void form_de();
void form_tt();
void form_bf();
void form_nl();
void display();
void clear();
void input();
void inout();
void reg();
void first(const char *c);
void next(const char *c);
void next_pa();
void get_else(int i, IN *in_p, int st);
void edge(const char *c);
void dif_spe();
void get_clock();
void ge_ee(int idx);
void sm_ee();
void smcv();
void gcv();
void asnv();
void sfv();
void ee1_de(int val);
void ee2_de();
void pre_asn();
void de_asn();
void combine();
void sieve();
void get_not();
void get_and();
void connect_imp_gate();
void get_or();
void connect_out_gate();
void get_eff_reset();
void get_cp_rst();
void get_nstring(char *name);
void get_tsg();
void get_op();
void sg(const char *c);
void pr(const char *c);
void range_type(int i);
void find_al();
void clock_edg(const char *edg);
void gen_ee1(int idx);
void gen_ee2(int idx);
void add_gcv();
void asn_de(int val);
void gc_de();
void pa_de();
void get_bn(int val);
void pickvar(int idx, int start, int end);
void erase();
void add_not(int num);
void add_and();
void step();
void gens(int ind);
void genp(int ind);
void get_ive();
void posneg();
void getin();
void getout();
void obt();

int cmp(char *m, const char *t);
int get_range();
int check_num(char *c);
int check_sm();
int scan();
int check_sv_cov(char *name);
int seek_gc(char *c);
int check_sv();
int check_cov();
int check_de();
int rdd(IMP *a);
int in_num();
int check_reg();
int check_not(int num);
int check_clock_not();
int check_reset_not();
int check_ive_num();
int check_and();
int reg_num();
int not_num();
int and_num();
int imp_num();
int eff_reset();
int tsg_reg_num(int start, char end);
int match(int val);
int check_and_num();
int check_and_var();
int al_state();
int fil();
int uncov();
int cov();
int check_in_st();
int check_reg_st();
int check_regn_st();
int check_not_st();

char *in_name(int num);
char *reg_str(int num);





