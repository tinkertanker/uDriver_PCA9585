#ifndef UTEST_H
#define UTEST_H
/**
 * @file src/utest.h
 * Declares the functionality provided by the μTest framework 
 * 
 * @warning For microbit support, only include this file after you have included
 * pxt.h or microbit.h
*/

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

/** @defgroup debug Debugging Utilities
 *  An assortment of debugging goodies such as more infomative prints, memory
 *  dumps.
 *
 *  Debugging macros for development use when **DEBUG** is defined,
 *  compiled out when **DEBUG** is undefined. 
 *  
 *  For example:
 *  @code{c}
 *  #define DEBUG
 *  #include "utest.h"
 *
 *  int main()
 *  {
 *      DPRINT("test");
 *  }
 *  @endcode
 *
 *  Would print 'test'
 *  While this program:
 *  @code{.c}
 *  #include "utest.h"
 *
 *  int main()
 *  {
 *      DPRINT("test");
 *  }
 *  @endcode
 *  Would no print anything.
  */
//@{
#ifdef DEBUG

void _debug_mem_dump(void *mptr, size_t len, size_t blk, const char *fname, int line);
#define DMEM_DUMP(ptr, size, blklen)\
    _debug_mem_dump(ptr, size, blklen, __func__, __LINE__)

#ifdef MICROBIT_H
/* pxt.h defines uBit as a global variable, it seems that Microbit will become 
 * unresponsive if more than one instance of MicroBit is defined. This forces the
 * user to declare a global 'uBit' if not already present */
#undef printf
#define DPRINT(msg) uBit.serial.printf("DEBUG: %s: %d: %s\r\n",__func__,__LINE__,msg)
#define DPRINTF(...) uBit.serial.printf("DEBUG: " __VA_ARGS__)

#else
#define DPRINT(msg) printf("DEBUG: %s: %d: %s\n",__func__,__LINE__, msg)
#define DPRINTF(...) printf("DEBUG: " __VA_ARGS__)
#endif /* ifdef MICROBIT_H */

#else
/** @def DPRINT
 *  Debugging print - prints debugging message, with function name and line 
 *  number, appending a newline at the end. Debug print is only defined when
 *  DEBUG is defined. This allows the removal of debug prints by removing the 
 *  definition of DEBUG.
 *  If compiled for the MircoBit, would print to serial instead of standard 
 *  output, given that a MicroBit instance is defined as 'uBit' 
 *  (ie. Microbit uBit; is in the scope of the macro.)
 *  
 *  @param msg The debugging message to print
*/
#define DPRINT(msg)

/** @def DPRINTF
 *  Debugging print format - print printf format string with arguments, with 
 *  function name and line number.Debug print is only defined when
 *  DEBUG is defined. This allows the removal of debug prints by removing the 
 *  definition of DEBUG.
 *
 *  If compiled for the MircoBit, would print to serial instead of standard 
 *  output, given that a MicroBit instance is defined as 'uBit' 
 *  (ie. Microbit uBit; is in the scope of the macro.)
 *  
 *  
 *  @param ... printf arguments for printing.
 *  @sa printf
*/
#define DPRINTF(...)

/** @def DMEM_DUMP
 *  Debugging memory dump - print the memory at **mptr** for **size** bytes,
 *  in hexadecimal blocks of the size **blklen**, with function name and line 
 *  number. Debug print also prepends the current memory address and appends 
 *  a newline at approriate intervals to prevent text wrapping making output 
 *  illegible.
 *  Debug memory dump is only wqdefined when DEBUG is defined. 
 *  This allows the removal of debug prints by removing the definition of DEBUG.
 *
 *  If compiled for the MircoBit, would print to serial instead of standard 
 *  output, given that a MicroBit instance is defined as 'uBit' 
 *  (ie. Microbit uBit; is in the scope of the macro.)
 *  
 *  @param ptr Pointer to the memory to be dumped.
 *  @param size Size of the memory to be dumped in bytes.
 *  @param blklen Length of each space seperated block printed in bytes
*/
#define DMEM_DUMP(ptr, size, blklen)

