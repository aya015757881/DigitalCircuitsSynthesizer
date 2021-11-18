#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "synthesizer.h"

int main()
{
	while (1) {
		extract();
		find_var();
		find_sm();
		find_pa();
		find_bl();
		find_gc();
		find_smc();
		find_sv();
		find_ee();
		form_rv();
		form_de();
		form_tt();
		form_bf();
		form_nl();
		display();
		clear();
	}

    return 0;
}

void extract()
{
	int i;
	
	for (i = 0; i < 10000; ++i)
		a[i] = 0;
	
	printf("Please enter the name of the RTL level Verilog source file: ");

s1: gets(file);
	
	if ((fp = fopen(file, "r")) == NULL) {
		printf("\nFile %s does not exit, please reenter the file name: ", file);
		goto s1;
	}

	for (i = 0; (a[i] = fgetc(fp)) != EOF; ++i);
	a[i] = 0;
	fclose(fp);
	
	printf("\nNetlist for %s is being generated, please wait...\n", file);
}

void find_var()
{
	count_in = count_out = 0;
	in = &in_head;
	out = &out_head;
	input();
	inout();
	reg();
}

void find_sm()
{
	char c[20];
	int i, j;
	
    for(i = 0; !cmp(&a[i], "case(") && a[i]; ++i);
	for(i += 5, j = 0; a[i] != ')'; ++i, ++j)
		sm.name[j] = a[i];

	sm.name[j] = 0;
	strcpy(c, "case(");
	strcat(c, sm.name);
	strcat(c, ")");
	first(c);
	sm.start = n;
	strcpy(c, "endcase");
	first(c);
	sm.end = n;
}

void find_pa()
{
	int i, j;
	
    for (n = 0; !cmp(&a[n], "parameter ") && a[n]; ++n);
	
    n1 = n + 10;
	
    for(; !cmp(&a[n], ";") && a[n]; n++);
	
    n2 = n;
	
    for (n = n1, pa = &pa_head; n < n2; next_pa()) {
		
        pa->p = (PA*)calloc(1, sizeof(PA));
		
        for (i = n, j = 0; a[i] != ' ' && a[i] != '='; ++i, ++j)
			pa->p->name[j] = a[i];
		
        pa->p->name[j] = 0;
		
        for (i = n; a[i] != ',' && a[i] != ';'; ++i);
		
        for (--i, val = &pa->p->val_head; a[i] == '0' || a[i] == '1'; --i) {
			val->p = (NUM*)calloc(1, sizeof(NUM));
			val->p->n = a[i] - 48;
			val = val->p;
		}

		pa = pa->p;
	}
}

void find_bl()
{
	int i;
	bl = &bl_head;
	
    for (first("begin"); a[n]; next("begin")) {
		
        bl->p = (BL*)calloc(1, sizeof(BL));
		bl->p->start = n;
		count = 0;
		
        for (i = n + 5; a[i]; ++i) {
			if (cmp(&a[i], "begin"))
                ++count;
			else if (cmp(&a[i], "end")) {
				if (count != 0)
                    --count;
				else {
                    bl->p->end = i + 2;
                    break;
                }
			}
		}

		bl = bl->p;
	}
}

void find_gc()
{
	char name[20];
	int i, j;
	gc = &gc_head;
	
    for (first("if("); a[n]; next("if(")) {
		
        gc->p = (GC*)calloc(1, sizeof(GC));
		
        if (a[n + 3] != '!') {
            i = n + 3;
            gc->p->st = 1;
        } else {
            i = n + 4;
            gc->p->st = 0;
        }
		
        for (j = 0; a[i] != ')'; ++i, ++j)
			name[j] = a[i];
		
        name[j] = 0;
		
        for (in = &in_head; in->p != NULL; in = in->p)
			if (!strcmp(name, in->p->name)) {
                gc->p->in_p = in->p;
                break;
            }

		i = get_range();
		
        for (++i; a[i] < 'a' || a[i] > 'z'; ++i);

		if (cmp(&a[i], "else"))
            get_else(i, in->p, gc->p->st);

		gc = gc->p;
	}
}

void find_smc()
{
	int i, j, temp;
	char c[20];
	
    for (pa = &pa_head, count = 0; pa->p != NULL; pa = pa->p, ++count) {
		strcpy(c, pa->p->name);
		strcat(c, ":");
		for (first(c); a[n - 1] >= 'a' && a[n - 1] <= 'z'; next(c));
		pa->p->start = n;
	}

	p = (int*)calloc(count, sizeof(int));
	
    for (pa = &pa_head, i = 0; pa->p != NULL; pa = pa->p, i++)
		*(p + i) = pa->p->start;
	
    for (i = 0; i <= count - 2; ++i)
		for (j = 0; j <= count - 2 - i; ++j)
			if (*(p + j) > *(p + j + 1)) {
				temp = *(p + j);
				*(p + j) = *(p + j + 1);
				*(p + j + 1) = temp;
			}
	
    for (pa = &pa_head; pa->p != NULL; pa = pa->p)
		for (i = 0; i <= count - 1; ++i)
			if (*(p + i) == pa->p->start) {
				if (i != count - 1)
                    pa->p->end = *(p + i + 1) - 1;
				else {
                    first("endcase");
                    pa->p->end = n - 1;
                }
			}
	
    free(p);
}

void find_sv()
{
	sv = &sv_head;
	edge("posedge");
	edge("negedge");
	dif_spe();
	get_clock();
}

void find_ee()
{
	int i, idx, s;
	count = 0;
	
    for (out = &out_head; out->p != NULL; out = out->p) {
		
        ee1 = &out->p->ee1_head;
		ee2 = &out->p->ee2_head;
		
        if (!cmp(out->p->name, sm.name)) {
			
            if (check_num(out->p->name)) {
				
                for (i = 0; out->p->name[i] != '['; ++i)
					ee_name[i] = out->p->name[i];
				
                ee_name[i] = 0;
				
                for (; out->p->name[i] != ']'; ++i);
				
                for (idx = 0, --i, s = 1; out->p->name[i] != '['; --i, s *= 10)
					idx += s * (out->p->name[i] - 48);
				
                strcat(ee_name, "<=");
				
                ee_name_len = strlen(ee_name);
				
                ge_ee(idx);
			}

			strcpy(ee_name, out->p->name);
			strcat(ee_name, "<=");
			
            ee_name_len = strlen(ee_name);
			
            ge_ee(-1);

		} else {
            strcpy(ee_name, sm.name);
			strcat(ee_name, "<=");
			ee_name_len = strlen(ee_name);
			sm_ee();
            ++count;
		}
	}
}

