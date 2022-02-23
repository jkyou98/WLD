/**
 * @file WLD.h
 * @author StanleyHsu (jkyou98@gmail.com)
 * @brief Function prototypes for WLD
 * @version 0.1
 * @date 2022-02-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef WLD_H_
#define WLD_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "param.h"
  /// @brief Error codes for WLD
  typedef enum WLD_error_e
  {
    /// No error
    WLD_ERROR_OK = 0,
    /// Invalid arguments (ex.NuLL pointer, parameter overflow)
    WLD_ERROR_INVARG,
    /// Total # of errors in this list (NOT AN ACTUAL ERROR CODE);
    /// NOTE: that for this to work, it assumes your first error code is value
    /// 0 and you let it naturally increment from there, as is done above,
    /// without explicitly altering any error values above
    WLD_ERROR_COUNT
  } WLD_error_t;

  /**
   * @brief      Function to get a printable string from an enum error type
   * @param[in]  err   a valid error code for WLD module
   * @return     A printable C string corresponding to the error code input
   * above, or NULL if an invalid error code was passed in
   */
  const char *WLD_error_str (WLD_error_t err);
/**
 * @brief Weber Local Descriptor
 * 
 * @param IMG image struct
 * @param param paramter struct of WLD
 */
  WLD_error_t WLD (ImageInfo *IMG, WLD_feat *param);
#ifdef __cplusplus
}
#endif
#endif