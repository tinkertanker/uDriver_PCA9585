/*
 * src/utest.c 
 * Implmentation for μTest framework
*/

#include "utest.h"

//Debugging Utilities
#ifdef MICROBIT_H
#define PRINTF uBit.serial.printf
#else
#define PRINTF printf
#endif /* ifdef MICROBIT_H */

void _debug_mem_dump(void *mptr, size_t len, size_t blk, const char *fname, \
        int line)
{
    if(!mptr || !len || !blk) return;
    blk = (blk > sizeof(unsigned long)) ? sizeof(unsigned long) : blk;

    //Opening Header 
    PRINTF("DEBUG: %s: %d: %s",fname,line, "Memory Dump BEGIN");

    const char *ptr = (const char *)mptr;
    
    //Dump Memory
    for(size_t pos = 0; pos < len; pos += blk)
    {
        if(pos % 20 == 0) //newline + Address Header
        { PRINTF("\r\nDEBUG:%#08lx: ", (unsigned long) pos ); }

        if(blk <= sizeof(char)) 
            PRINTF("%" "2hh" "X",  *((unsigned char *) ptr + pos)); 
        else if(blk <= sizeof(short)) 
            PRINTF("%" "4h" "X",  *((unsigned short *) ptr + pos)); 
        else if(blk <= sizeof(int)) 
            PRINTF("%" "8" "X",  *((unsigned int *) ptr + pos)); 
        else if(blk <= sizeof(long)) 
            PRINTF("%" "8l" "X",  *((unsigned long *) ptr + pos)); 
        else if(blk <= sizeof(long long)) 
            PRINTF("%" "16ll" "X",  *((unsigned long long *) ptr + pos)); 
        
        PRINTF(" "); //Space between blocks
    }
    
    //Closing Marker
    PRINTF("\r\nDEBUG: %s: %d: %s\r\n",fname,line, "Memory Dump END");
}

//Test Harness
#define TEST_LJMP_FAIL -1

/** @private */
struct test_state_t
{
    char name[TEST_STATE_LIMIT_NAME_STRLEN];
    TestFunc *func;
    bool status;
    char reason[TEST_STATE_LIMIT_REASON_STRLEN];
    int16_t line;
    double time;
    jmp_buf jmpbuf;
    uint16_t tlen;
    uint16_t tpass;
};

/** @private Global Variable for test failure handling */
#ifdef thread_local
static thread_local TestState *_current_test_state;
#else
#warning uTest will not be thread safe: Thread local variables are not supported
static TestState *_current_test_state;
#endif

void test_signal_handler(int sig)
{
    char *sbuf = (char *)malloc(sizeof(char) * 30);
    snprintf(sbuf, sizeof(char) *30, "Caught Deadly Signal: %d", sig);

    if(sig == SIGHUP) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGHUP");
    else if(sig == SIGINT) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGINT");
    else if(sig == SIGQUIT) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGQUIT");
    else if(sig == SIGILL ) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGILL ");
    else if(sig == SIGTRAP) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGTRAP");
    else if(sig == SIGABRT) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGABRT");
    else if(sig == SIGFPE ) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGFPE ");
    else if(sig == SIGKILL) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGKILL");
    else if(sig == SIGBUS ) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGBUS ");
    else if(sig == SIGSEGV) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGSEGV");
    else if(sig == SIGSYS ) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGSYS ");
    else if(sig == SIGPIPE) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGPIPE");
    else if(sig == SIGALRM) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGALRM");
    else if(sig == SIGTERM) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGTERM");
    else if(sig == SIGTSTP) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGTSTP");
    else if(sig == SIGTTIN) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGTTIN");
    else if(sig == SIGTTOU) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGTTOU");
    else if(sig == SIGXCPU) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGXCPU");
    else if(sig == SIGXFSZ) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGXFSZ");
    else if(sig == SIGVTALRM) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGVTALRM");
    else if(sig == SIGPROF) snprintf(sbuf, sizeof(char) * 30, "Caught Signal: SIGPROF");

    _test_fail(sbuf, -1);
    free(sbuf);
}