void form_rv()
{
	for (out = &out_head; out->p != NULL; out = out->p) {
		
        rv = &out->p->rv_head;
		
        if (cmp(out->p->name, sm.name) || check_sm())
            smcv();
		
        gcv();
		asnv();
		sfv();
	}
}

void form_de()
{
	for (out = &out_head; out->p != NULL; out = out->p) {
		
        de = &out->p->de_head;
		
        for (ee1 = &out->p->ee1_head; ee1->p != NULL; ee1 = ee1->p) {
			n = ee1->p->addr;
			ee1_de(0);
			ee1_de(1);
		}

		for (ee2 = &out->p->ee2_head; ee2->p != NULL; ee2 = ee2->p) {
			n = ee2->p->addr;
			ee2_de();
		}
	}
}

void form_tt()
{
	for (out = &out_head; out->p != NULL; out = out->p) {
		for (rv = &out->p->rv_head, n = 0; rv->p != NULL; rv = rv->p, n++);
		count = (int)pow(2, n);
		tt = out->p->tt = (TT*)calloc(count, sizeof(TT));
		p = (int*)calloc(n, sizeof(int));
		pre_asn();
		de_asn();
		free(p);
	}
}

void form_bf()
{
	for (out = &out_head; out->p != NULL; out = out->p) {
		
        tt = out->p->tt;
		imp = &out->p->bf.imp_head;
		for (rv = &out->p->rv_head, n = 0; rv->p != NULL; rv = rv->p, ++n);
		p = (int*)calloc(n, sizeof(int));
		count = (int)pow(2, n);
		
        if ((out->p->bf.idx = scan()) == 2) {
			combine();
			sieve();
		}

		free(p);
	}
}

void form_nl()
{
	get_not();
	get_and();
	connect_imp_gate();
	get_or();
	connect_out_gate();
	get_eff_reset();
	get_cp_rst();
	get_nstring(clock->name);
	get_nstring(reset->name);
	get_tsg();
	get_op();
}

void display()
{
	int i;
	
	strcpy(filectt, file);
	for (i = 0; filectt[i] && filectt[i] != '.'; ++i);
	filectt[i] = '\0';
	strcat(filectt, "_netlist.v");
    fp = fopen(filectt, "w");

	fprintf(fp, "`include \"D_trigger.v\"\n");
	
    first("module");
	
    while (a[n] != ';')
        fputc(a[n++], fp);

	fprintf(fp, ";\n");

	for (first("input"); a[n]; next("input")) {
		while (a[n] != ';')
            fputc(a[n++], fp);
		fprintf(fp, ";\n");
	}

	for (first("inout"); a[n]; next("inout")) {
		while (a[n]!=';')
            fputc(a[n++],fp);
		fprintf(fp,";\n");
	}

	for (first("output"); a[n]; next("output")) {
		while (a[n]!=';')
            fputc(a[n++], fp);
		fprintf(fp, ";\n");
	}

	fprintf(fp, "\nD_trigger\n");
	
    for (out = &out_head; out->p != NULL; out = out->p) {
		
        fprintf(fp, "D%d(", out->p->num);

		if (out->p->clear_type == 2)
            fprintf(fp, "1'b1, 1'b1, ");
		else if (out->p->clear_type) {
			if (!out->p->clear_edge)
                fprintf(fp, "1'b1, %s, ", reset->name);
			else
                fprintf(fp, "1'b1, %s, ", reset_not);
		} else {
			if (!out->p->clear_edge)
                fprintf(fp, "%s, 1'b1, ", reset->name);
			else
                fprintf(fp,"%s,1'b1, ", reset_not);
		}

		if (out->p->clock_edge)
            fprintf(fp, "%s, ", clock->name);
		else
            fprintf(fp, "%s, ", clock_not);
		
        if (out->p->idx == -1)
            fprintf(fp, "1'b%d, ", out->p->dnum);
		else if (!out->p->idx)
            fprintf(fp, "%s, ", in_name(out->p->dnum));
		else if (out->p->idx == 1)
            fprintf(fp, "%s, ", reg_str(out->p->dnum));
		else if(out->p->idx == 2)
            fprintf(fp, "qnout%d, ", out->p->dnum);
		else if (out->p->idx == 3)
            fprintf(fp, "notout%d, ", out->p->dnum);
		else if (out->p->idx == 4)
            fprintf(fp, "andout%d, ", out->p->dnum);
		else
            fprintf(fp, "orout%d, ", out->p->dnum);
		
        fprintf(fp, "%s,qnout%d ", reg_str(out->p->num), out->p->num);
		
        if (out->p->p != NULL)
            fprintf(fp, "),\n");
		else
            fprintf(fp, ");\n\n");
	}

	fprintf(fp, "not\n");
	
    for (not_ = &not_head; not_->p != NULL; not_ = not_->p) {
		fprintf(fp, "not%d(notout%d, %s)", not_->p->num, not_->p->num, in_name(not_->p->in));
		if (not_->p->p != NULL)
            fprintf(fp, ",\n");
		else
            fprintf(fp, ";\n\n");
	}

	fprintf(fp, "and\n");
	
    for (and_ = &and_head; and_->p != NULL; and_ = and_->p) {
		
        fprintf(fp, "and%d(andout%d", and_->p->num, and_->p->num);
		
        for (iu = &and_->p->in_head; iu->p != NULL; iu = iu->p)
			fprintf(fp, ", %s", in_name(iu->p->n));

		for (ru = &and_->p->r_head; ru->p != NULL; ru = ru->p)
			fprintf(fp, ", %s", reg_str(ru->p->n));

		for (rnu = &and_->p->rn_head; rnu->p != NULL; rnu = rnu->p)
			fprintf(fp, ", qnout%d", rnu->p->n);

		for (nu = &and_->p->not_head; nu->p != NULL; nu = nu->p)
			fprintf(fp, ", notout%d", nu->p->n);

		if (and_->p->p != NULL)
            fprintf(fp, "),\n");
		else
            fprintf(fp, ");\n\n");
	}

	fprintf(fp, "or\n");
	
    for (or_ = &or_head; or_->p != NULL; or_ = or_->p) {
		
        fprintf(fp, "or%d(orout%d", or_->p->num, or_->p->num);

		for (iu = &or_->p->in_head; iu->p != NULL; iu = iu->p)
			fprintf(fp, ", %s", in_name(iu->p->n));

		for (ru = &or_->p->r_head; ru->p != NULL; ru = ru->p)
			fprintf(fp,", %s", reg_str(ru->p->n));
		
        for (rnu = &or_->p->rn_head; rnu->p != NULL; rnu = rnu->p)
			fprintf(fp, ", qnout%d", rnu->p->n);

		for (nu = &or_->p->not_head; nu->p != NULL; nu = nu->p)
			fprintf(fp, ", notout%d", nu->p->n);

		for (au = &or_->p->and_head; au->p != NULL; au = au->p)
			fprintf(fp, ", andout%d", au->p->n);

		if (or_->p->p != NULL)
            fprintf(fp, "),\n");
		else
            fprintf(fp, ");\n\n");
	}

	fprintf(fp, "bufif1\n");
	
    for (tsg = &tsg_head; tsg->p != NULL; tsg = tsg->p) {
		
        fprintf(fp, "TSG%d(%s, ", tsg->p->num, tsg->p->out_name);
		
        if (tsg->p->d_idx == -1)
            fprintf(fp, "1'b%d, ", tsg->p->data);
		else
            fprintf(fp, "%s, ", reg_str(tsg->p->data));
		
        fprintf(fp, "%s ", reg_str(tsg->p->ena));
		
        if (tsg->p->p != NULL)
            fprintf(fp, "),\n");
		else
            fprintf(fp, ");\n");
	}

	fprintf(fp, "endmodule\n");
	fclose(fp);
	printf("Netlist for %s has been generated.\n\n\n\n", file);
}

