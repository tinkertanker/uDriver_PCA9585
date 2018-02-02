/*
 * src/utest.c 
 * Implmentation for μTest framework
*/

#include "utest.h"

//Debugging Utilities
#ifndef MICROBIT_H
#define PRINTF printf
#else
static MicroBit uBit;
#define PRINTF uBit.serial.printf
#endif /* ifndef MICROBIT_H */


void _debug_mem_dump(void *mptr, size_t len, size_t blk, const char *fname, \
        int line)
{
    if(!mptr || !len || !blk) return;
    blk = (blk > sizeof(unsigned long)) ? sizeof(unsigned long) : blk;

    //Opening Header 
    PRINTF("DEBUG: %s: %d: %s",fname,line, "Memory Dump BEGIN");

    const char *ptr = mptr;
    
    //Dump Memory
    for(size_t pos = 0; pos < len; pos += blk)
    {
        if(pos % 20 == 0) //newline + Address Header
        { PRINTF("\r\nDEBUG:%#08lx: ", (unsigned long) pos ); }

        if(blk <= sizeof(char)) 
            PRINTF("%" "hh" "X",  *((unsigned char *) ptr + pos)); 
        else if(blk <= sizeof(short)) 
            PRINTF("%" "h" "X",  *((unsigned short *) ptr + pos)); 
        else if(blk <= sizeof(int)) 
            PRINTF("%" "" "X",  *((unsigned int *) ptr + pos)); 
        else if(blk <= sizeof(long)) 
            PRINTF("%" "l" "X",  *((unsigned long *) ptr + pos)); 
        else if(blk <= sizeof(long long)) 
            PRINTF("%" "ll" "X",  *((unsigned long long *) ptr + pos)); 
        
        PRINTF(" "); //Space between blocks
    }
    
    //Closing Marker
    PRINTF("\r\nDEBUG: %s: %d: %s\r\n",fname,line, "Memory Dump END");
}

//Test Harness
#define TEST_LJMP_FAIL -1

#ifdef thread_local
    thread_local TestState *_current_test_state = NULL;
#else
    TestState *_current_test_state = NULL;
#endif

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
    struct test_state_t *next;
};

void test_signal_handler(int sig)
{
    char *sbuf = malloc(sizeof(char) * 30);
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
    TestState *tstate = malloc(sizeof(TestState));
    memset(tstate, 0, sizeof(TestState));

    return tstate;
}

void _test_run(TestState *tstate, TestFunc *tfunc, const char *fname)
{
    if(!tstate || !tfunc) return;
    
    //Configure Current Test State
    if(!_current_test_state) _current_test_state = tstate; //First Stuct not used
    else 
    {
        _current_test_state->next = malloc(sizeof(TestState));
        memset(_current_test_state->next, 0, sizeof(TestState));
        _current_test_state =  _current_test_state->next;
    }

    strncpy(_current_test_state->name, fname, TEST_STATE_LIMIT_NAME_STRLEN);
    _current_test_state->func = tfunc;
    _current_test_state->status = false;
    _current_test_state->line = -1;
    _current_test_state->time = 0.0;
    _current_test_state->next = NULL;

    //Timing Runtime
    clock_t clock_begin = clock();
    time_t time_begin = time(NULL);
    
    PRINTF("Run %s()...", fname);
    
    //Setup jmpbuf to jump out of the failing test
    if(setjmp(_current_test_state->jmpbuf) == TEST_LJMP_FAIL)
    {
        //Long Jmp from failing test
        PRINTF("FAIL\r\n");
        _current_test_state->status = false;
    }
    else //Run Test
    {
        (*tfunc)();
        PRINTF("PASS\r\n");
        _current_test_state->status = true;
    }
    
    //Compute Time Taken
    double clock_elapse = 
        (double)(clock() - clock_begin) / (double) CLOCKS_PER_SEC;
    double time_elapse = (double) time(NULL) - time_begin;
    
    //Perfer Clock's increased precision but use time if clock overflows
    _current_test_state->time = (clock_elapse > time_elapse) ? 
        clock_elapse : time_elapse;
    
}

void _test_fail(const char *msg, int line)
{
    strncpy(_current_test_state->reason, msg, TEST_STATE_LIMIT_REASON_STRLEN);
    _current_test_state->line = (line < 1) ? -1 : line;

    longjmp(_current_test_state->jmpbuf, TEST_LJMP_FAIL);
}

char *human_readable_time(double tsec)
{
    char *htime = malloc(sizeof(char) * 10);

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

    PRINTF("---===TEST REPORT===---\r\n");

    int tlen = 0; //Determine Number of Tests Run
    int tpass = 0; //Determine Number of Tests Run without failure
    
    TestState *ts = NULL;
    do 
    {
        ts = (!ts) ? tstate : ts->next;

        //Print Test report/summary
        char *htime = human_readable_time(ts->time);
        char sstatus[20];
        if(!ts->status) 
        {
            (ts->line < 1) ? snprintf(sstatus, sizeof(char) * 20, "FAIL") : 
                snprintf(sstatus, sizeof(char) * 20, "FAIL@%d", ts->line);
        }
        else
        { snprintf(sstatus, sizeof(char) * 20, "PASS"); }
    
        PRINTF("Test %s() - %s [%s]\r\n", ts->name, sstatus, htime);
        PRINTF("\t%s\r\n", ts->reason);
        free(htime);

        tlen ++;
        if(ts->status == true) // Test Passed
            tpass ++; 
    }while(ts->next != NULL);

    
    PRINTF("============================\r\n");
    PRINTF(" %d Tests Failed / %d Tests Run\r\n", tlen - tpass, tlen);
    PRINTF(" Overall: %s\r\n", (tpass >= tlen)  ? "PASS" : "FAIL");
    PRINTF("---===TEST END===---\r\n");
}

void _test_cleanup(TestState **tstate_ptr)
{
    TestState *tstate = (*tstate_ptr);
    if(!tstate) return;
    
    
    TestState *ts = tstate;
    TestState *ts_next = tstate;

    do
    {
        ts = ts_next;
        ts_next = ts->next;
        free(ts);
    }while(ts_next != NULL);
    
    _current_test_state = NULL;
    (*tstate_ptr) = NULL;
}
