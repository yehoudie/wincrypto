#ifndef SHARED_DEBUG_H
#define SHARED_DEBUG_H

#define DEBUG_PRINT_INFO (0)
#define debug_info(...) if (DEBUG_PRINT_INFO) fprintf(stdout, __VA_ARGS__)
#define debug_winfo(...) if (DEBUG_PRINT_INFO) fwprintf(stdout, __VA_ARGS__)

#endif