void clear()
{
	in = &in_head;
	out = &out_head;
	num = &num_head;
	pa = &pa_head;
	bl = &bl_head;
	gc = &gc_head;
	sv = &sv_head;
	not_ = &not_head;
	and_ = &and_head;
	or_ = &or_head;
	er = &er_head;
	tsg = &tsg_head;
	op = &op_head;
	in->p = NULL;
	out->p = NULL;
	num->p = NULL;
	pa->p = NULL;
	bl->p = NULL;
	gc->p = NULL;
	sv->p = NULL;
	not_->p = NULL;
	and_->p = NULL;
	or_->p = NULL;
	er->p = NULL;
	tsg->p = NULL;
	op->p = NULL;
}

void input()
{
	sg("input ");
	pr("input[");
}

void inout()
{
	sg("inout ");
	pr("inout[");
}

void reg()
{
	sg("reg ");
	pr("reg[");
}

void next_pa()
{
	for (; a[n] != ',' && a[n] != ';'; ++n);
	for (; a[n] < 'a' || a[n] > 'z'; ++n);
}

int get_range()
{
	int i;
	for (i = n; a[i] != ')'; ++i);
	for (; a[i] < 'a' || a[i] > 'z'; ++i);
	
    gc->p->start = i;
	
    range_type(i);
	
    return gc->p->end;
}

void get_else(int i, IN *in_p, int st)
{
	gc = gc->p;
	gc->p = (GC*)calloc(1, sizeof(GC));
	gc->p->in_p = in_p;
	
    if (st == 0)
        gc->p->st = 1;
	else
        gc->p->st = 0;
	
    for (i += 4; a[i] < 'a'|| a[i] > 'z'; ++i);
	
    gc->p->start = i;
	
    range_type(i);
}

void edge(const char *c)
{
	int i, j;
	char name[20];
	
    for (first(c); a[n]; next(c)) {
		for (i = n + strlen(c); a[i] < 'a' || a[i] > 'z'; ++i);
		for (j = 0; a[i] >= 'a' && a[i] <= 'z'; ++i, ++j)
			name[j] = a[i];
		name[j] = 0;
		if (!check_sv_cov(name)) {
			sv->p = (SV*)calloc(1, sizeof(SV));
			strcpy(sv->p->name, name);
			sv = sv->p;
		}
	}
}

void dif_spe()
{
	for (sv = &sv_head; sv->p != NULL; sv = sv->p)
		if (seek_gc(sv->p->name)) {
			for (in = &in_head; in->p != NULL; in = in->p)
				if (!strcmp(in->p->name, sv->p->name))
                    reset = in->p;
		} else
			for (in = &in_head; in->p != NULL; in = in->p)
				if (!strcmp(in->p->name, sv->p->name))
                    clock = in->p;
}

void get_clock()
{
	find_al();
	clock_edg("posedge ");
	clock_edg("negedge ");
}

void ge_ee(int idx)
{
	for (first(ee_name); a[n]; next(ee_name)) {
		if (a[n + ee_name_len] == '~' ||
            a[n + ee_name_len] >= 'a' &&
            a[n + ee_name_len] <= 'z'
        )
            gen_ee1(idx);
		else
            gen_ee2(idx);
    }
}

void sm_ee()
{
	int i;
	
    for (first(ee_name); a[n]; next(ee_name)) {
		ee2->p = (EE2*)calloc(1, sizeof(EE2));
		ee2->p->addr = n;
		for (pa = &pa_head; pa->p != NULL; pa = pa->p)
			if (cmp(&a[n + ee_name_len], pa->p->name)) {
				for (i = 0, val = &pa->p->val_head; i < count; val = val->p, ++i);
				    ee2->p->val = val->p->n;
			}
		ee2 = ee2->p;
	}
}

int check_sm()
{
	int i;
	char c[20];
	
    strcpy(c, out->p->name);
	strcat(c, "<=");
	
    for (first(c); a[n]; next(c))
		if (n >= sm.start && n <= sm.end)
            return 1;
	
    if (check_num(out->p->name)) {
		for (i = 0; out->p->name[i] != '['; ++i)
			c[i] = out->p->name[i];
		c[i] = 0;
		strcat(c, "<=");
		for (first(c); a[n]; next(c))
			if (n >= sm.start && n <= sm.end)
                return 1;
	}

	return 0;
}