#endif /* ifndef DEBUG */
//@}

/** @defgroup test Test Harness
 *  A small test harness for testing on the embeded devices such as the Microbit.
 *  Small tset harness for c/c++ development on the Microbit platform. 
 *  Simplies the process of writing test functions with macros, and measures 
 *  certain area of interests for the user, such as the execution time, which 
 *  line the testfailed etc. 
 *  Prints out a test report at the end for the user to measure how many test 
 *  succeeded, failed, along with infomation on why and where they failed
 *  
 *  An exmaple test program would look something like this:
 *  @code{c}
 *  #include "utest.h"
 *  void test_func(TestState* _state_){
 *      TEST_TRUE(1 == 1); // assert true or fail test
 *      TEST_EQUAL(1, 1); // assert equal or fail test
 *      TEST_FALSE(1 == 2); // assert false or fail test
 *      TEST_NOT_EQUAL(1, 2); // assert not equal or fail test.
 *      TEST_MEM_EQUAL("meh","meh",4)// assert memory equal or fail test
 *      TEST_NOT_MEM_EQUAL("meh", "neh", 4) //assert memory not equal or fail test 
 *  } // a test.
 *  
 *  int main()
 *  {
 *      //Setup Code
 *      ....
 *  
 *      //Testing
 *      TEST_BEGIN // setup test enviroment
 *      TEST(test_func) // run test 'test_func'
 *      TEST_END // test cleanup and print test report.
 *  
 *      //Cleanup Code
 *      ....
 *  }
 *  @endcode
*/
//@{

/** @typedef TestFunc
 *  Type to encapsulate a test function
*/
typedef void (TestFunc)();

/* Limits */
#define TEST_STATE_LIMIT_NAME_STRLEN 40
#define TEST_STATE_LIMIT_REASON_STRLEN 100

/** @typedef TestState
 *  Type to encapsulate test state data;
*/
typedef struct test_state_t TestState;

/* Defining thread_local for thread safety.*/
#ifndef thread_local
    #if defined __GNUC__
        #define thread_local __thread
    #elif __STDC_VERSION__ >= 201112L
        #define thread_local _Thread_local
    #elif defined(_MSC_VER)
        #define thread_local __declspec(thread)
    #else
        #warn Cannot define thread_local. uTest will NOT be thread safe.
        #define thread_local //thread_local expands to nothing
    #endif /* if defined __GNUC__ */
#endif /* ifndef thread_local */

/** @private */
TestState *_test_setup();
/** @def TEST_BEGIN
 *  Use this macro before running tests:
 *  @code{.c}
 *  TEST_BEGIN;
 *  //... Run Tests
 *  @endcode
 *  Sets up the testing enviroment.
*/
#define TEST_BEGIN \
    TestState *_test_state = _test_setup();
    
/** @private */
void _test_report(TestState *state);
/** @private */
void _test_run(TestState *tstate, TestFunc *tfunc, const char *fname);

