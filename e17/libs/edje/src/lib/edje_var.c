#include "Edje.h"
#include "edje_private.h"

static int _edje_var_timer_cb(void *data);
static int _edje_var_anim_cb(void *data);

static Ecore_Animator *_edje_animator = NULL;
static Evas_List   *_edje_anim_list = NULL;

static int
_edje_var_timer_cb(void *data)
{
   Edje_Var_Timer *et;
   Edje *ed;
   Embryo_Function fn;
   
   et = data;
   if (!et) return 0;
   ed = et->edje;
   _edje_embryo_script_reset(ed);
   embryo_parameter_cell_push(ed->collection->script, (Embryo_Cell)et->val);
   ed->var_pool->timers = evas_list_remove(ed->var_pool->timers, et);
   fn = et->func;
   free(et);
     {
	void *pdata;
	
	pdata = embryo_program_data_get(ed->collection->script);
	embryo_program_data_set(ed->collection->script, ed);	
	embryo_program_run(ed->collection->script, fn);
	embryo_program_data_set(ed->collection->script, pdata);
	_edje_recalc(ed);
     }
   return 0;
}

static int
_edje_var_anim_cb(void *data)
{
   Evas_List *l, *tl = NULL;
   double t;
   
   t = ecore_time_get();
   for (l = _edje_anim_list; l; l = l->next)
     tl = evas_list_append(tl, l->data);
   while (tl)
     {
	Edje *ed;
	Evas_List *tl2;
	
	ed = tl->data;
	_edje_ref(ed);
	_edje_block(ed);
	_edje_freeze(ed);
	tl = evas_list_remove(tl, ed);
	if (!ed->var_pool) continue;
	tl2 = NULL;
	for (l = ed->var_pool->animators; l; l = l->next)
	  tl2 = evas_list_append(tl2, l->data);
	ed->var_pool->walking_list++;
	while (tl2)
	  {
	     Edje_Var_Animator *ea;
	     
	     ea = tl2->data;
	     if ((ed->var_pool) && (!ea->delete_me))
	       {
		  if ((!ed->paused) && (!ed->delete_me))
		    {
		       Embryo_Function fn;
		       float v;
		       
		       v = (t - ea->start)  / ea->len;
		       if (v > 1.0) v= 1.0;
		       _edje_embryo_script_reset(ed);
		       embryo_parameter_cell_push(ed->collection->script, (Embryo_Cell)ea->val);
		       embryo_parameter_cell_push(ed->collection->script, EMBRYO_FLOAT_TO_CELL(v));
		       fn = ea->func;
			 {
			    void *pdata;
			    
			    pdata = embryo_program_data_get(ed->collection->script);
			    embryo_program_data_set(ed->collection->script, ed);	
			    embryo_program_run(ed->collection->script, fn);
			    embryo_program_data_set(ed->collection->script, pdata);
			    _edje_recalc(ed);
			 }
		       if (v == 1.0) ea->delete_me = 1;
		    }
	       }
	     tl2 = evas_list_remove(tl2, ea);
	     if (ed->block_break)
	       {
		  evas_list_free(tl2);
		  break;
	       }
	  }
	ed->var_pool->walking_list--;
	for (l = ed->var_pool->animators; l;)
	  {
	     Edje_Var_Animator *ea;
	     
	     ea = l->data;
	     if (ea->delete_me)
	       {
		  l = l->next;
		  ed->var_pool->animators = evas_list_remove(ed->var_pool->animators, ea);
		  free(ea);
	       }
	     else
	       l = l->next;
	  }
	if (!ed->var_pool->animators)
	  _edje_anim_list = evas_list_remove(_edje_anim_list, ed);
	_edje_unblock(ed);
	_edje_thaw(ed);
	_edje_unref(ed);
     }
   if (!_edje_anim_list)
     {
	if (_edje_animator)
	  {
	     ecore_animator_del(_edje_animator);
	     _edje_animator = NULL;
	  }
     }
   return !!_edje_animator;
}