void smcv()
{
	for (in = &in_head; in->p != NULL; in = in->p)
		if (cmp(in->p->name, sm.name)) {
			rv->p = (RV*)calloc(1,sizeof(RV));
			rv->p->in_p = in->p;
			rv = rv->p;
		}
}

void gcv()
{
	for (ee1 = &out->p->ee1_head; ee1->p != NULL; ee1 = ee1->p) {
		n = ee1->p->addr;
		add_gcv();
	}

	for (ee2 = &out->p->ee2_head; ee2->p != NULL; ee2 = ee2->p) {
		n = ee2->p->addr;
		add_gcv();
	}
}

void asnv()
{
	for (ee1 = &out->p->ee1_head; ee1->p != NULL; ee1 = ee1->p) {
		strcpy(v_name, ee1->p->in_p->name);
		if (check_sv())
            continue;
		else if(check_cov())
            continue;
		else {
			rv->p = (RV*)calloc(1, sizeof(RV));
			rv->p->in_p = ee1->p->in_p;
			rv = rv->p;
		}
	}
}

void sfv()
{
	for (in = &in_head; in->p != NULL; in = in->p)
		if (!strcmp(in->p->name, out->p->name)) {
			strcpy(v_name, in->p->name);
			if (!check_cov()) {
				rv->p = (RV*)calloc(1, sizeof(RV));
				rv->p->in_p = in->p;
				rv = rv->p;
			}
		}
}

void ee1_de(int val)
{
	de->p = (DE*)calloc(1, sizeof(DE));
	de->p->st = val;
	ive = &de->p->ive_head;
	asn_de(val);
	gc_de();
	pa_de();
	de = de->p;
}

void ee2_de()
{
	de->p = (DE*)calloc(1, sizeof(DE));
	de->p->st = ee2->p->val;
	ive = &de->p->ive_head;
	gc_de();
	pa_de();
	de = de->p;
}

void pre_asn()
{
	int i, j, k, val;
	
    for (rv = &out->p->rv_head, i = 0; rv->p != NULL; rv = rv->p, ++i)
		if (!strcmp(rv->p->in_p->name, out->p->name))
            break;
	
    for (j = 0; j <= count - 1; ++j) {
		for (val = j, k = 0; k < i; val >>= 1, ++k);
		    (tt + j)->val = val % 2;
	}
}

void de_asn()
{
	int i;
	
    for (i = 0; i <= count - 1; ++i) {
		get_bn(i);
		for (de = &out->p->de_head; de->p != NULL; de = de->p)
			if (check_de()) {
                (tt + i)->val = de->p->st;
                break;
            }
	}
}

int scan()
{
	int i, zero = 0, one = 0;
	
    for (i = 0; i < count; ++i) {
		if ((tt + i)->val == 0)
            zero = 1;
		if ((tt + i)->val == 1)
            one = 1;
		if (zero && one)
            return 2;
	}

	if (zero)
        return 0;
	else
        return 1;
}

void combine()
{
	int i;
	for (i = 1; i <= n; ++i) {
		qua = i;
		ive1 = (IVE*)calloc(i, sizeof(IVE));
		pickvar(0, 0, n - i);
		free(ive1);
		if (allcov)
            break;
	}
}

void sieve()
{
	for (imp = &out->p->bf.imp_head; imp->p != NULL;)
		if (rdd(imp)) {
			erase();
			imp->p = imp->p->p;
		} else
            imp =imp->p;
}

void get_not()
{
	int num;
	not_ = &not_head;
	
    for (count = 0, out = &out_head; out->p != NULL; out = out->p)
		for (imp = &out->p->bf.imp_head;imp->p != NULL; imp = imp->p)
			for (ive = &imp->p->ive_head; ive->p != NULL; ive = ive->p)
				if (!ive->p->st &&
                    !check_reg() &&
                    !check_not(num = in_num())
                )
                    add_not(num);
	
    if (check_clock_not())
        add_not(clock->num);
	
    if (check_reset_not())
        add_not(reset->num);
}

void get_and()
{
	and_ = &and_head;
	
    for (count = 0, out = &out_head; out->p != NULL; out = out->p)
		for(imp = &out->p->bf.imp_head; imp->p != NULL; imp = imp->p)
			if (check_ive_num() > 1 && !check_and())
                add_and();
}

void connect_imp_gate()
{
	for (out = &out_head; out->p != NULL; out = out->p) {

		for (imp = &out->p->bf.imp_head; imp->p != NULL; imp = imp->p) {
			
            if (check_ive_num() == 1) {
				
                ive = &imp->p->ive_head;
				
                if (!check_reg()) {
					if (ive->p->st)
                        imp->p->num = in_num();
					else {
						imp->p->idx = 3;
						imp->p->num = not_num();
					}
				} else {
					imp->p->num = reg_num();
					if (ive->p->st)
                        imp->p->idx = 1;
					else
                        imp->p->idx = 2;
				}
			} else {
				imp->p->idx = 4;
				imp->p->num = and_num();
			}
        }
    }
}

void get_or()
{
	int num;

	for (count = 0, or_ = &or_head, out = &out_head; out->p != NULL; out = out->p) {
		
        if (imp_num() > 1) {
			
            or_->p = (OR*)calloc(1, sizeof(OR));
			or_->p->num = count++;
			iu = &or_->p->in_head;
			ru = &or_->p->r_head;
			rnu = &or_->p->rn_head;
			nu = &or_->p->not_head;
			au = &or_->p->and_head;
			
            for (imp = &out->p->bf.imp_head; imp->p != NULL; imp = imp->p) {
				
                num = imp->p->num;
				
                if (!imp->p->idx) {
					iu->p = (NUM*)calloc(1, sizeof(NUM));
					iu->p->n=num;
					iu = iu->p;
				} else if (imp->p->idx == 1) {
					ru->p = (NUM*)calloc(1, sizeof(NUM));
					ru->p->n = num;
					ru = ru->p;
				} else if(imp->p->idx == 2) {
					rnu->p = (NUM*)calloc(1, sizeof(NUM));
					rnu->p->n = num;
					rnu = rnu->p;
				} else if (imp->p->idx == 3) {
					nu->p = (NUM*)calloc(1, sizeof(NUM));
					nu->p->n = num;
					nu = nu->p;
				} else {
					au->p = (NUM*)calloc(1, sizeof(NUM));
					au->p->n = num;
					au = au->p;
				}
			}

			out->p->idx = 5;
			out->p->dnum = or_->p->num;
			or_ = or_->p;
		}
    }
}