/** @def TEST
 * Use this macro between TEST_BEGIN and TEST_END:
 *      TEST_BEGIN;
 *      TEST(test_function)
 *      TEST_END;
 * Run the test by the given test function and commits its Test State.
 *
 * @param tfunc The test function to run. It has to be of type void (tfunc)()
*/
#define TEST(tfunc) \
    do{ \
        _test_run(_test_state, tfunc, #tfunc); \
        _test_report(_test_state); \
    } while(0)

/** @private */
void _test_end(TestState **state);
/** @def TEST_END 
 *  Use this macro after running tests:
 *       //... Run Tests
 *      TEST_END
 *  Cleans up the test enviroment and prints summary/report of the tests that
 *  have been run.
*/
#define TEST_END \
    do{ \
        _test_end(&_test_state); \
        _test_state = NULL; \
    }while(0)

/** @private */
void _test_fail(const char *msg, int line);

/** @def TEST_FAIL
 *  Use this marcro in a test function to cause the test to fail with a 'msg'
 *  message to explain what went wrong.
 *  When this macro is executed, the test would be marked as a failure, after
 *  which the current test would stop execution, leaving the rest of the test
 *  function unrun.
 *  
 *  @param msg A short message describing the reason why the test failed.
*/
#define TEST_FAIL(msg) _test_fail(_test_state, msg, __LINE__)

/** @def TEST_TRUE
 *  Test whether the given condition is true or fail the test if proven 
 *  otherwise.
 *  Would automatically generate a failure message if the condition is false, 
 *  explaining why the test fails.
 *
 *  @param cond The condition to test for validity
*/
#define TEST_TRUE(cond) \
    do{ \
        if(!( cond )) \
        {_test_fail("TEST_TRUE: Condition False: " #cond, __LINE__);} \
    }while(0)
 
/** @def TEST_FALSE
 *  Test whether the given condition is false or fail the test if proven 
 *  otherwise.
 *  Would automatically generate a failure message if the condition is true, 
 *  explaining why the test fails.
 *
 *  @param cond The condition to test for validity
*/
#define TEST_FALSE(cond) \
    do{ \
        if(( cond )) \
        {_test_fail("TEST_FALSE: Condition True: " #cond, __LINE__);} \
    }while(0)

/** @def TEST_EQUAL
 *  Test whether the given two arguments are equal, or fail the test if proven
 *  otherwise.
 *  Would automatically generate a failure message if the arguments are not 
 *  equal explaining why the test fails.
 *
 *  @param lhs The left or first argument used in the comparision test
 *  @param lhs The right or second argument used in the comparision test
*/
#define TEST_EQUAL(lhs, rhs) \
    do{ \
        if(lhs != rhs) \
        { _test_fail("TEST_EQUAL: Arguments not equal: " #lhs " NOT EQUAL " #rhs,\
            __LINE__); } \
    }while(0)

/** @def TEST_NOT_EQUAL
 *  Test whether the given two arguments are not equal, or fail the test if 
 *  proven otherwise.
 *  Would automatically generate a failure message if the arguments are equal 
 *  explaining why the test fails.
 *
 *  @param lhs The left or first argument used in the comparision test
 *  @param lhs The right or second argument used in the comparision test
*/
#define TEST_NOT_EQUAL(lhs, rhs)\
    do{ \
        if(lhs == rhs) \
        {_test_fail("TEST_NOT_EQUAL: Arguments equal: " #lhs " EQUAL " #rhs,\
            __LINE__); } \
    }while(0)

/** @def TEST_MEM_EQUAL
 *  Test whether the blobs of memory given by the two passed pointer are equal, 
 *  or fail the test if proven otherwise. The memory blobs should 
 *  be of the given size
 *  Would automatically generate a failure message if the arguments are not 
 *  equal explaining why the test fails.
 *
 *  @param lhs The left or first pointer used in the memory comparision test
 *  @param rhs The right or second pointer used in the memory test
 *  @param size The size of the memory blobs to compare
*/
#define TEST_MEM_EQUAL(lhs, rhs, size) \
    do{ \
        if(memcmp((void *)lhs, (void *)rhs, (size_t)size) != 0) \
        { _test_fail("TEST_MEM_EQUAL: Arguments not equal: " #lhs " NOT EQUAL " #rhs,\
            __LINE__); } \
    }while(0)

/** @def TEST_NOT_MEM_EQUAL
 *  Test whether the blobs of memory given by the two passed pointer are equal, 
 *  or fail the test if proven otherwise. The memory blobs should be of the 
 *  given size.
 *  Would automatically generate a failure message if the arguments are equal 
 *  explaining why the test fails.
 *
 *  @param lhs The left or first argument used in the comparision test
 *  @param rhs The right or second argument used in the comparision test
 *  @param size The size of the memory blobs to compare
*/
#define TEST_NOT_MEM_EQUAL(lhs, rhs, size)\
    do{ \
        if(memcmp((void *)lhs, (void *)rhs, (size_t)size) == 0) \
        {_test_fail("TEST_NOT_MEM_EQUAL: Arguments equal: " #lhs " EQUAL " #rhs,\
            __LINE__); } \
    }while(0)


//@}

#endif /* ifndef UTEST_H */
