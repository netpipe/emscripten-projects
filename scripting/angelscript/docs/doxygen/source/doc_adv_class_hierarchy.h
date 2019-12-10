/**

\page doc_adv_class_hierarchy Registering class hierarchies

AngelScript cannot automatically determine relationships between registered classes, so in 
order to establish the hierarchies for use within the script language it is necessary
to do a bit more registration beyond the normal \ref doc_register_type "object registration".

Hierarchies can currently only be registered for \ref doc_register_ref_type "reference types", 
not for \ref doc_register_val_type "value types".

\section doc_adv_class_hierarchy_1 Establishing the relationship

In order to let AngelScript know that two types are related you need to register the 
reference cast behaviours \ref asBEHAVE_REF_CAST and \ref asBEHAVE_IMPLICIT_REF_CAST. The 
asBEHAVE_REF_CAST should be used if you only want to allow the cast through an explicit
call with the <tt>\ref conversion "cast&lt;class&gt;"</tt> operator. asBEHAVE_IMPLICIT_REF_CAST
should be used when you want to allow the compiler to implicitly perform the cast as necessary.

Usually you'll want to use asBEHAVE_IMPLICIT_REF_CAST for casts from a derived type to the base type, 
and asBEHAVE_REF_CAST for casts from a base type to a derived type.

\code
// Example REF_CAST behaviour
template<class A, B>
B* refCast(A* a)
{
    // If the handle already is a null handle, then just return the null handle
    if( !a ) return 0;

    // Now try to dynamically cast the pointer to the wanted type
    B* b = dynamic_cast<B*>(a);
    if( b == 0 )
    {
        // Since the cast couldn't be made, we need to release the handle we received
        a->release();
    }
    return b;
}

// Example registration of the behaviour
r = engine->RegisterGlobalBehaviour(asBEHAVE_REF_CAST, "derived@ f(base@)", asFUNCTION(refCast<base,derived>), asCALL_CDECL); assert( r >= 0 );
r = engine->RegisterGlobalBehaviour(asBEHAVE_IMPLICIT_REF_CAST, "base@ f(derived@)", asFUNCTION(refCast<derived,base>), asCALL_CDECL); assert( r >= 0 );
\endcode

\section doc_adv_class_hierarchy_2 Inherited methods and properties

Just as relationships cannot be determined, there is also no way to automatically let AngelScript
add inherited methods and properties to derived types. The reason for this is that method pointers
and property offsets may differ between the base class and derived class, especially when multiple
inheritance is used, and there is no way to automatically determine exactly what the difference is.

For this reason the application needs to register all the inherited methods and properties for 
the derived classes, which may lead to a bit of duplicate code. However, you may be able to avoid
the duplication through a bit of clever thinking. Here is an example of registering the methods and 
properties for a base class and the derived class (registration of behaviours has been omitted for briefness):

\code
// The base class
class base
{
public:
  virtual void aMethod();
  
  int aProperty;
};

// The derived class
class derived : public base
{
public:
  virtual void aNewMethod();
  
  int aNewProperty;
};

// The code to register the classes
// This is implemented as a template function, to support multiple inheritance
template <class T>
void RegisterBaseMembers(asIScriptEngine *engine, const char *type)
{
  int r;

  r = engine->RegisterObjectMethod(type, "void aMethod()", asMETHOD(T, aMethod), asCALL_THISCALL); assert( r >= 0 );
  
  r = engine->RegisterObjectProperty(type, "int aProperty", offsetof(T, aProperty)); assert( r >= 0 );
}

template <class T>
void RegisterDerivedMembers(asIScriptEngine *engine, const char *type)
{
  int r;

  // Register the inherited members by calling 
  // the registration of the base members
  RegisterBaseMembers<T>(engine, type);

  // Now register the new members
  r = engine->RegisterObjectMethod(typem "void aNewMethod()", asMETHOD(T, aNewMethod), asCALL_THISCALL); assert( r >= 0 );

  r = engine->RegisterObjectProperty(type, "int aProperty", offsetof(T, aProperty)); assert( r >= 0 );
}

void RegisterTypes(asIScriptEngine *engine)
{
  int r;

  // Register the base type
  r = engine->RegisterObjectType("base", 0, asOBJ_REF); assert( r >= 0 );
  RegisterFathersMethods<base>(engine, "base");

  // Register the derived type
  r = engine->RegisterObjectType("derived", 0, asOBJ_REF); assert( r >= 0 );
  RegisterSonsMethods<derived>(engine, "derived");
}
\endcode
   
*/