void connect_out_gate()
{
	for (out = &out_head; out->p != NULL; out = out->p) {
		if (!imp_num()) {
			out->p->idx = -1;
			out->p->dnum = out->p->bf.idx;
		} else if (imp_num() == 1) {
			imp = &out->p->bf.imp_head;
			out->p->idx = imp->p->idx;
			out->p->dnum = imp->p->num;
		}
    }
}

void get_eff_reset()
{
	er = &er_head;
	
    for (gc = &gc_head; gc->p != NULL; gc = gc->p) {
		if (!strcmp(gc->p->in_p->name, reset->name) && eff_reset()) {
			er->p = (ER*)calloc(1, sizeof(ER));
			er->p->st = gc->p->st;
			er->p->start = gc->p->start;
			er->p->end = gc->p->end;
			er = er->p;
		}
    }
}

void get_cp_rst()
{
	int addr;
	
    for (out = &out_head; out->p != NULL; out = out->p) {
		
        out->p->clear_type = 2;
		ee1 = &out->p->ee1_head;
		ee2 = &out->p->ee2_head;
		addr = ee1->p != NULL ? ee1->p->addr : ee2->p->addr;
		
        for (gc = &gc_head; gc->p != NULL; gc = gc->p) {
			if (!strcmp(gc->p->in_p->name, clock->name) &&
                addr >= gc->p->start &&
                addr <= gc->p->end
            ) {
				out->p->clock_edge = gc->p->st;
				break;
			}
        }

		for (; ee2->p!=NULL; ee2=ee2->p) {
            addr = ee2->p->addr;
            for (er = &er_head; er->p != NULL; er = er->p)
				if (addr >= er->p->start && addr <= er->p->end) {
					out->p->clear_edge = er->p->st;
					out->p->clear_type = ee2->p->val;
					goto s1;
				}
		}
s1:;
	}
}

void get_nstring(char *name)
{
	int i, val;
	char temp[20] = "notout", num[10];

	for (not_= &not_head; not_->p != NULL; not_ = not_->p) {
		if (!strcmp(in_name(not_->p->in), name)) {
			if (!not_->p->num)
                strcat(temp, "0");
			else {
				for (val = not_->p->num, i = 0; val != 0; val /= 10, i++)
					num[i] = val % 10 + 48;
				num[i] = 0;
				strrev(num);
				strcat(temp,num);
			}
		}
    }

	if (!strcmp(clock->name, name))
        strcpy(clock_not, temp);

	if (!strcmp(reset->name, name))
        strcpy(reset_not, temp);
}

void get_tsg()
{
	int i, j;
	
    for (first("assign"), tsg = &tsg_head, count = 0; a[n]; next("assign"), tsg = tsg->p, ++count) {
		
        tsg->p = (TSG*)calloc(1, sizeof(TSG));
		tsg->p->num = count;
		
        for (i = n + strlen("assign"), j = 0; a[i] != '='; ++i)
			if (a[i] >= 'a' && a[i] <= 'z') {
				tsg->p->out_name[j] = a[i];
				j++;
			}

		tsg->p->out_name[j] = 0;
		tsg->p->ena = tsg_reg_num(++i, '?');
		
        while (a[i++] != '?');
		
        if (a[i] == '0' || a[i] == '1') {
			tsg->p->d_idx = -1;
			tsg->p->data = a[i] - 48;
		} else
            tsg->p->data = tsg_reg_num(i,':');
	}
}

void get_op()
{
	int i;
	char temp[20];
	op = &op_head;
	
    for (first("output "), n += strlen("output "); a[n - 1] != ';'; step()) {
		
        op->p = (OP*)calloc(1, sizeof(OP));
		
        for (i = 0; a[n] != ',' && a[n] != ';'; ++n, ++i)
			temp[i] = a[n];
		
        temp[i] = 0;

        strcpy(op->p->name, temp);

		for (out = &out_head; out->p != NULL; out = out->p)
			if (!strcmp(out->p->name, temp))
                op->p->num = out->p->num;
		
        op = op->p;
	}
}

char *reg_str(int num)
{
	int i;
	char temp[10];
	
    for (op = &op_head; op->p != NULL; op = op->p)
		if (op->p->num == num)
            return
                op->p->name;
	
    if (!num)
        strcpy(reg_string, "qout0");
	else {
		for (i = 0; num != 0; num /= 10, ++i)
			temp[i] = num % 10 + 48;
		temp[i] = 0;
		strrev(temp);
		strcpy(reg_string, "qout");
		strcat(reg_string, temp);
	}

	return reg_string;
}

void sg(const char *c)
{
	int i, ind;
	
    if (!strcmp(c, "reg "))
        ind=0;
	else
        ind = 1;

	for (i = 0; a[i]; ++i)
		if (cmp(&a[i], c))
            break;

	for(n = i + strlen(c); a[n - 1] != ';'; step())
		gens(ind);
}

void pr(const char *c)
{
	int i, ind, s, sum, len;
	
    if (!strcmp(c, "reg["))
        ind=0;
	else
        ind=1;
	
    for (i = 0, len = strlen(c), num = &num_head; !(a[i] == '\n' && a[i - 1] == '\n'); ++i)
		if (cmp(&a[i], c)) {
			num->p = (NUM*)calloc(1,sizeof(NUM));
			num = num->p;
			num->n = i + len;
		}

	num->p = NULL;
	
    for (num = &num_head; num->p != NULL; num = num->p) {
		
        for (n = num->p->n; a[n] != ':'; ++n);
		
        for(--n, sum = 0, s = 1; a[n] != '['; --n, s *= 10)
			sum += (a[n] - 48) * s;
		
        n1 = sum;
		
        for (; a[n] != ']'; ++n);

		for (--n, sum = 0, s = 1; a[n] != ':'; --n, s *= 10)
			sum += (a[n] - 48) * s;
		
        n2 = sum;
		
        for (; a[n] != ' '; ++n);

		for (++n; a[n - 1] != ';'; step())
			genp(ind);
	}

	num = &num_head;
	num->p = NULL;
}