Edje_Var *
_edje_var_new(void)
{
   return calloc(1, sizeof(Edje_Var));
}

void
_edje_var_free(Edje_Var *var)
{
   if (var->type == EDJE_VAR_STRING)
     {
	if (var->data.s.v)
	  {
	     free(var->data.s.v);
	  }
     }
   free(var);
}

void
_edje_var_init(Edje *ed)
{
   if (!ed) return;
   if (!ed->collection) return;
   if (!ed->collection->script) return;
   ed->var_pool = calloc(1, sizeof(Edje_Var_Pool));
   if (!ed->var_pool) return;
   ed->var_pool->size = embryo_program_variable_count_get(ed->collection->script);
   ed->var_pool->vars = calloc(1, sizeof(Edje_Var) * ed->var_pool->size);
}

void
_edje_var_shutdown(Edje *ed)
{
   if (!ed->var_pool) return;
   if (ed->var_pool->vars)
     {
	int i;
	
	for (i = 0; i < ed->var_pool->size; i++)
	  {
	     if (ed->var_pool->vars[i].type == EDJE_VAR_STRING)
	       {
		  if (ed->var_pool->vars[i].data.s.v)
		    {
		       free(ed->var_pool->vars[i].data.s.v);
		       ed->var_pool->vars[i].data.s.v = NULL;
		    }
	       }
	     else if (ed->var_pool->vars[i].type == EDJE_VAR_LIST)
	       {
		  while (ed->var_pool->vars[i].data.l.v)
		    {
		       _edje_var_free(ed->var_pool->vars[i].data.l.v->data);
		       ed->var_pool->vars[i].data.l.v = evas_list_remove_list(ed->var_pool->vars[i].data.l.v, ed->var_pool->vars[i].data.l.v);
		    }
	       }
	  }
	free(ed->var_pool->vars);
     }
   while (ed->var_pool->timers)
     {
	Edje_Var_Timer *et;
	
	et = ed->var_pool->timers->data;
	ecore_timer_del(et->timer);
	free(et);
	ed->var_pool->timers = evas_list_remove(ed->var_pool->timers, et);
     }
   if (ed->var_pool->animators)
     {
	_edje_anim_list = evas_list_remove(_edje_anim_list, ed);
	if (!_edje_anim_list)
	  {
	     if (_edje_animator)
	       {
		  ecore_animator_del(_edje_animator);
		  _edje_animator = NULL;
	       }
	  }
     }
   while (ed->var_pool->animators)
     {
	Edje_Var_Animator *ea;
	
	ea = ed->var_pool->animators->data;
	free(ea);
	ed->var_pool->animators = evas_list_remove(ed->var_pool->animators, ea);
     }
   free(ed->var_pool);
   ed->var_pool = NULL;
}

int
_edje_var_string_id_get(Edje *ed, char *string)
{
   Embryo_Cell cell, *cptr;
   
   if (!ed) return 0;
   if (!ed->collection) return 0;
   if (!ed->collection->script) return 0;
   if (!string) return 0;
   cell = embryo_program_variable_find(ed->collection->script, string);
   if (cell == EMBRYO_CELL_NONE) return  0;
   cptr = embryo_data_address_get(ed->collection->script, cell);
   if (!cptr) return 0;
   return (int)(*cptr);
}

int
_edje_var_var_int_get(Edje *ed, Edje_Var *var)
{
   /* auto-cast */
   if (var->type == EDJE_VAR_STRING)
     {
	if (var->data.s.v)
	  {
	     double f;
	
	     f = atof(var->data.s.v);
	     free(var->data.s.v);
	     var->data.s.v = NULL;
	     var->data.i.v = (int)f;
	  }
	var->type = EDJE_VAR_INT;
     }
   else if (var->type == EDJE_VAR_FLOAT)
     {
	var->data.i.v = (int)(var->data.f.v);
	var->type = EDJE_VAR_INT;
     }
   else if (var->type == EDJE_VAR_NONE)
     {
	var->type = EDJE_VAR_INT;
     }
   else if (var->type == EDJE_VAR_LIST)
     {
	return 0;
     }
   else if (var->type == EDJE_VAR_HASH)
     {
	return 0;
     }
   return var->data.i.v;
}

