#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
//
// Examples showing the relationships between:
//  MicroPython Function and Method calls
//            and
//  MicroPython C code
//
// Each examples provides:
// - MicroPython usage
// - C code to implement the MicroPython usage
//
// This code can be compiled and linked into a MicroPython build
// - see related commit in Github for the other files that need modifications
//
// ----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------

// Function Example 1:
//      0 arguments
//
// MicroPython Usage:
//
//      def f0():
//         ...
//
//      import argex
//      argex.f0()

STATIC mp_obj_t function_0_args() {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(function_0_args_obj, function_0_args);


// Function Example 2:
//      1 positional argument
//
// MicroPython Usage:
//
//      def f1(arg1):
//         ...
//
//      import argex
//      argex.f1(44)

STATIC mp_obj_t function_1_args(mp_obj_t arg1) {
    __attribute__((unused)) mp_int_t n = mp_obj_get_int(arg1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(function_1_args_obj, function_1_args);


// Function Example 3:
//      2 positional arguments, returns boolean
//
// MicroPython Usage:
//
//      def f2(pos1, pos2):
//         ...
//
//      import argex
//      argex.f2(4.5, 99.0)

STATIC mp_obj_t function_2_args(mp_obj_t arg1, mp_obj_t arg2) {
    mp_float_t float1 = mp_obj_get_float(arg1);
    mp_float_t float2 = mp_obj_get_float(arg2);

    if (float1 > float2) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(function_2_args_obj, function_2_args);


// Function Example 4:
//      3 positional arguments, returns int
//
// MicroPython Usage:
//
//      def f3(pos1, pos2, pos3):
//         ...
//
//      import argex
//      argex.f3(4, 99, 111)

STATIC mp_obj_t function_3_args(mp_obj_t arg1, mp_obj_t arg2, mp_obj_t arg3) {
    mp_int_t n1 = mp_obj_get_int(arg1);
    mp_int_t n2 = mp_obj_get_int(arg2);
    mp_int_t n3 = mp_obj_get_int(arg3);

    mp_int_t sum = n1 + n2 + n3;

    return mp_obj_new_int(sum);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(function_3_args_obj, function_3_args);


// Function Example 5:
//      variable number of positional arguments, specified by min and max, returns float
//
// MicroPython Usage:
//
//      def f_min_max(pos1, [pos2]):
//         ...
//
//      import argex
//      argex.f_min_max(34.74)
//      argex.f_min_max(34.74, 5.0)

#define MIN_NUM_ARGS (1)
#define MAX_NUM_ARGS (2)
STATIC mp_obj_t function_min_max_args(size_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        mp_float_t float1 = mp_obj_get_float(args[0]);
        return mp_obj_new_float(float1);
    } else {
        mp_float_t float1 = mp_obj_get_float(args[0]);
        mp_float_t float2 = mp_obj_get_float(args[1]);
        return mp_obj_new_float(float1 + float2);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(function_min_max_args_obj, MIN_NUM_ARGS, MAX_NUM_ARGS, function_min_max_args);


// Function Example 6:
//      mix of positional and keyword args, minimum number of positional args, returns none
//          - 2 positional args
//          - 1 keyword arg
//          - first positional arg is mandatory (e.g.  MIN_NUM_POSITIONAL_ARGS_VARIANT1 = 1)
//          - keyword arg is optional
//
// MicroPython Usage:
//
//      def f_pos_and_kw_v1(pos1, [pos2], [kw1=None]):
//         ...
//
//      import argex
//      argex.f_pos_and_kw_v1(6)
//      argex.f_pos_and_kw_v1(6, 7)          # two positional args
//      argex.f_pos_and_kw_v1(6, kw1=99)     # one positional arg, one kw arg
//      argex.f_pos_and_kw_v1(6, 7, kw1=99)  # two positional args, one kw arg

#define MIN_NUM_POSITIONAL_ARGS_VARIANT1 (1)
STATIC mp_obj_t function_pos_and_kw_args_variant_1(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_pos1, ARG_pos2, ARG_kw1 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pos1,                 MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_pos2,                 MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_kw1, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // important notes:
    //      'n_args' refers ONLY to the count of positional args
    //      does NOT include count of keyword args
    printf("n_args = %u\n", n_args);

    if (args[ARG_pos1].u_obj != mp_const_none) {
        printf("arg[ARG_pos1] = %u\n", mp_obj_get_int(args[ARG_pos1].u_obj));
    }

    if (args[ARG_pos2].u_obj != mp_const_none) {
        printf("arg[ARG_pos2] = %u\n", mp_obj_get_int(args[ARG_pos2].u_obj));
    }

    if (args[ARG_kw1].u_obj != mp_const_none) {
        printf("arg[ARG_kw1] = %u\n", mp_obj_get_int(args[ARG_kw1].u_obj));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(function_pos_and_kw_args_variant_1_obj, MIN_NUM_POSITIONAL_ARGS_VARIANT1, function_pos_and_kw_args_variant_1);


// Function Example 7:
//      keyword args, returns none
//          - MIN_NUM_POSITIONAL_ARGS_VARIANT2 = 0
//          - 2 keyword args
//          - both keyword args are optional
//
// MicroPython Usage:
//
//      def f_pos_and_kw_v2([kw1=None], [kw2=None]):
//         ...
//
//      import argex
//      argex.f_pos_and_kw_v2()
//      argex.f_pos_and_kw_v2(33)
//      argex.f_pos_and_kw_v2(kw1=33)
//      argex.f_pos_and_kw_v2(33, 55)
//      argex.f_pos_and_kw_v2(33, kw2=55)
//      argex.f_pos_and_kw_v2(kw1=33, kw2=55)
//      argex.f_pos_and_kw_v2(kw2=55)

#define MIN_NUM_POSITIONAL_ARGS_VARIANT2 (0)
STATIC mp_obj_t function_pos_and_kw_args_variant_2(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_kw1, ARG_kw2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_kw1, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_kw2, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // important notes:
    //      'n_args' refers ONLY to the count of positional args
    //      does NOT include count of keyword args
    printf("n_args = %u\n", n_args);

    if (args[ARG_kw1].u_obj != mp_const_none) {
        printf("arg[ARG_kw1] = %u\n", mp_obj_get_int(args[ARG_kw1].u_obj));
    }

    if (args[ARG_kw2].u_obj != mp_const_none) {
        printf("arg[ARG_kw2] = %u\n", mp_obj_get_int(args[ARG_kw2].u_obj));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(function_pos_and_kw_args_variant_2_obj, MIN_NUM_POSITIONAL_ARGS_VARIANT2, function_pos_and_kw_args_variant_2);


// Function Example 8:
//      mix of positional and keyword args, minimum number of positional args, returns none
//      - one positional arg.  Mandatory. MIN_NUM_POSITIONAL_ARGS_VARIANT3 = 1
//      - 2 keyword args, one keyword arg is Mandatory, the other is Optional
//
// MicroPython Usage:
//
//      def f_pos_and_kw_v3(pos1, [kw1=-1], kw2=None):
//         ...
//
//      import argex
//      argex.f_pos_and_kw_v3(True, kw2='le chien')
//      argex.f_pos_and_kw_v3(False, kw2="j'aime bien le MicroPython")
//      argex.f_pos_and_kw_v3(False, kw1=1000, kw2="ok")

#define MIN_NUM_POSITIONAL_ARGS_VARIANT3 (1)
STATIC mp_obj_t function_pos_and_kw_args_variant_3(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_pos1, ARG_kw1, ARG_kw2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pos1, MP_ARG_REQUIRED |                  MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_kw1,                    MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = -1} },
        { MP_QSTR_kw2,  MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // important notes:
    //      'n_args' refers ONLY to the count of positional args
    //      does NOT include count of keyword args
    printf("n_args = %u\n", n_args);

    printf("arg[ARG_pos1] = %u\n", args[ARG_pos1].u_bool);
    printf("arg[ARG_kw1] = %d\n", args[ARG_kw1].u_int);

    mp_uint_t len;
    const char *p;
    if (args[ARG_kw2].u_obj != mp_const_none) {
        p = mp_obj_str_get_data(args[ARG_kw2].u_obj, &len);
        printf("arg[ARG_kw2] = %s\n", p);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(function_pos_and_kw_args_variant_3_obj, MIN_NUM_POSITIONAL_ARGS_VARIANT3, function_pos_and_kw_args_variant_3);


//----------------------------------------------------------------------------
// METHOD EXAMPLES
//----------------------------------------------------------------------------

typedef struct _argex_obj_t {
    mp_obj_base_t   base;
    bool            var1;
    int32_t         var2;
} argex_obj_t;

// Instantiation
//      2 positional arguments
//
//      class Argex:
//          __init__(self, pos1, pos2):
//
//      from argex import Argex
//      a = Argex(True, 4)
//      a = Argex(pos1=True, pos2=4)

mp_obj_t argex_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_pos1, ARG_pos2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pos1,     MP_ARG_REQUIRED | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_pos2,     MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    argex_obj_t *self = m_new_obj(argex_obj_t);
    self->base.type = type;
    self->var1 = args[ARG_pos1].u_bool;
    self->var2 = args[ARG_pos2].u_int;
    return MP_OBJ_FROM_PTR(self);
}

// Method Example 1:
//      0 positional arguments   e.g.  argex.m0()
//      NOTE: for Methods, self is the first argument in the call
//          Notice use of MP_DEFINE_CONST_FUN_OBJ_1 below rather than MP_DEFINE_CONST_FUN_OBJ_0
//
//      class Argex:
//          m0(self):
//
//      from argex import Argex
//      a = Argex(True, 7)
//      a.m0()

STATIC mp_obj_t method_0_args(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(method_0_args_obj, method_0_args);


// Method Example 2:
//      2 positional arguments
//      NOTE: for Methods, "self" is the first argument in the call
//          Notice use of MP_DEFINE_CONST_FUN_OBJ_3 below rather than MP_DEFINE_CONST_FUN_OBJ_2
//
//      class Argex:
//          m2(self, pos1, pos2):
//
//      from argex import Argex
//      a = Argex(True, 7)
//      a.m2(False, 99)

STATIC mp_obj_t method_2_args(mp_obj_t self_in, mp_obj_t arg1, mp_obj_t arg2) {
    argex_obj_t *self = self_in;

    self->var1 = mp_obj_get_int(arg1);
    self->var2 = mp_obj_get_int(arg2);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(method_2_args_obj, method_2_args);


// Method Example 3:
//      variable number of positional arguments, specified by min and max, returns float
//      NOTE: for a method call, MIN_NUM_METHOD_ARGS must never be zero
//
//      class Argex:
//          m_min_max(self, [pos1]):
//
//      from argex import Argex
//      a = Argex(True, 7)
//      a.m_min_max()
//      a.m_min_max(False)

#define MIN_NUM_METHOD_ARGS (1)  // self
#define MAX_NUM_METHOD_ARGS (2)  // self + one Optional positional argument
STATIC mp_obj_t method_min_max_args(size_t n_args, const mp_obj_t *args) {
    argex_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    // important notes:
    //      'n_args' refers to the count of positional args, and +1 for the class instance self
    printf("n_args = %u\n", n_args);

    if (n_args == 1) {
        // when n_args equals 1 it means that ONLY argument passed is the class instance self
        return mp_obj_new_int(self->var1);
    } else {
        self->var1 = mp_obj_get_int(args[1]);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(method_min_max_args_obj, MIN_NUM_METHOD_ARGS, MAX_NUM_METHOD_ARGS, method_min_max_args);


// Method Example 4:
//      mix of positional and keyword args
//          - one Mandatory positional arg
//          - 2 keyword args, one keyword arg is Mandatory, the other is Optional
//
//      class Argex:
//          m_pos_and_kw(self, pos1, [kw1=-1], kw2=None):
//
//      from argex import Argex
//      a = Argex(True, 7))
//      a.m_pos_and_kw(True, kw2="a string")
//      a.m_pos_and_kw(pos1=True, kw2="a string")
//      a.m_pos_and_kw(True, kw1=-999, kw2="a string")
//
//      Some experimentation and reverse-engineering shows these patterns:
//          allowed_args[]                                                          MIN_NUM_POSITIONAL_ARGS   allowed arg formats for pos1, kw2
//          ==================================================================      =======================   ============================
//          { MP_QSTR_pos1, MP_ARG_REQUIRED | MP_ARG_BOOL, {.u_bool = false} }      1                         (False, kw2='k'), (pos1=False, kw2='k')
//          { MP_QSTR_pos1, MP_ARG_REQUIRED | MP_ARG_BOOL, {.u_bool = false} }      2                         (False, kw2='k')
//          { MP_QSTR_pos1,                   MP_ARG_BOOL, {.u_bool = false} }      1                         (kw2='k'), (False, kw2='k'), (pos1=False, kw2='k')
//          { MP_QSTR_pos1,                   MP_ARG_BOOL, {.u_bool = false} }      2                         (False, kw2='k')
//
#define MIN_NUM_POSITIONAL_ARGS (2)  // self + one Mandatory positional argument
STATIC mp_obj_t method_pos_and_kw_args(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_pos1, ARG_kw1, ARG_kw2 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pos1, MP_ARG_REQUIRED |                  MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_kw1,                    MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = -1} },
        { MP_QSTR_kw2,  MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    };

    argex_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    //  note the subtle difference in how the argument parse routine is called for a Method
    //      - for a Function the call is (n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args)
    //      - for a Method, the call changes as shown below:
    //          '*pos_args + 1' skips over self, and points to the first positional arg
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // important notes:
    //      'n_args' refers to the count of positional args, and +1 for the class instance 'self'
    //      does NOT include count of keyword args
    printf("n_args = %u\n", n_args);

    printf("arg[ARG_pos1] = %u\n", args[ARG_pos1].u_bool);
    printf("arg[ARG_kw1] = %d\n", args[ARG_kw1].u_int);

    self->var1 = args[ARG_pos1].u_bool;

    mp_uint_t len;
    const char *p;
    if (args[ARG_kw2].u_obj != mp_const_none) {
        p = mp_obj_str_get_data(args[ARG_kw2].u_obj, &len);
        printf("arg[ARG_kw2] = %s\n", p);
    } else {
        printf("arg[ARG_kw2] = None\n");
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(method_pos_and_kw_args_obj, MIN_NUM_POSITIONAL_ARGS, method_pos_and_kw_args);


// Methods for Class "Argex"
STATIC const mp_rom_map_elem_t argex_locals_dict_table[] = {
    // METHOD EXAMPLES
    { MP_ROM_QSTR(MP_QSTR_m0),              MP_ROM_PTR(&method_0_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_m2),              MP_ROM_PTR(&method_2_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_m_min_max),       MP_ROM_PTR(&method_min_max_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_m_pos_and_kw),    MP_ROM_PTR(&method_pos_and_kw_args_obj) },
};

STATIC MP_DEFINE_CONST_DICT(argex_locals_dict, argex_locals_dict_table);

const mp_obj_type_t argex_type = {
    { &mp_type_type },
    .name = MP_QSTR_Argex,
    //.print = argex_print,
    .make_new = argex_make_new,
    .locals_dict = (mp_obj_dict_t*)&argex_locals_dict,
};


// Functions for the Module "argex" and the Class "Argex"
STATIC const mp_rom_map_elem_t argex_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_argex) },

    // FUNCTION EXAMPLES
    { MP_ROM_QSTR(MP_QSTR_f0),                  MP_ROM_PTR(&function_0_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_f1),                  MP_ROM_PTR(&function_1_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_f2),                  MP_ROM_PTR(&function_2_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_f3),                  MP_ROM_PTR(&function_3_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_f_min_max),           MP_ROM_PTR(&function_min_max_args_obj) },
    { MP_ROM_QSTR(MP_QSTR_f_pos_and_kw_v1),     MP_ROM_PTR(&function_pos_and_kw_args_variant_1_obj) },
    { MP_ROM_QSTR(MP_QSTR_f_pos_and_kw_v2),     MP_ROM_PTR(&function_pos_and_kw_args_variant_2_obj) },
    { MP_ROM_QSTR(MP_QSTR_f_pos_and_kw_v3),     MP_ROM_PTR(&function_pos_and_kw_args_variant_3_obj) },
    { MP_ROM_QSTR(MP_QSTR_Argex),               MP_ROM_PTR(&argex_type) },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_argex_globals,
    argex_globals_table
);

const mp_obj_module_t mp_module_argex = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_argex_globals,
};