void range_type(int i)
{
	if (cmp(&a[i], "begin")) {
		for (bl = &bl_head; bl->p != NULL; bl = bl->p)
			if (bl->p->start == i) {
                gc->p->end = bl->p->end;
                break;
            }
	} else if (cmp(&a[i], "case"))
        gc->p->end=sm.end;
	else {
		for (; a[i] != ';'; ++i);
		    gc->p->end = i;
	}
}

int check_sv_cov(char *name)
{
	for (sv1 = &sv_head; sv1->p != NULL; sv1 = sv1->p)
		if (!strcmp(sv1->p->name, name))
            return 1;

	return 0;
}

int seek_gc(char *c)
{
	for (gc1 = &gc_head; gc1->p != NULL; gc1 = gc1->p)
		if (!strcmp(gc1->p->in_p->name, c))
            return 1;

	return 0;
}

void find_al()
{
	int i;
	al = &al_head;
	
    for (first("always"); a[n]; next("always")) {
		al->p = (BL*)calloc(1, sizeof(BL));
		al->p->start = n;
		for (i = n + 1; !(cmp(&a[i], "always") || cmp(&a[i], "endmodule")); ++i);
		al->p->end = i - 1;
		al = al->p;
	}
}

void clock_edg(const char *edg)
{
	char c[20];
	
    strcpy(c, edg);
	strcat(c, clock->name);
	
    for (first(c); a[n]; next(c)) {
		gc->p = (GC*)calloc(1, sizeof(GC));
		for (al = &al_head; al->p != NULL; al = al->p)
			if (n >= al->p->start && n <= al->p->end) {
				for (in = &in_head; in->p != NULL; in = in->p)
					if (!strcmp(in->p->name, clock->name))
                        gc->p->in_p = in->p;
				gc->p->start = al->p->start;
				gc->p->end = al->p->end;
			}

		if (!strcmp(edg, "posedge "))
            gc->p->st = 1;
		else
            gc->p->st = 0;
		gc = gc->p;
	}
}

void gen_ee1(int idx)
{
	int i, j;
	char name[20], ch[20];
	
    ee1->p = (EE1*)calloc(1, sizeof(EE1));
	ee1->p->addr = n;
	
    if (idx == -1) {

		if (a[n + ee_name_len] == '~') {
            ee1->p->in_st = 0;
            i = n + ee_name_len + 1;
        } else {
            ee1->p->in_st = 1;
            i = n + ee_name_len;
        }
		for (in = &in_head; in->p != NULL; in = in->p)
			if (cmp(&a[i], in->p->name))
                ee1->p->in_p=in->p;

	} else {

		ee1->p->in_st = 1;
		
        for (i = n + ee_name_len, j = 0; a[i] != ';'; ++i, ++j)
			name[j] = a[i];
		
        name[j] = '[';
		name[j + 1] = 0;
		
        if (!idx) {
            ch[0] = '0';
            ch[1] = 0;
        } else {
			for (j = 0; idx != 0; idx /= 10, ++j)
				ch[j] = idx % 10 + 48;
			ch[j] = 0;
			strrev(ch);
		}

		strcat(name, ch);
		strcat(name, "]");
		
        for (in = &in_head; in->p != NULL; in = in->p)
			if (!strcmp(name, in->p->name))
                ee1->p->in_p = in->p;
	}

	ee1 = ee1->p;
}

void gen_ee2(int idx)
{
	int i;
	ee2->p = (EE2*)calloc(1, sizeof(EE2));
	ee2->p->addr = n;
	i = n + ee_name_len;
	
    if (idx != -1) {
		for(; a[i] != ';'; ++i);
		for(--i; idx != 0; --idx, --i);
	}

	if (a[i] =='0')
        ee2->p->val = 0;
	else
        ee2->p->val = 1;
	
    ee2 = ee2->p;
}

int check_num(char *c)
{
	int i;
	
    for (i = 0; *(c + i); ++i)
		if (*(c + i) >= '0' && *(c + i) <= '9')
            return 1;
	
    return 0;
}

void add_gcv()
{
	for (gc = &gc_head; gc->p != NULL; gc = gc->p) {
		strcpy(v_name, gc->p->in_p->name);
		if (check_sv())
            continue;
		else if (check_cov())
            continue;
		else if (n < gc->p->start || n > gc->p->end)
            continue;
		else {
			rv->p = (RV*)calloc(1, sizeof(RV));
			rv->p->in_p = gc->p->in_p;
			rv = rv->p;
		}
	}
}

void asn_de(int val)
{
	strcpy(v_name, ee1->p->in_p->name);
	
    if (ee1->p->in_st)
        st = val;
	else
        st = !val;
	
    get_ive();
}

void gc_de()
{
	for (gc = &gc_head; gc->p != NULL; gc = gc->p)
		if (n >= gc->p->start && n <= gc->p->end) {
			strcpy(v_name, gc->p->in_p->name);
			if (!check_sv()) {
				st = gc->p->st;
				get_ive();
			}
		}	
}

void pa_de()
{
	int i;
	
    for (pa = &pa_head; pa->p != NULL; pa = pa->p)
		if (n >= pa->p->start && n <= pa->p->end) {
			for (val = &pa->p->val_head, i = 0; val->p != NULL; val = val->p, ++i) {
				for (in = &in_head, count = i; in->p != NULL; in = in->p) {
					if (cmp(in->p->name, sm.name))
						if (!count) {
                            strcpy(v_name, in->p->name);
                            break;
                        } else
                            --count;
				}
				st = val->p->n;
				get_ive();
			}
			break;
		}
}

int check_de()
{
	if (de->p->ive_head.p == NULL)
        return 0;
	else {
		for (ive = &de->p->ive_head; ive->p != NULL; ive=ive->p)
			if (*(p + ive->p->num) != ive->p->st)
                return 0;
		return 1;
	}
}

void pickvar(int idx, int start, int end)
{
	int a;
	
    for (a = start; a <= end; ++a) {
		(ive1 + idx)->num = a;
		if (end < n - 1)
            pickvar(idx + 1, a + 1, end + 1);
		else
            posneg();
		if (allcov)
            break;
	}
}

