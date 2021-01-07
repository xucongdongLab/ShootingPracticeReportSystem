//
// MATLAB Compiler: 4.18.1 (R2013a)
// Date: Sun Aug 16 23:43:14 2020
// Arguments: "-B" "macro_default" "-W" "cpplib:GetPos_Condor" "-T" "link:lib"
// "-d" "E:\Code\GetPosCondor\GetPos_Condor\src" "-w"
// "enable:specified_file_mismatch" "-w" "enable:repeated_file" "-w"
// "enable:switch_ignored" "-w" "enable:missing_lib_sentinel" "-w"
// "enable:demo_license" "-v" "E:\Code\GetPosCondor\GetPos_Condor.m" "-a"
// "E:\Code\GetPosCondor\matlabCondor.m" "-a"
// "E:\Code\GetPosCondor\matlabCondor.mexw32" "-a"
// "E:\Code\GetPosCondor\OF_GetPos_Condor.m" 
//

#ifndef __GetPos_Condor_h
#define __GetPos_Condor_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_GetPos_Condor
#define PUBLIC_GetPos_Condor_C_API __global
#else
#define PUBLIC_GetPos_Condor_C_API /* No import statement needed. */
#endif

#define LIB_GetPos_Condor_C_API PUBLIC_GetPos_Condor_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_GetPos_Condor
#define PUBLIC_GetPos_Condor_C_API __declspec(dllexport)
#else
#define PUBLIC_GetPos_Condor_C_API __declspec(dllimport)
#endif

#define LIB_GetPos_Condor_C_API PUBLIC_GetPos_Condor_C_API


#else

#define LIB_GetPos_Condor_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_GetPos_Condor_C_API 
#define LIB_GetPos_Condor_C_API /* No special import/export declaration */
#endif

extern LIB_GetPos_Condor_C_API 
bool MW_CALL_CONV GetPos_CondorInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_GetPos_Condor_C_API 
bool MW_CALL_CONV GetPos_CondorInitialize(void);

extern LIB_GetPos_Condor_C_API 
void MW_CALL_CONV GetPos_CondorTerminate(void);



extern LIB_GetPos_Condor_C_API 
void MW_CALL_CONV GetPos_CondorPrintStackTrace(void);

extern LIB_GetPos_Condor_C_API 
bool MW_CALL_CONV mlxGetPos_Condor(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_GetPos_Condor
#define PUBLIC_GetPos_Condor_CPP_API __declspec(dllexport)
#else
#define PUBLIC_GetPos_Condor_CPP_API __declspec(dllimport)
#endif

#define LIB_GetPos_Condor_CPP_API PUBLIC_GetPos_Condor_CPP_API

#else

#if !defined(LIB_GetPos_Condor_CPP_API)
#if defined(LIB_GetPos_Condor_C_API)
#define LIB_GetPos_Condor_CPP_API LIB_GetPos_Condor_C_API
#else
#define LIB_GetPos_Condor_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_GetPos_Condor_CPP_API void MW_CALL_CONV GetPos_Condor(int nargout, mwArray& opt, const mwArray& InitData, const mwArray& targetWidth, const mwArray& targetHeight, const mwArray& vspeed_min, const mwArray& vspeed_max, const mwArray& A_min, const mwArray& A_max, const mwArray& B_min, const mwArray& B_max);

#endif
#endif
