#include "Eobj.h"
#include "simple.h"

#include "inherit.h"
#include "inherit2.h"

#include "config.h"

#include "../eunit_tests.h"

EAPI Eobj_Op INHERIT2_BASE_ID = 0;

#define MY_CLASS INHERIT2_CLASS

static void
_a_set(Eobj *obj, void *class_data EINA_UNUSED, va_list *list)
{
   int a;
   a = va_arg(*list, int);
   printf("%s %d\n", eobj_class_name_get(MY_CLASS), a);
   eobj_do(obj, simple_a_print());
   eobj_do_super(obj, simple_a_set(a + 1));

   fail_if(eobj_do_super(obj, simple_a_print()));
}

static void
_print(Eobj *obj, void *class_data EINA_UNUSED, va_list *list EINA_UNUSED)
{
   printf("Hey\n");
   fail_if(eobj_do_super(obj, inherit2_print()));
}

static void
_print2(Eobj *obj EINA_UNUSED, void *class_data EINA_UNUSED, va_list *list EINA_UNUSED)
{
   printf("Hey2\n");
}

static void
_class_constructor(Eobj_Class *klass)
{
   const Eobj_Op_Func_Description func_desc[] = {
        EOBJ_OP_FUNC(SIMPLE_ID(SIMPLE_SUB_ID_A_SET), _a_set),
        EOBJ_OP_FUNC(INHERIT2_ID(INHERIT2_SUB_ID_PRINT), _print),
        EOBJ_OP_FUNC(INHERIT2_ID(INHERIT2_SUB_ID_PRINT2), _print2),
        EOBJ_OP_FUNC_SENTINEL
   };

   eobj_class_funcs_set(klass, func_desc);
}

static const Eobj_Op_Description op_desc[] = {
     EOBJ_OP_DESCRIPTION(INHERIT2_SUB_ID_PRINT, "", "Print hey"),
     EOBJ_OP_DESCRIPTION(INHERIT2_SUB_ID_PRINT2, "", "Print hey2"),
     EOBJ_OP_DESCRIPTION_SENTINEL
};

static const Eobj_Class_Description class_desc = {
     "Inherit2",
     EOBJ_CLASS_TYPE_REGULAR,
     EOBJ_CLASS_DESCRIPTION_OPS(&INHERIT2_BASE_ID, op_desc, INHERIT2_SUB_ID_LAST),
     NULL,
     0,
     NULL,
     NULL,
     _class_constructor,
     NULL
};

EOBJ_DEFINE_CLASS(inherit2_class_get, &class_desc, INHERIT_CLASS, NULL);