TestState *_test_setup()
{
    //Signal Catching
    signal(SIGHUP, &test_signal_handler);
    signal(SIGINT, &test_signal_handler);
    signal(SIGQUIT, &test_signal_handler);
    signal(SIGILL, &test_signal_handler);
    signal(SIGTRAP, &test_signal_handler);
    signal(SIGABRT, &test_signal_handler);
    signal(SIGFPE, &test_signal_handler);
    signal(SIGKILL, &test_signal_handler);
    signal(SIGBUS, &test_signal_handler);
    signal(SIGSEGV, &test_signal_handler);
    signal(SIGSYS, &test_signal_handler);
    signal(SIGPIPE, &test_signal_handler);
    signal(SIGALRM, &test_signal_handler);
    signal(SIGTERM, &test_signal_handler);
    signal(SIGTSTP, &test_signal_handler);
    signal(SIGTTIN, &test_signal_handler);
    signal(SIGTTOU, &test_signal_handler);
    signal(SIGXCPU, &test_signal_handler);
    signal(SIGXFSZ, &test_signal_handler);
    signal(SIGVTALRM, &test_signal_handler);
    signal(SIGPROF, &test_signal_handler);

    //Setup Test State
    TestState *tstate = (TestState *)malloc(sizeof(TestState));
    memset(tstate, 0, sizeof(TestState));

    PRINTF("*=====TEST BEGIN===---\r\n");

    return tstate;
}

void _test_run(TestState *tstate, TestFunc *tfunc, const char *fname)
{
    if(!tstate || !tfunc) return;
    
    strncpy(tstate->name, fname, TEST_STATE_LIMIT_NAME_STRLEN);
    tstate->func = tfunc;
    tstate->status = false;
    memset(tstate->reason, 0, sizeof(char) * TEST_STATE_LIMIT_REASON_STRLEN);
    tstate->line = -1;
    tstate->time = 0.0;
    memset(tstate->jmpbuf, 0, sizeof(jmp_buf));

    //Timing Runtime
    clock_t clock_begin = clock();
    time_t time_begin = time(NULL);
    
    _current_test_state = tstate;
    //Setup jmpbuf to jump out of the failing test
    if(setjmp(tstate->jmpbuf) != TEST_LJMP_FAIL)
    {
        //Run Test
        (*tfunc)();
        tstate->status = true;
    }
    else
    {
        //Long Jmp from failing test
        tstate->status = false;
    }
    
    //Compute Time Taken
    double clock_elapse = 
        (double)(clock() - clock_begin) / (double) CLOCKS_PER_SEC;
    double time_elapse = (double) time(NULL) - time_begin;
    
    //Perfer Clock's increased precision but use time if clock overflows
    tstate->time = (clock_elapse > time_elapse) ? 
        clock_elapse : time_elapse;
    
}

void _test_fail(const char *msg, int line)
{
    TestState *tstate = _current_test_state;

    tstate->status = false; //Test Failed
    strncpy(tstate->reason, msg, TEST_STATE_LIMIT_REASON_STRLEN);
    tstate->line = (line < 1) ? -1 : line;

    longjmp(tstate->jmpbuf, TEST_LJMP_FAIL);
}

char *human_readable_time(double tsec)
{
    char *htime = (char *)malloc(sizeof(char) * 10);

    if(tsec > 60.0) 
    { snprintf(htime, sizeof(char) * 10, "%.3f min", tsec / 60.0); }
    else if(tsec > 1.0)
    { snprintf(htime, sizeof(char) * 10, "%.3f s", tsec); }
    else if(tsec > 0.001)
    { snprintf(htime, sizeof(char) * 10, "%.3f ms", tsec * 1000.0); }
    else if(tsec > 0.000001)
    { snprintf(htime, sizeof(char) * 10, "%.3f us", tsec * 1000000.0); }
    else
    { snprintf(htime, sizeof(char) * 10, "%.3f ns", tsec * 1000000000.0); }

    return htime;
}

void _test_report(TestState *tstate)
{
    if(!tstate) return;

    //Print Test report/summary
    char *htime = human_readable_time(tstate->time);
    char sstatus[20];
    if(!tstate->status) 
    {
        (tstate->line < 1) ? snprintf(sstatus, sizeof(char) * 20, "FAIL") : 
            snprintf(sstatus, sizeof(char) * 20, "FAIL@%d", tstate->line);
    }
    else
    { snprintf(sstatus, sizeof(char) * 20, "PASS"); }

    PRINTF("* Run %-30s - %s [%s]\r\n", tstate->name, sstatus, htime);
    if(!tstate->status) PRINTF("\t! %s\r\n", tstate->reason);
    free(htime);

    tstate->tlen ++;
    if(tstate->status == true) tstate->tpass ++;  //Test Passed
    
}

void _test_end(TestState **tstate_ptr)
{
    TestState *tstate = (*tstate_ptr);
    if(!tstate) return;
    
    //Print Test Summary
    PRINTF("*=====TEST END===---\r\n");
    PRINTF(" %d Tests Failed / %d Tests Run\r\n", tstate->tlen - tstate->tpass, tstate->tlen);
    PRINTF(" Overall: %s\r\n", (tstate->tpass >= tstate->tlen)  ? "PASS" : "FAIL");

    free(tstate);
}