int rdd(IMP *a)
{
	int i;
	
    for (ive = &a->p->ive_head, i = 0; ive->p != NULL; ive = ive->p, ++i);
	
    qua = i;
	ive1 = (IVE*)calloc(qua, sizeof(IVE));
	
    for (ive = &a->p->ive_head, i = 0; ive->p != NULL; ive = ive->p, ++i) {
		(ive1 + i)->num = ive->p->num;
		(ive1 + i)->st = ive->p->st;
	}

	for (i = 0; i < count; ++i)
		if (match(i) && (tt + i)->idx)
            return 0;
	
    return 1;
}

void erase()
{
	int i;
	
    for (i = 0; i < count; ++i)
		if (match(i))
            --(tt + i)->idx;
}

int check_not(int num)
{
	for (not1 = &not_head; not1->p != NULL; not1 = not1->p)
		if (not1->p->in == num)
            return 1;
	
    return 0;
}

int check_clock_not()
{
	for (gc = &gc_head; gc->p != NULL; gc = gc->p)
		if (!strcmp(gc->p->in_p->name, clock->name) && !gc->p->st)
            return 1;
	
    return 0;
}

int check_reset_not()
{
	for (er = &er_head; er->p != NULL; er = er->p)
		if (er->p->st)
            return 1;
	
    return 0;
}

void add_not(int num)
{
	not_->p = (NOT*)calloc(1, sizeof(NOT));
	not_->p->num = count++;
	not_->p->in = num;
	not_ = not_->p;
}

int check_ive_num()
{
	int i;
	
    for (ive = &imp->p->ive_head, i = 0; ive->p != NULL; ive = ive->p, ++i);
	
    return i;
}

int check_and()
{
	for (and1 = &and_head; and1->p != NULL; and1 = and1->p)
		if (check_and_num() && check_and_var())
            return 1;

	return 0;
}

void add_and()
{
	and_->p = (AND*)calloc(1, sizeof(AND));
	and_->p->num = count++;
	iu = &and_->p->in_head;
	ru = &and_->p->r_head;
	rnu = &and_->p->rn_head;
	nu = &and_->p->not_head;
	
    for (ive = &imp->p->ive_head; ive->p != NULL; ive = ive->p) {
		if (!check_reg()) {
			if (ive->p->st) {
				iu->p = (NUM*)calloc(1, sizeof(NUM));
				iu->p->n = in_num();
				iu = iu->p;
			} else {
				nu->p = (NUM*)calloc(1, sizeof(NUM));
				nu->p->n = not_num();
				nu = nu->p;
			}
		} else {
			if (ive->p->st) {
				ru->p = (NUM*)calloc(1, sizeof(NUM));
				ru->p->n = reg_num();
				ru = ru->p;
			} else {
				rnu->p = (NUM*)calloc(1, sizeof(NUM));
				rnu->p->n = reg_num();
				rnu = rnu->p;
			}
		}
    }

	and_ = and_->p;
}

int and_num()
{
	for (and1 = &and_head; and1->p != NULL; and1 = and1->p)
		if (check_and_num() && check_and_var())
            return and1->p->num;
	
    return -1;
}

int imp_num()
{
	int i;
	
    for (imp = &out->p->bf.imp_head, i = 0; imp->p != NULL; imp = imp->p, ++i);
	
    return i;
}

int eff_reset()
{
	for (al = &al_head; al->p != NULL; al = al->p)
		if (gc->p->start >= al->p->start &&
            gc->p->end <= al->p->end &&
            gc->p->st == al_state()
        )
            return 1;
	
    return 0;
}

char *in_name(int num)
{
	int i;
	
    for (in = &in_head, i = 0; i < num; in = in->p, ++i);
	
    return in->p->name;
}

int tsg_reg_num(int start, char end)
{
	int i,j;
	char temp[20];
	
    for (i = start, j = 0; a[i] != end; ++i, ++j)
		temp[j] = a[i];
	
    temp[j] = 0;
	
    for (out = &out_head; out->p != NULL; out = out->p)
		if (!strcmp(out->p->name, temp))
            return out->p->num;
	
    return -1;
}

void step()
{
	int i;
	for (i = 0; a[n + i] != ',' && a[n + i] != ';'; ++i);
	n += i + 1;
}

void gens(int ind)
{
	if (ind == 1) {
		getin();
		in = in->p;
	} else {
		getin();
		in = in->p;
		getout();
		out = out->p;
	}
}

void genp(int ind)
{
	int i, j, p, q;
	char t, lett[20];
	lett[0] = '[';
	
    for (i = n2; i <= n1; ++i) {
		if (i == 0) {
			lett[1] = '0';
			lett[2] = ']';
			lett[3] = 0;
		} else {
			for (p = i, j = 1; p != 0; p /= 10, ++j)
				lett[j] = p % 10 + 48;
			for (p = 1, q = j - 1; p < q; ++p, --q) {
				t = lett[p];
				lett[p] = lett[q];
				lett[q] = t;
			}
			lett[j] = ']';
			lett[j + 1] = 0;
		}

		if (ind == 1) {
			getin();
			strcat(in->p->name, lett);
			in = in->p;
		} else {
			getin();
			strcat(in->p->name, lett);
			in = in->p;
			getout();
			strcat(out->p->name, lett);
			out = out->p;
		}
	}
}

void first(const char *c)
{
	for (n = 0; !cmp(&a[n], c) && a[n]; ++n);
}

void next(const char *c)
{
	for (++n; !cmp(&a[n], c) && a[n]; ++n);
}

int check_sv()
{
	for (sv = &sv_head; sv->p != NULL; sv = sv->p)
		if (!strcmp(sv->p->name, v_name))
            return 1;

	return 0;
}

int check_cov()
{
	for (rv1 = &out->p->rv_head; rv1->p != NULL; rv1 = rv1->p)
		if (!strcmp(rv1->p->in_p->name, v_name))
            return 1;
	
    return 0;
}

void get_ive()
{
	ive->p = (IVE*)calloc(1, sizeof(IVE));
	
    for (rv = &out->p->rv_head, count = 0; rv->p != NULL; rv = rv->p, ++count)
		if (!strcmp(rv->p->in_p->name, v_name))
            break;

	ive->p->num = count;
	ive->p->st = st;
	ive = ive->p;
}

