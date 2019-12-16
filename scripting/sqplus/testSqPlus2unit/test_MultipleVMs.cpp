#include "testEnv.hpp"
#include <iostream>

SQPLUS_TEST(Test_MultipleVMs)
{
    SQPLUS_TEST_TRACE();

    using namespace SqPlus;

    struct SquirrelVMSys vms1;
    SquirrelVM::GetVMSys(vms1); // Get Original VMsys

    SquirrelVM::Init();
    struct SquirrelVMSys vms2;
    SquirrelVM::GetVMSys(vms2); // Get new VMsys

    
    // Run script in original VMsys
    SquirrelVM::SetVMSys(vms1);
    RUN_SCRIPT(_SC("\
x <- 1.0; \n\
"));

    // Run script in new VMsys
    SquirrelVM::SetVMSys(vms2);
    RUN_SCRIPT(_SC("\
assert(!(\"x\" in getroottable())); \n\
x <- 2.0; \n\
"));
    
    // Run script in original VMsys
    SquirrelVM::SetVMSys(vms1);
    RUN_SCRIPT(_SC("\
assert(x == 1.0);\n\
"));

    // Current shared ownership of SQVM is not completely safe 
    // (due to mixed ownership modes in sq_open(), sq_newthread() 
    // and sq_close()). 
    // The safest is to reset SquirrelVMSys manually and shutdown
    // the VM we created above manually.
    
    // Close/destroy VM2
    SquirrelVM::SetVMSys(vms2);
    vms2.Reset();
    SquirrelVM::Shutdown();
    
    // Just clear the reference for VM1, the test framework will destroy
    // the VM finally.
	SquirrelVM::SetVMSys(vms1);
    vms1.Reset();
}