void
_edje_var_var_int_set(Edje *ed, Edje_Var *var, int v)
{
   /* auto-cast */
   if (var->type == EDJE_VAR_STRING)
     {
	if (var->data.s.v)
	  {
	     free(var->data.s.v);
	     var->data.s.v = NULL;
	  }
	var->type = EDJE_VAR_INT;
     }
   else if (var->type == EDJE_VAR_FLOAT)
     {
	var->type = EDJE_VAR_INT;
     }
   else if (var->type == EDJE_VAR_NONE)
     {
	var->type = EDJE_VAR_INT;
     }
   else if (var->type == EDJE_VAR_LIST)
     {
	return;
     }
   else if (var->type == EDJE_VAR_HASH)
     {
	return;
     }
   var->data.i.v = v;
}

double
_edje_var_var_float_get(Edje *ed, Edje_Var *var)
{
   /* auto-cast */
   if (var->type == EDJE_VAR_STRING)
     {
	if (var->data.s.v)
	  {
	     double f;
	
	     f = atof(var->data.s.v);
	     free(var->data.s.v);
	     var->data.s.v = NULL;
	     var->data.f.v = f;
	  }
	var->type = EDJE_VAR_FLOAT;
     }
   else if (var->type == EDJE_VAR_INT)
     {
	var->data.f.v = (double)(var->data.i.v);
	var->type = EDJE_VAR_FLOAT;
     }
   else if (var->type == EDJE_VAR_NONE)
     {
	var->type = EDJE_VAR_FLOAT;
     }
   else if (var->type == EDJE_VAR_LIST)
     {
	return 0.0;
     }
   else if (var->type == EDJE_VAR_HASH)
     {
	return 0.0;
     }
   return var->data.f.v;
}

void
_edje_var_var_float_set(Edje *ed, Edje_Var *var, double v)
{
   /* auto-cast */
   if (var->type == EDJE_VAR_STRING)
     {
	if (var->data.s.v)
	  {
	     free(var->data.s.v);
	     var->data.s.v = NULL;
	  }
	var->type = EDJE_VAR_FLOAT;
     }
   else if (var->type == EDJE_VAR_INT)
     {
	var->data.f.v = 0;
	var->type = EDJE_VAR_FLOAT;
     }
   else if (var->type == EDJE_VAR_NONE)
     {
	var->type = EDJE_VAR_FLOAT;
     }
   else if (var->type == EDJE_VAR_LIST)
     {
	return;
     }
   else if (var->type == EDJE_VAR_HASH)
     {
	return;
     }
   var->data.f.v = v;
}

const char *
_edje_var_var_str_get(Edje *ed, Edje_Var *var)
{
   /* auto-cast */
   if (var->type == EDJE_VAR_INT)
     {
	char buf[64];
	
	snprintf(buf, sizeof(buf), "%i", var->data.i.v);
	var->data.s.v = strdup(buf);
	var->type = EDJE_VAR_STRING;
     }
   else if (var->type == EDJE_VAR_FLOAT)
     {
	char buf[64];
	
	snprintf(buf, sizeof(buf), "%f", var->data.f.v);
	var->data.s.v = strdup(buf);
	var->type = EDJE_VAR_STRING;
     }
   else if (var->type == EDJE_VAR_NONE)
     {
	var->data.s.v = strdup("");
	var->type = EDJE_VAR_STRING;
     }
   else if (var->type == EDJE_VAR_LIST)
     {
	return NULL;
     }
   else if (var->type == EDJE_VAR_HASH)
     {
	return NULL;
     }
   return var->data.s.v;
}

