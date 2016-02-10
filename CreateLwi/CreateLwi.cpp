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
void LWLibavVideoSource_B
(
lwlibav_option_t   *opt,
int                 seek_mode,
uint32_t            forward_seek_threshold,
int                 direct_rendering,
int                 stacked_format,
enum AVPixelFormat  pixel_format,
  IScriptEnvironment   *env,
  cmdlineinfo_handler__byCrLwi  *clih
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
    return;
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
  int ret = lwlibav_construct_index__byCrLwi(&lwh, &vdh, &voh, &adh, &aoh, &lh, opt, &indicator, NULL, clih);
  lwlibav_cleanup_audio_decode_handler(&adh);
  lwlibav_cleanup_audio_output_handler(&aoh);

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


//
//コマンドライン解析
//
void CommandLineParser(
  size_t argc, char* argv[],
  char* filepath, char* lwipath, char* filepath_innerlwi,
  cmdlineinfo_handler__byCrLwi* clih
  )
{
  bool mode_stdin = false;
  bool fullpath_innerlwi = true;
  bool create_footer = false;
  double readlimit_MiBsec = 0.0;

  for (size_t i = 1; i < argc; i++)
  {
    //引数１つ目がファイルパス？
    if (i == 1)
    {
      FILE *fp = fopen(argv[1], "r");
      if (fp != NULL)
      {
        mode_stdin = false;
        sprintf(filepath, "%s", argv[i]);
        fclose(fp);
      }
    }

    //-file
    if (_strnicmp(argv[i], "-file", 6) == 0)
    {
      mode_stdin = false;
      if (i + 1 < argc)  sprintf(filepath, "%s", argv[i + 1]);
    }
    //-pipe
    else if (_strnicmp(argv[i], "-pipe", 6) == 0)
    {
      mode_stdin = true;
      if (i + 1 < argc)  sprintf(filepath, "%s", argv[i + 1]);
    }
    //-lwi
    else if (_strnicmp(argv[i], "-lwi", 5) == 0)
    {
      if (i + 1 < argc)  sprintf(lwipath, "%s", argv[i + 1]);
    }
    //-footer
    else if (_strnicmp(argv[i], "-footer", 8) == 0)
    {
      create_footer = true;
    }
    //-ref_filename
    else if (_strnicmp(argv[i], "-ref_filename", 14) == 0)
    {
      fullpath_innerlwi = false;
    }
    //-limit
    else if (_strnicmp(argv[i], "-limit", 7) == 0)
    {
      if (i + 1 < argc)
        if ((_snscanf(argv[i + 1], 6, "%lf", &readlimit_MiBsec)) <= 0)
          readlimit_MiBsec = 0;        //変換失敗
    }
  }

  //lwipath
  //  指定なし？　filepathから lwipath作成
  if (_strnicmp(lwipath, "", 1) == 0)
    sprintf(lwipath, "%s", filepath);

  //  拡張子.lwiがなければつける
  char ext[64];
  _splitpath(lwipath, NULL, NULL, NULL, ext);
  if (_strnicmp(ext, ".lwi", 5) != 0)
    sprintf(lwipath, "%s.lwi", lwipath);

  //lwi内に書き込むfilepath
  if (fullpath_innerlwi)
    sprintf(filepath_innerlwi, "%s", filepath);
  else
  {
    //fullpath  →  filename
    char fname[1024], ext[64];
    _splitpath(filepath, NULL, NULL, fname, ext);
    sprintf(filepath_innerlwi, "%s%s", fname, ext);
  }

  //result
  clih->filepath = filepath;
  clih->filepath_innerlwi = filepath_innerlwi;
  clih->lwipath = lwipath;
  clih->create_footer = create_footer && mode_stdin;
  clih->mode_stdin = mode_stdin;
  clih->readlimit_MiBsec = readlimit_MiBsec;

  return;
}


//
//debug commandline argument
//  -file "E:\TS_Samp\n60s.ts"  -ref_filename  -lwi "E:\TS_Samp\n60s.ts.debug.lwi"
//  -file "E:\TS_Samp\scramble_180s.ts"  -ref_filename  -lwi "E:\TS_Samp\scramble_180s.ts.debug.lwi"
//

//
//　Main
//
int main(size_t argc, char* argv[])
{
  cmdlineinfo_handler__byCrLwi clih;
  char filepath[1024] = "";
  char lwipath[1024] = "";
  char filepath_innerlwi[1024] = "";

  CommandLineParser(
    argc, argv,
    filepath, lwipath, filepath_innerlwi,
    &clih);

  //ファイルの存在チェック
  if (clih.mode_stdin == false)
  {
    FILE *fp = fopen(clih.filepath, "r");
    if (fp == NULL)
      return 1;         //オープン失敗、終了
  }


  ///*  reference  */
  ///*  lwlibav_source.cpp  AVSValue CreateLWLibavVideoSource(...)  */
  int stacked_format = 0;
  enum AVPixelFormat pixel_format = AV_PIX_FMT_NONE;       // get_av_output_pixel_format(args[12].AsString(NULL));
  lwlibav_option_t opt;
  opt.file_path = clih.filepath;
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
  LWLibavVideoSource_B(&opt, seek_mode, forward_seek_threshold, direct_rendering, stacked_format, pixel_format, env, &clih);


  return 0;
}
