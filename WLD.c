/**
 * @file WLD.c
 * @author StanleyHsu (jkyou98@gmail.com)
 * @brief Source code of WLD
 * @version 0.1
 * @date 2022-02-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "WLD.h"
#include "Border_Reflect.h"
#include "image_conv.h"
#include "param.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define PI acosf (-1)

static void
Differential_Excitation (uint8_t *proc_arr, float *result, uint8_t *image,
                         int16_t *Differential_conv, uint16_t width,
                         uint16_t height)
{
  int8_t DE_Kernel_0[9] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };

  float k;
  image_conv_K_int8_t (proc_arr, DE_Kernel_0, Differential_conv, height, width,
                       3);

  for (uint32_t i = 0; i < (uint32_t)height * width; i++)
    {
      k = (float)Differential_conv[i] / (image[i] + 0.000001);
      result[i] = atanf (k);
    }
}

static void
Orientation (uint8_t *proc_arr, uint8_t *result, int16_t *grad_x,
             int16_t *grad_y, uint16_t width, uint16_t height, uint8_t bins)
{
  int t;
  int8_t Ori_Kernel_y[9] = { 0, -1, 0, 0, 0, 0, 0, 1, 0 };
  int8_t Ori_Kernel_x[9] = { 0, 0, 0, -1, 0, 1, 0, 0, 0 };

  float theta_p;
  image_conv_K_int8_t (proc_arr, Ori_Kernel_y, grad_y, height, width, 3);
  image_conv_K_int8_t (proc_arr, Ori_Kernel_x, grad_x, height, width, 3);
  for (uint32_t i = 0; i < (uint32_t)height * width; i++)
    {
      theta_p = atan2f ((float)grad_y[i], (float)grad_x[i]) + PI;
      t = ((uint8_t)((theta_p * bins / (2 * PI)) + 0.5)) % bins;

      result[i] = t;
    }
}

static void
WLD_Feat (float *DE_arr, uint8_t *Ori_arr, uint32_t *WLD_Hist,
          uint32_t *WLD_Hist_2D, uint16_t width, uint16_t height,
          uint8_t parts, uint8_t sub_bins, uint8_t ori_bins)
{

  uint8_t t;
  uint16_t c, C = parts * sub_bins;
  // 2D WLD Histogram Computation
  for (uint32_t i = 0; i < (uint32_t)height * width; ++i)
    {

      c = ((uint16_t)((DE_arr[i] + (PI / 2)) * ((C / PI)))) % C;
      t = Ori_arr[i];
      WLD_Hist_2D[c * ori_bins + t]++;
    }

  // Transform to 1D arrangement
  for (uint8_t i = 0; i < parts; i++)
    {
      for (uint8_t j = 0; j < ori_bins; j++)
        {
          for (uint8_t k = 0; k < sub_bins; k++)
            {
              WLD_Hist[i * sub_bins * ori_bins + j * sub_bins + k]
                  = WLD_Hist_2D[i * sub_bins * ori_bins + k * ori_bins + j];
            }
        }
    }
}
const char *
WLD_error_str (WLD_error_t err)
{
  const char *err_str = NULL;
  const char *const WLD_ERROR_STRS[] = {
    "WLD_ERROR_OK",
    "WLD_ERROR_INVARG",
  };
  // Ensure error codes are within the valid array index range
  if (err >= WLD_ERROR_COUNT)
    {
      goto done;
    }

  err_str = WLD_ERROR_STRS[err];

done:
  return err_str;
}

WLD_error_t
WLD (ImageInfo *IMG, WLD_feat *param)
{
  if (IMG->image == NULL || param->WLD_Hist == NULL
      || param->WLD_Hist_2D == NULL || param->image_padding == NULL
      || param->DE_info.Differential_Excitation == NULL
      || param->DE_info.Differential_conv == NULL
      || param->Ori_info.Orientation == NULL || param->Ori_info.grad_x == NULL
      || param->Ori_info.grad_y == NULL)
    return WLD_ERROR_INVARG;
  Border_Reflect ((uint8_t *)param->image_padding, (uint8_t *)IMG->image,
                  (uint8_t)IMG->width, (uint8_t)IMG->height, 3);
  Differential_Excitation (
      (uint8_t *)param->image_padding,
      (float *)param->DE_info.Differential_Excitation, (uint8_t *)IMG->image,
      (int16_t *)param->DE_info.Differential_conv, IMG->width, IMG->height);
  Orientation (
      (uint8_t *)param->image_padding, (uint8_t *)param->Ori_info.Orientation,
      (int16_t *)param->Ori_info.grad_x, (int16_t *)param->Ori_info.grad_y,
      IMG->width, IMG->height, param->ori_bins);
  WLD_Feat ((float *)param->DE_info.Differential_Excitation,
            (uint8_t *)param->Ori_info.Orientation,
            (uint32_t *)param->WLD_Hist, (uint32_t *)param->WLD_Hist_2D,
            IMG->width, IMG->height, param->part, param->sub_bins,
            param->ori_bins);
  return WLD_ERROR_OK;
}