void
_edje_var_var_str_set(Edje *ed, Edje_Var *var, char *str)
{
   /* auto-cast */
   if (var->type == EDJE_VAR_STRING)
     {
	if (var->data.s.v)
	  {
	     free(var->data.s.v);
	     var->data.s.v = NULL;
	  }
     }
   else if (var->type == EDJE_VAR_INT)
     {
	var->type = EDJE_VAR_STRING;
     }
   else if (var->type == EDJE_VAR_FLOAT)
     {
	var->type = EDJE_VAR_STRING;
     }
   else if (var->type == EDJE_VAR_NONE)
     {
	var->type = EDJE_VAR_STRING;
     }
   else if (var->type == EDJE_VAR_LIST)
     {
	return;
     }
   else if (var->type == EDJE_VAR_HASH)
     {
	return;
     }
   var->data.s.v = strdup(str);
}

int
_edje_var_int_get(Edje *ed, int id)
{
   if (!ed) return 0;
   if (!ed->var_pool) return 0;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return 0;
   return _edje_var_var_int_get(ed, &(ed->var_pool->vars[id]));
}

void
_edje_var_int_set(Edje *ed, int id, int v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   _edje_var_var_int_set(ed, &(ed->var_pool->vars[id]), v);
}

double
_edje_var_float_get(Edje *ed, int id)
{
   if (!ed) return 0;
   if (!ed->var_pool) return 0;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return 0;
   return _edje_var_var_float_get(ed, &(ed->var_pool->vars[id]));
}

void
_edje_var_float_set(Edje *ed, int id, double v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   _edje_var_var_float_set(ed, &(ed->var_pool->vars[id]), v);
}

const char *
_edje_var_str_get(Edje *ed, int id)
{
   if (!ed) return NULL;
   if (!ed->var_pool) return NULL;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return NULL;
   return _edje_var_var_str_get(ed, &(ed->var_pool->vars[id]));
}

void
_edje_var_str_set(Edje *ed, int id, char *str)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   if (!str) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   _edje_var_var_str_set(ed, &(ed->var_pool->vars[id]), str);
}

/* list stuff */

void
_edje_var_list_var_append(Edje *ed, int id, Edje_Var *var)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
   ed->var_pool->vars[id].data.l.v = evas_list_append(ed->var_pool->vars[id].data.l.v, var);
}

void
_edje_var_list_var_prepend(Edje *ed, int id, Edje_Var *var)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
   ed->var_pool->vars[id].data.l.v = evas_list_prepend(ed->var_pool->vars[id].data.l.v, var);
}

void
_edje_var_list_var_append_relative(Edje *ed, int id, Edje_Var *var, Edje_Var *relative)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
   ed->var_pool->vars[id].data.l.v = evas_list_append_relative(ed->var_pool->vars[id].data.l.v, var, relative);
}

void
_edje_var_list_var_prepend_relative(Edje *ed, int id, Edje_Var *var, Edje_Var *relative)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
   ed->var_pool->vars[id].data.l.v = evas_list_prepend_relative(ed->var_pool->vars[id].data.l.v, var, relative);
}

Edje_Var *
_edje_var_list_nth(Edje *ed, int id, int n)
{
   if (!ed) return NULL;
   if (!ed->var_pool) return NULL;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return NULL;
   if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return NULL;
   return evas_list_nth(ed->var_pool->vars[id].data.l.v, n);
}

int
_edje_var_list_count_get(Edje *ed, int id)
{
   if (!ed) return 0;
   if (!ed->var_pool) return 0;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return 0;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return 0;
   return evas_list_count(ed->var_pool->vars[id].data.l.v);
}

void
_edje_var_list_remove_nth(Edje *ed, int id, int n)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Evas_List *nth;
	
	nth = evas_list_nth_list(ed->var_pool->vars[id].data.l.v, n);
	if (nth)
	  {
	     _edje_var_free(nth->data);
	     ed->var_pool->vars[id].data.l.v = evas_list_remove_list(ed->var_pool->vars[id].data.l.v, nth);
	  }
     }
}

