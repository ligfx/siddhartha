#ifndef SIDDARTHA
#define SIDDARTHA

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #define SIDD_API __declspec(dllexport)
  #else
    #define SIDD_API __declspec(dllimport)
  #endif
#else
  #if __GNUC__ >= 4
    #define SIDD_API __attribute__ ((visibility("default")))
  #else
    #define SIDD_API
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

	
	
#ifdef __cplusplus
}
#endif

#endif // SIDDARTHA
