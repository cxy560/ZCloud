/**
******************************************************************************
* @file     zc_module_config.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    moudle config
******************************************************************************
*/
#ifndef  __ZC_MOUDLE_CONFIG_H__ 
#define  __ZC_MOUDLE_CONFIG_H__

#define MT7681

#if ZC_DEBUG
#ifdef ZC_OFF_LINETEST
#define ZC_Printf(format, ...) printf("\1\2\3\4"format"File:%s, Line:%d, Function:%s\n", ##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__)
#else
#define ZC_Printf(format, ...) Printf_High("\1\2\3\4"format"File:%s, Line:%d, Function:%s\n", ##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__)
#endif 
#else
#ifdef ZC_OFF_LINETEST
#define ZC_Printf(format, ...) printf(""format"", ##__VA_ARGS__)
#else
#define ZC_Printf(format, ...) Printf_High("\1\2\3\4"format"", ##__VA_ARGS__)
#endif 
#endif


#define ZC_malloc  malloc
#define ZC_free    free


#endif
/******************************* FILE END ***********************************/