int
_edje_var_list_nth_int_get(Edje *ed, int id, int n)
{
   if (!ed) return 0;
   if (!ed->var_pool) return 0;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return 0;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return 0;
     {
	Edje_Var *var;
	
	id += EDJE_VAR_MAGIC_BASE;
	var = _edje_var_list_nth(ed, id, n);
	if (!var) return 0;
	return _edje_var_var_int_get(ed, var);
     }
}

void
_edje_var_list_nth_int_set(Edje *ed, int id, int n, int v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	id += EDJE_VAR_MAGIC_BASE;
	var = _edje_var_list_nth(ed, id, n);
	if (!var) return;
	_edje_var_var_int_set(ed, var, v);
     }
}

void
_edje_var_list_int_append(Edje *ed, int id, int v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_int_set(ed, var, v);
	_edje_var_list_var_append(ed, id, var);
     }
}

void
_edje_var_list_int_prepend(Edje *ed, int id, int v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_int_set(ed, var, v);
	_edje_var_list_var_prepend(ed, id, var);
     }
}

void
_edje_var_list_int_insert(Edje *ed, int id, int n, int v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var, *var_rel;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_int_set(ed, var, v);
	var_rel = _edje_var_list_nth(ed, id, n);
	if (!var_rel)
	  _edje_var_list_var_append(ed, id, var);
	else
	  _edje_var_list_var_prepend_relative(ed, id, var, var_rel);
     }
}

double
_edje_var_list_nth_float_get(Edje *ed, int id, int n)
{
   if (!ed) return 0;
   if (!ed->var_pool) return 0;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return 0;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return 0;
     {
	Edje_Var *var;
	
	id += EDJE_VAR_MAGIC_BASE;
	var = _edje_var_list_nth(ed, id, n);
	if (!var) return 0;
	return _edje_var_var_float_get(ed, var);
     }
}

void
_edje_var_list_nth_float_set(Edje *ed, int id, int n, double v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	id += EDJE_VAR_MAGIC_BASE;
	var = _edje_var_list_nth(ed, id, n);
	if (!var) return;
	_edje_var_var_float_set(ed, var, v);
     }
}

void
_edje_var_list_float_append(Edje *ed, int id, double v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_float_set(ed, var, v);
	_edje_var_list_var_append(ed, id, var);
     }
}

void
_edje_var_list_float_prepend(Edje *ed, int id, double v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_float_set(ed, var, v);
	_edje_var_list_var_prepend(ed, id, var);
     }
}

void
_edje_var_list_float_insert(Edje *ed, int id, int n, double v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var, *var_rel;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_float_set(ed, var, v);
	var_rel = _edje_var_list_nth(ed, id, n);
	if (!var_rel)
	  _edje_var_list_var_append(ed, id, var);
	else
	  _edje_var_list_var_prepend_relative(ed, id, var, var_rel);
     }
}

const char *
_edje_var_list_nth_str_get(Edje *ed, int id, int n)
{
   if (!ed) return NULL;
   if (!ed->var_pool) return NULL;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return NULL;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return NULL;
     {
	Edje_Var *var;
	
	id += EDJE_VAR_MAGIC_BASE;
	var = _edje_var_list_nth(ed, id, n);
	if (!var) return NULL;
	return _edje_var_var_str_get(ed, var);
     }
}

void
_edje_var_list_nth_str_set(Edje *ed, int id, int n, char *v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	id += EDJE_VAR_MAGIC_BASE;
	var = _edje_var_list_nth(ed, id, n);
	if (!var) return;
	_edje_var_var_str_set(ed, var, v);
     }
}

void
_edje_var_list_str_append(Edje *ed, int id, char *v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_str_set(ed, var, v);
	_edje_var_list_var_append(ed, id, var);
     }
}

void
_edje_var_list_str_prepend(Edje *ed, int id, char *v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_str_set(ed, var, v);
	_edje_var_list_var_prepend(ed, id, var);
     }
}

