#include "testEnv.hpp"

SQPLUS_TEST(Test_Weakref)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;
    SquirrelObject root = SquirrelVM::GetRootTable();
    
    SquirrelObject a = SquirrelVM::CreateArray(3);
    a.SetValue(0, _SC("first"));
    a.SetValue(1, _SC("second"));
    a.SetValue(2, _SC("third"));

    SquirrelObject w;
    w.Weakref(a);
    CHECK_EQUAL(w.GetType(), OT_WEAKREF);

    // check types of pointed objects
    CHECK_EQUAL(SquirrelObject(w.GetWeakrefVal()).GetType(), OT_ARRAY);
    CHECK_EQUAL(SquirrelObject(w.GetWeakrefVal()).GetValue(0).GetType(), OT_STRING);

    // check value
    CHECK_EQUAL(SquirrelObject(w.GetWeakrefVal()).GetString(0), _SC("first"));


    // destory the array by assigning a integer
    a.SetByValue<int>(123);

    // the weakref now returns NULL
    CHECK_EQUAL(w.GetType(), OT_WEAKREF);
    CHECK(SquirrelObject(w.GetWeakrefVal()).GetType() != OT_ARRAY);
    CHECK(SquirrelObject(w.GetWeakrefVal()).GetType() == OT_NULL);
}