void posneg()
{
	int i, j, val;
	
    for (i = 0; i < (int)pow(2, qua); ++i) {
		for (val = i,j = 0; j < qua; val /= 2, ++j)
			(ive1 + j)->st = val % 2;
		if (fil() && uncov())
            obt();
        if (allcov = cov())
            break;
	}
}

int check_and_num()
{
	int i1, i2, i3, i4, j1, j2, j3, j4;
	
    for (iu = &and1->p->in_head, i1 = 0; iu->p != NULL; iu = iu->p, ++i1);
	
    for (ru = &and1->p->r_head, i2 = 0; ru->p != NULL; ru = ru->p, ++i2);
	
    for (rnu = &and1->p->rn_head, i3 = 0; rnu->p != NULL; rnu = rnu->p, ++i3);
	
    for (nu = &and1->p->not_head, i4 = 0; nu->p != NULL; nu = nu->p, ++i4);
	
    for (ive = &imp->p->ive_head, j1 = j2 = j3 = j4 = 0; ive->p != NULL; ive = ive->p) {
		if (!check_reg()) {
			if (ive->p->st)
                ++j1;
			else
                ++j4;
		} else {
			if (ive->p->st)
                ++j2;
			else
                ++j3;
		}
	}

	if (i1 == j1 &&
        i2 == j2 &&
        i3 == j3 &&
        i4 == j4
    )
        return 1;
	else
        return 0;
}

int check_and_var()
{
	for (iu = &and1->p->in_head; iu->p != NULL; iu = iu->p)
		if (!check_in_st())
            return 0;

	for (ru = &and1->p->r_head; ru->p != NULL; ru = ru->p)
		if (!check_reg_st())
            return 0;

	for (rnu = &and1->p->rn_head; rnu->p != NULL; rnu = rnu->p)
		if (!check_regn_st())
            return 0;

	for (nu = &and1->p->not_head; nu->p != NULL; nu = nu->p)
		if (!check_not_st())
            return 0;

	return 1;
}

int al_state()
{
	char temp[30];
	
    strcpy(temp, "posedge ");
	strcat(temp, reset->name);
	
    for (n = al->p->start; a[n] != ')'; ++n)
		if (cmp(&a[n], temp))
            return 1;

	return 0;
}

void getin()
{
	int i, j;
	in->p = (IN*)calloc(1,sizeof(IN));
	in->p->num = ++count_in;
	
    for (i = n, j = 0; a[i] != ',' && a[i] != ';'; ++i, ++j)
		in->p->name[j] = a[i];
}

void getout()
{
	int i, j;
	
    out->p = (OUT*)calloc(1, sizeof(OUT));
	out->p->num = ++count_out;
	
    for (i = n, j = 0; a[i] != ',' && a[i] != ';'; ++i, ++j)
		out->p->name[j] = a[i];
}

int cmp(char *m, const char *t)
{
	int i;
	
    for (i = 0; *(t + i); i++)
		if (*(m + i) != *(t + i))
            return 0;
	
    return 1;
}

int fil()
{
	int i;
	
    for (i = 0; i < count; ++i)
		if (match(i) && !(tt + i)->val)
            return 0;

	return 1;
}

int uncov()
{
	int i;

	for (i = 0; i < count; ++i)
		if (match(i) && !(tt + i)->idx)
            return 1;
	
    return 0;
}

void obt()
{
	int i;
	
    for (i = 0; i < count; ++i)
		if (match(i))
            ++(tt + i)->idx;

	imp->p = (IMP*)calloc(1, sizeof(IMP));
	ive = &imp->p->ive_head;

	for (i = 0; i < qua; ++i) {
		ive->p = (IVE*)calloc(1, sizeof(IVE));
		ive->p->num = (ive1 + i)->num;
		ive->p->st = (ive1 + i)->st;
		ive = ive->p;
	}

	imp = imp->p;
}

int cov()
{
	int i;
	
    for (i = 0; i < count; ++i)
		if ((tt + i)->val && !(tt + i)->idx)
            return 0;

	return 1;
}

int check_in_st()
{
	for (ive = &imp->p->ive_head; ive->p != NULL; ive = ive->p)
		if (!check_reg() && ive->p->st && in_num() == iu->p->n)
            return 1;
	
    return 0;
}

int check_reg_st()
{
	for (ive = &imp->p->ive_head; ive->p != NULL; ive = ive->p)
		if (check_reg() && ive->p->st && reg_num() == ru->p->n)
            return 1;
	
    return 0;
}

int check_regn_st()
{
	for (ive = &imp->p->ive_head; ive->p != NULL; ive = ive->p)
		if (check_reg() && !ive->p->st && reg_num() == rnu->p->n)
            return 1;

	return 0;
}

int check_not_st()
{
	for (ive = &imp->p->ive_head; ive->p != NULL; ive = ive->p)
		if (!check_reg() && !ive->p->st && not_num() == nu->p->n)
            return 1;

	return 0;
}

int match(int val)
{
	int i;
	
    get_bn(val);
	
    for (i = 0; i < qua; ++i)
		if (*(p + (ive1 + i)->num) != (ive1 + i)->st)
            return 0;

	return 1;
}

int check_reg()
{
	int i;
	
    for (rv = &out->p->rv_head, i = 0; i < ive->p->num; rv = rv->p, ++i);
	
    for (out1 = &out_head; out1->p != NULL; out1 = out1->p)
		if (!strcmp(out1->p->name, rv->p->in_p->name))
            return 1;

	return 0;
}

int reg_num()
{
	int i;
	
    for (rv = &out->p->rv_head, i = 0; i < ive->p->num; rv = rv->p, ++i);
	
    for (out1 = &out_head; out1->p != NULL; out1 = out1->p)
		if (!strcmp(out1->p->name, rv->p->in_p->name))
            return out1->p->num;

	return -1;
}

int not_num()
{
	int num = in_num();
	
    for (not_= &not_head; not_->p != NULL; not_ = not_->p)
		if (not_->p->in == num)
            return not_->p->num;
	
    return -1;
}

void get_bn(int val)
{
	int i;

	for (i = 0; i < n; val >>= 1, ++i)
		*(p + i) = val % 2;
}

int in_num()
{
	int i;
	
    for (rv = &out->p->rv_head, i = 0; i < ive->p->num; rv = rv->p, ++i);
	
    return rv->p->in_p->num;
}