void
_edje_var_list_str_insert(Edje *ed, int id, int n, char *v)
{
   if (!ed) return;
   if (!ed->var_pool) return;
   id -= EDJE_VAR_MAGIC_BASE;
   if ((id < 0) || (id >= ed->var_pool->size)) return;
   if (ed->var_pool->vars[id].type == EDJE_VAR_NONE)
     ed->var_pool->vars[id].type = EDJE_VAR_LIST;
   else if (ed->var_pool->vars[id].type != EDJE_VAR_LIST) return;
     {
	Edje_Var *var, *var_rel;
	
	var = _edje_var_new();
	if (!var) return;
	id += EDJE_VAR_MAGIC_BASE;
	_edje_var_var_str_set(ed, var, v);
	var_rel = _edje_var_list_nth(ed, id, n);
	if (!var_rel)
	  _edje_var_list_var_append(ed, id, var);
	else
	  _edje_var_list_var_prepend_relative(ed, id, var, var_rel);
     }
}

int
_edje_var_timer_add(Edje *ed, double in, char *fname, int val)
{
   Edje_Var_Timer *et;
   Embryo_Function fn;
   
   if (!ed->var_pool) return 0;
   fn = embryo_program_function_find(ed->collection->script, fname);
   if (fn == EMBRYO_FUNCTION_NONE) return 0;
   et = calloc(1, sizeof(Edje_Var_Timer));
   if (!et) return 0;
   et->id = ++ed->var_pool->id_count;
   et->edje = ed;
   et->func = fn;
   et->val = val;
   et->timer = ecore_timer_add(in, _edje_var_timer_cb, et);
   if (!et->timer)
     {
	free(et);
	return 0;
     }
   ed->var_pool->timers = evas_list_prepend(ed->var_pool->timers, et);
   return et->id;
}

void
_edje_var_timer_del(Edje *ed, int id)
{
   Evas_List *l;

   if (!ed->var_pool) return;
   for (l = ed->var_pool->timers; l; l = l->next)
     {
	Edje_Var_Timer *et;
	
	et = l->data;
	if (et->id == id)
	  {
	     ed->var_pool->timers = evas_list_remove(ed->var_pool->timers, et);
	     ecore_timer_del(et->timer);
	     free(et);
	     return;
	  }
     }
}

int
_edje_var_anim_add(Edje *ed, double len, char *fname, int val)
{
   Edje_Var_Animator *ea;
   Embryo_Function fn;
   
   if (!ed->var_pool) return 0;
   if (len <= 0.0) return 0;
   fn = embryo_program_function_find(ed->collection->script, fname);
   if (fn == EMBRYO_FUNCTION_NONE) return 0;
   ea = calloc(1, sizeof(Edje_Var_Animator));
   if (!ea) return 0;
   ea->start = ecore_time_get();
   ea->len = len;
   ea->id = ++ed->var_pool->id_count;
   ea->edje = ed;
   ea->func = fn;
   ea->val = val;
   if (!_edje_anim_list)
     _edje_anim_list = evas_list_append(_edje_anim_list, ed);
   ed->var_pool->animators = evas_list_prepend(ed->var_pool->animators, ea);
   if (!_edje_animator)
     _edje_animator = ecore_animator_add(_edje_var_anim_cb, NULL);
   return ea->id;
}

void
_edje_var_anim_del(Edje *ed, int id)
{
   Evas_List *l;

   if (!ed->var_pool) return;
   for (l = ed->var_pool->animators; l; l = l->next)
     {
	Edje_Var_Animator *ea;
	
	ea = l->data;
	if (ea->id == id)
	  {
	     if (!ed->var_pool->walking_list)
	       {
		  ed->var_pool->animators = evas_list_remove(ed->var_pool->animators, ea);
		  free(ea);
		  if (!ed->var_pool->animators)
		    {
		       _edje_anim_list = evas_list_remove(_edje_anim_list, ed);
		       if (!_edje_anim_list)
			 {
			    if (_edje_animator)
			      {
				 ecore_animator_del(_edje_animator);
				 _edje_animator = NULL;
			      }
			 }
		    }
	       }
	     else
	       ea->delete_me = 1;
	     return;
	  }
     }
}
