// create_lwi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#pragma once


/*  lwlibav_source.cppのinclude  */
#include <stdio.h>
#include "lsmashsource.h"

#include <iostream>
extern "C"
{
  /* Libav
  * The binary file will be LGPLed or GPLed. */
#include <libavformat/avformat.h>      /* Codec specific info importer */
#include <libavcodec/avcodec.h>        /* Decoder */
#include <libswscale/swscale.h>        /* Colorspace converter */
#include <libavresample/avresample.h>  /* Audio resampler */
#include <libavutil/imgutils.h>
#include <libavutil/mem.h>
#include <libavutil/opt.h>
}

#include "video_output.h"
#include "audio_output.h"
#include "lwlibav_source.h"

#pragma warning( disable:4996 )



/*  lwlibav_source.hから  */
VideoInfo                      vi;
lwlibav_file_handler_t         lwh;
lwlibav_video_decode_handler_t vdh;
lwlibav_video_output_handler_t voh;

/*  reference  */
/*  lwlibav_source.cpp  LWLibavVideoSource::LWLibavVideoSource(...)  */
int LWLibavVideoSource__CrLwi
(
lwlibav_option_t   *opt,
int                 seek_mode,
uint32_t            forward_seek_threshold,
int                 direct_rendering,
int                 stacked_format,
enum AVPixelFormat  pixel_format,
  IScriptEnvironment   *env,
  crlwi_setting_handler__CrLwi  *clshp
  )
{
  memset(&vi, 0, sizeof(VideoInfo));
  memset(&lwh, 0, sizeof(lwlibav_file_handler_t));
  memset(&vdh, 0, sizeof(lwlibav_video_decode_handler_t));
  memset(&voh, 0, sizeof(lwlibav_video_output_handler_t));
  vdh.seek_mode = seek_mode;
  vdh.forward_seek_threshold = forward_seek_threshold;
  as_video_output_handler_t *as_vohp = (as_video_output_handler_t *)lw_malloc_zero(sizeof(as_video_output_handler_t));
  if (!as_vohp)
    /*create_lwi  エラー表示をしないで終了*/
    //env->ThrowError("LWLibavVideoSource: failed to allocate the AviSynth video output handler.");
    return 1;
  as_vohp->vi = &vi;
  as_vohp->env = env;
  voh.private_handler = as_vohp;
  voh.free_private_handler = as_free_video_output_handler;
  /* Set up error handler. */
  lw_log_handler_t lh;
  lh.level = LW_LOG_FATAL;   /* Ignore other than fatal error. */
  lh.priv = env;
  lh.show_log = throw_error;
  lh.show_log = NULL;        /*create_lwi  ビデオオープン失敗時のダイアログ表示をけす。*/

  /* Set up progress indicator. */
  progress_indicator_t indicator;
  indicator.open = NULL;
  indicator.update = NULL;
  indicator.close = NULL;
  /* Construct index. */
  lwlibav_audio_decode_handler_t adh = { 0 };
  lwlibav_audio_output_handler_t aoh = { 0 };
  int ret = lwlibav_construct_index__CrLwi(&lwh, &vdh, &voh, &adh, &aoh, &lh, opt, &indicator, NULL, clshp);
  lwlibav_cleanup_audio_decode_handler(&adh);
  lwlibav_cleanup_audio_output_handler(&aoh);
  return ret;

  /*インデックス作成が終わったので以降はスキップ*/
  /*create_lwi_off*/
  //if (ret < 0)
  //  env->ThrowError("LWLibavVideoSource: failed to construct index.");
  ///* Get the desired video track. */
  //if (lwlibav_get_desired_video_track(lwh.file_path, &vdh, lwh.threads) < 0)
  //  env->ThrowError("LWLibavVideoSource: failed to get the video track.");
  //vdh.lh = lh;
  ///* Set average framerate. */
  //int64_t fps_num = 25;
  //int64_t fps_den = 1;
  //lwlibav_setup_timestamp_info(&lwh, &vdh, &voh, &fps_num, &fps_den);
  //vi.fps_numerator = (unsigned int)fps_num;
  //vi.fps_denominator = (unsigned int)fps_den;
  //vi.num_frames = voh.frame_count;
  ///* */
  ////prepare_video_decoding(direct_rendering, stacked_format, pixel_format, env);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
//コマンドライン解析
//
void ParseCommandLine(int argc, char** argv,
                      crlwi_setting_handler__CrLwi *clshp)
{
  char file_path[_MAX_PATH] = { 0 };
  char lwi_path[_MAX_PATH] = { 0 };
  char file_path_inner_lwi[_MAX_PATH] = { 0 };
  bool mode_pipe_input = false;
  bool is_filename_inner_lwi = false;
  bool create_footer = false;
  double read_limit_MiBsec = 0.0;


  for (int i = 1; i < argc; i++)
  {
    //引数１つ目がファイルパス？
    if (i == 1)
    {
      FILE *fp = fopen(argv[1], "r");
      if (fp != NULL)
      {
        mode_pipe_input = false;
        sprintf(file_path, "%s", argv[i]);
        fclose(fp);
      }
    }

    //-file
    if (_stricmp(argv[i], "-file") == 0)
    {
      mode_pipe_input = false;
      if (i + 1 < argc)  
        sprintf(file_path, "%s", argv[i + 1]);
    }
    //-pipe
    else if (_stricmp(argv[i], "-pipe") == 0)
    {
      mode_pipe_input = true;
      if (i + 1 < argc) 
        sprintf(file_path, "%s", argv[i + 1]);
    }
    //-lwi
    else if (_stricmp(argv[i], "-lwi") == 0)
    {
      if (i + 1 < argc) 
        sprintf(lwi_path, "%s", argv[i + 1]);
    }
    //-footer
    else if (_stricmp(argv[i], "-footer") == 0)
    {
      create_footer = true;
    }
    //-ref_filename
    else if (_stricmp(argv[i], "-ref_filename") == 0)
    {
      is_filename_inner_lwi = false;
    }
    //-limit
    else if (_stricmp(argv[i], "-limit") == 0)
    {
      if (i + 1 < argc)
        if ((_snscanf(argv[i + 1], 6, "%lf", &read_limit_MiBsec)) <= 0)
          read_limit_MiBsec = 0; 
    }
  }

  //parser post process
  {
    //create lwipath from filepath
    if (_stricmp(lwi_path, "") == 0)
      sprintf(lwi_path, "%s.lwi", file_path);

    //append .lwi extension
    char ext[_MAX_EXT];
    _splitpath(lwi_path, NULL, NULL, NULL, ext);
    if (_stricmp(ext, ".lwi") != 0)
    {
      char tmp[_MAX_PATH] = { 0 };
      sprintf(tmp, "%s.lwi", lwi_path);
      sprintf(lwi_path, "%s", tmp);
    }

    if (is_filename_inner_lwi)
    {
      //fullpath  →  filename
      char name[_MAX_PATH], ext[_MAX_EXT];
      _splitpath(file_path, NULL, NULL, name, ext);
      sprintf(file_path_inner_lwi, "%s%s", name, ext);
    }
    else
      sprintf(file_path_inner_lwi, "%s", file_path);
  }

  //parse result
  {
    memset(clshp->file_path, 0, _MAX_PATH);
    memset(clshp->lwi_path, 0, _MAX_PATH);
    memset(clshp->file_path_inner_lwi, 0, _MAX_PATH);
    sprintf(clshp->file_path, "%s", file_path);
    sprintf(clshp->lwi_path, "%s", lwi_path);
    sprintf(clshp->file_path_inner_lwi, "%s", file_path_inner_lwi);
    clshp->mode_pipe_input = mode_pipe_input;
    clshp->create_footer = mode_pipe_input && create_footer;
    clshp->read_limit_MiBsec = read_limit_MiBsec;
  }
}


//
//debug commandline argument
//  -file "E:\TS_Samp\n60s.ts"  -ref_filename  -lwi "E:\TS_Samp\n60s.ts.debug.lwi"
//  -file "E:\TS_Samp\scramble_180s.ts"  -ref_filename  -lwi "E:\TS_Samp\scramble_180s.ts.debug.lwi"
//
// lwindex.c
//   line 3200    static void create_index__crlwi(...)



//
//　Main
//
int main(size_t argc, char* argv[])
{

  crlwi_setting_handler__CrLwi clshp;
  ParseCommandLine(argc, argv, &clshp);

  //check file existance
  if (clshp.mode_pipe_input == false)
  {
    FILE *fp = fopen(clshp.file_path, "r");
    if (fp == NULL)
      return 1;
    fclose(fp);
  }


  ///*  reference  */
  ///*  lwlibav_source.cpp  AVSValue CreateLWLibavVideoSource(...)  */
  int stacked_format = 0;
  enum AVPixelFormat pixel_format = AV_PIX_FMT_NONE;       // get_av_output_pixel_format(args[12].AsString(NULL));
  lwlibav_option_t opt;
  opt.file_path = clshp.file_path;
  opt.threads = -1;
  opt.av_sync = 0;
  opt.no_create_index = 0;
  opt.force_video = 0;
  opt.force_video_index = 0;
  opt.force_audio = 0;
  opt.force_audio_index = -1;
  opt.apply_repeat_flag = 0;
  opt.field_dominance = 0;   /* 0: Obey source flags, 1: TFF, 2: BFF */
  opt.vfr2cfr.active = 0;
  opt.vfr2cfr.fps_num = 0;
  opt.vfr2cfr.fps_den = 0;
  int  seek_mode = 0;
  int  forward_seek_threshold = 10;
  int direct_rendering = 0;  //AV_PIX_FMT_NONE==-1
  IScriptEnvironment *env = NULL;

  int ret_code = LWLibavVideoSource__CrLwi(&opt, seek_mode, forward_seek_threshold, direct_rendering, stacked_format, pixel_format, env, &clshp);
  return ret_code;
}
