//结构体
typedef struct in
{
	char name[20];
	int num;
	struct in *p;
}IN;
typedef struct sm
{
	char name[20];
	int start,end;
}SM;
typedef struct rv
{
	IN *in_p;
	struct rv *p;
}RV;
typedef struct num
{
	int n;
	struct num *p;
}NUM;
typedef struct ee1
{
	IN *in_p;
	int in_st,addr;
	struct ee1 *p;
}EE1;
typedef struct ee2
{
	int val,addr;
	struct ee2 *p;
}EE2;
typedef struct not
{
	int in,num;
	struct not *p;
}NOT;
typedef struct and
{
	NUM in_head,r_head,rn_head,not_head;
	int num;
	struct and *p;
}AND;
typedef struct or
{
	NUM in_head,r_head,rn_head,not_head,and_head;
	int num;
	struct or *p;
}OR;
typedef struct ive
{
	int num,st;
	struct ive *p;
}IVE;
typedef struct de
{
	int st;
	IVE ive_head;
	struct de *p;
}DE;
typedef struct imp
{
	IVE ive_head;
	int idx,num;
	struct imp *p;
}IMP;
typedef struct bf
{
	int idx;
	IMP imp_head;
}BF;
typedef struct tt
{
	short val,idx;
}TT;
typedef struct out
{
	char name[20];
	EE1 ee1_head;
	EE2 ee2_head;
	RV rv_head;
	DE de_head;
	TT *tt;
	BF bf;
	int num,idx,dnum,clear_type,clock_edge,clear_edge;
	struct out *p;
}OUT;
typedef struct pa
{
	char name[20];
	int start,end;
	NUM val_head;
	struct pa *p;
}PA;
typedef struct bl
{
	int start,end;
	struct bl *p;
}BL;
typedef struct gc
{
	IN *in_p;
	int st,start,end;
	struct gc *p;
}GC;
typedef struct sv
{
	char name[20];
	struct sv *p;
}SV;
typedef struct er
{
	int st,start,end;
	struct er *p;
}ER;
typedef struct tsg
{
	int ena,data,d_idx,num;
	char out_name[20];
	struct tsg *p;
}TSG;
typedef struct output
{
	char name[20];
	int num;
	struct output *p;
}OP;





//全局变量
FILE *fp;
int n,n1,n2,ee_name_len,count,count_in,count_out,st,*p,qua,allcov,sign;
char a[10000],file[20],filetxt[30],filectt[30],ee_name[20],v_name[20],clock_not[20],reset_not[20],reg_string[20];
IN in_head,*in,*clock,*reset;
OUT out_head,*out,*out1;
NUM num_head,*num,*val,*iu,*ru,*rnu,*nu,*au;
PA pa_head,*pa;
BL bl_head,*bl,al_head,*al;
GC gc_head,*gc,*gc1;
SV sv_head,*sv,*sv1;
EE1 *ee1;
EE2 *ee2;
RV *rv,*rv1;
SM sm;
DE *de;
IVE *ive,*ive1;
TT *tt;
IMP *imp;
NOT not_head,*not,*not1;
AND and_head,*and,*and1;
OR or_head,*or;
ER er_head,*er;
TSG tsg_head,*tsg;
OP op_head,*op;





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
void first();
void next();
void next_pa();
void get_else();
void edge();
void dif_spe();
void get_clock();
void ge_ee();
void sm_ee();
void smcv();
void gcv();
void asnv();
void sfv();
void ee1_de();
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
void get_nstring();
void get_tsg();
void get_op();
void sg();
void pr();
void range_type();
void find_al();
void clock_edg();
void gen_ee1();
void gen_ee2();
void add_gcv();
void asn_de();
void gc_de();
void pa_de();
void get_bn();
void pickvar();
void erase();
void add_not();
void add_and();
void step();
void gens();
void genp();
void get_ive();
void posneg();
void getin();
void getout();
void obt();

int cmp();
int get_range();
int check_num();
int check_sm();
int scan();
int check_sv_cov();
int seek_gc();
int check_sv();
int check_cov();
int check_de();
int rdd();
int in_num();
int check_reg();
int check_not();
int check_clock_not();
int check_reset_not();
int check_ive_num();
int check_and();
int reg_num();
int not_num();
int and_num();
int imp_num();
int eff_reset();
int tsg_reg_num();
int match();
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

char *in_name();
char *reg_str();