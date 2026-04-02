/* ============================================================
   Modified by Greentechnical on 2026-02-20.
   Changes:
   - Added <io.h>, <process.h>, <sys/stat.h>, and <signal.h>
     includes for MSVC/Windows compatibility.
   - Added Windows compatibility definitions for STDOUT_FILENO,
     STDERR_FILENO, S_ISDIR, and SIGUSR1.
   - Added a minimal sigaction/SA_RESTART/sigemptyset shim for
     Windows builds.
   - Corrected minor comment wording/typos.
   Based on LIGGGHTS-PUBLIC commit:
   3d5c00f20519e6bb6eb6756f51f1ad36564e649d
   Original project:
   https://github.com/CFDEMproject/LIGGGHTS-PUBLIC
   ============================================================ */

#include <iso646.h>
#if !defined(__MINGW32__) && !defined(_WINDOWS)
#include "erf.h"
#endif
#include "direct.h"
#include <cmath>
#include <io.h>
#include <process.h>
#include <sys/stat.h>
#include <signal.h>

// LAMMPS uses usleep with 100 ms arguments, no microsecond precision needed
#if !defined(__MINGW32__) && !defined(_WINDOWS)
#include "sleep.h"
#endif

// Windows equivalents for POSIX file descriptors
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

// S_ISDIR macro for Windows
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif

// SIGUSR1 doesn't exist on Windows - define as unused signal
#ifndef SIGUSR1
#define SIGUSR1 10
#endif

// sigaction doesn't exist on Windows - provide a minimal shim
#ifndef _SIGACTION_DEFINED
#define _SIGACTION_DEFINED
struct sigaction {
    void (*sa_handler)(int);
    int sa_flags;
    int sa_mask;
};
#define SA_RESTART 0
#define sigemptyset(x) (*(x) = 0)
inline int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    void (*old_handler)(int) = signal(signum, act->sa_handler);
    if (oldact) oldact->sa_handler = old_handler;
    return 0;
}
#endif

// some symbols have different names in Windows
#undef ATOBIGINT
#define ATOBIGINT _atoi64
#define pclose _pclose
#define __restrict__ __restrict

// the following functions are defined to get rid of
// 'ambiguous call to overloaded function' error in VSS for mismatched type arguments
//#if !defined(__MINGW32__)
//inline double pow(int i, int j){
//  return pow((double)i,j);
//}
//#endif
#if !defined(__MINGW32__)
inline double sqrt(int i){
  return sqrt((double) i);
}
#endif
inline double fabs(int i){
  return fabs((double) i);
}
inline double round(double d)
{
  return floor(d + 0.5);
}
inline double trunc(double x) {
  return x > 0 ? floor(x) : ceil(x);
}
// Windows version of mkdir function does not have permission flags
# define S_IRWXU 0
# define S_IRGRP 0
# define S_IXGRP 0
inline int mkdir(const char *path, int){
  return _mkdir(path);
}