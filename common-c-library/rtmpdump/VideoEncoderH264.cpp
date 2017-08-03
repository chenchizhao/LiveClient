//
//  VideoEncoderH264.cpp
//  RtmpClient
//
//  Created by Max on 2017/7/17.
//  Copyright © 2017年 net.qdating. All rights reserved.
//

#include "VideoEncoderH264.h"

#include <common/CommonFunc.h>
#include <common/KLog.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
    
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
    
}

#define DEFAULT_VIDEO_BUFFER_COUNT 80
#define DEFAULT_VIDEO_BUFFER_SIZE 480 * 640 * 4

namespace coollive {
// 重采样线程
class ConvertEncodeVideoRunnable : public KRunnable {
public:
    ConvertEncodeVideoRunnable(VideoEncoderH264 *container) {
        mContainer = container;
    }
    virtual ~ConvertEncodeVideoRunnable() {
        mContainer = NULL;
    }
protected:
    void onRun() {
        mContainer->ConvertVideoHandle();
    }
    
private:
    VideoEncoderH264 *mContainer;
};
    
// 编码线程
class EncodeVideoRunnable : public KRunnable {
public:
    EncodeVideoRunnable(VideoEncoderH264 *container) {
        mContainer = container;
    }
    virtual ~EncodeVideoRunnable() {
        mContainer = NULL;
    }
protected:
    void onRun() {
        mContainer->EncodeVideoHandle();
    }
    
private:
    VideoEncoderH264 *mContainer;
};
    
VideoEncoderH264::VideoEncoderH264() {
    FileLevelLog("rtmpdump", KLog::LOG_WARNING, "VideoEncoderH264::VideoEncoderH264( this : %p )", this);
    
    mpCallback = NULL;
    mWidth = 480;
    mHeight = 640;
    mBitRate = 1000000;
    mKeyFrameInterval = 20;
    mFPS = 20;
    
    mbRunning = false;
    
    mCodec = NULL;
    mContext = NULL;
    
    mSrcFormat = AV_PIX_FMT_BGRA;
    mPts = 0;
    
    mVideoFormatConverter.SetDstFormat(VIDEO_FORMATE_YUV420P);
    
    mpConvertEncodeVideoRunnable = new ConvertEncodeVideoRunnable(this);
    mpEncodeVideoRunnable = new EncodeVideoRunnable(this);
    
}

VideoEncoderH264::~VideoEncoderH264() {
    FileLevelLog("rtmpdump", KLog::LOG_WARNING, "VideoEncoderH264::~VideoEncoderH264( this : %p )", this);
    
    Stop();
    
    if( mpConvertEncodeVideoRunnable ) {
        delete mpConvertEncodeVideoRunnable;
    }
    
    if( mpEncodeVideoRunnable ) {
        delete mpEncodeVideoRunnable;
    }
}

bool VideoEncoderH264::SetSrcFormat(VIDEO_FORMATE_TYPE type) {
    FileLevelLog("rtmpdump", KLog::LOG_WARNING, "VideoEncoderH264::SetSrcFormat( this : %p, type : %d )", this, type);
    
    bool result = false;
    switch (type) {
        case VIDEO_FORMATE_BGRA: {
            mSrcFormat = AV_PIX_FMT_BGRA;
            result = true;
        }break;
        case VIDEO_FORMATE_RGB565: {
            mSrcFormat = AV_PIX_FMT_RGB565;
            result = true;
        }break;
        case VIDEO_FORMATE_RGB24: {
            mSrcFormat = AV_PIX_FMT_RGB24;
            result = true;
        }break;
        case VIDEO_FORMATE_YUV420P: {
            mSrcFormat = AV_PIX_FMT_YUV420P;
            result = true;
        }break;
        case VIDEO_FORMATE_NV21: {
            mSrcFormat = AV_PIX_FMT_NV21;
            result = true;
        }
        default:
            break;
    }
    return result;
}
    
bool VideoEncoderH264::Create(VideoEncoderCallback* callback, int width, int height, int bitRate, int keyFrameInterval, int fps) {
    bool bFlag = true;
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_WARNING,
                 "VideoEncoderH264::Create( "
                 "this : %p, "
                 "width : %d, "
                 "height : %d, "
                 "bitRate : %d, "
                 "keyFrameInterval : %d, "
                 "fps : %d "
                 ")",
                 this,
                 width,
                 height,
                 bitRate,
                 keyFrameInterval,
                 fps
                 );
    
    mpCallback = callback;
    
    mWidth = width;
    mHeight = height;
    mBitRate = bitRate;
    mKeyFrameInterval = keyFrameInterval;
    mFPS = fps;
    
    mRuningMutex.lock();
    if( mbRunning ) {
        Stop();
    }
    
    // 创建解码器
    bFlag = CreateContext();
    if( bFlag ) {
        mbRunning = true;
        mPts = 0;
        
        VideoFrame* frame = NULL;
        mFreeBufferList.lock();
        for(int i = 0; i < DEFAULT_VIDEO_BUFFER_COUNT; i++) {
            frame = new VideoFrame();
            frame->RenewBufferSize(DEFAULT_VIDEO_BUFFER_SIZE);
            mFreeBufferList.push_back(frame);
        }
        mFreeBufferList.unlock();
        
        // 开启重采样线程
        mConvertVideoThread.Start(mpConvertEncodeVideoRunnable);
        
        // 开启解码线程
        mEncodeVideoThread.Start(mpEncodeVideoRunnable);
    }
    
    mRuningMutex.unlock();
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_WARNING,
                 "VideoEncoderH264::Create( "
                 "[Finish], "
                 "this : %p, "
                 "bFlag : %s "
                 ")",
                 this,
                 bFlag?"true":"false"
                 );
    
    return bFlag;
}

bool VideoEncoderH264::Reset() {
    bool bFlag = true;
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_WARNING,
                 "VideoEncoderH264::Reset( "
                 "this : %p, "
                 "bFlag : %s "
                 ")",
                 this,
                 bFlag?"true":"false"
                 );
    
    return bFlag;
}

void VideoEncoderH264::Pause() {
    FileLevelLog("rtmpdump", KLog::LOG_WARNING, "VideoEncoderH264::Pause( this : %p )", this);
    
}

void VideoEncoderH264::EncodeVideoFrame(void* data, int size, void* frame) {
    mFreeBufferList.lock();
    VideoFrame* srcFrame = NULL;
    if( !mFreeBufferList.empty() ) {
        srcFrame = (VideoFrame *)mFreeBufferList.front();
        mFreeBufferList.pop_front();
        
    } else {
        srcFrame = new VideoFrame();
    }
    mFreeBufferList.unlock();
    
    srcFrame->SetBuffer((unsigned char *)data, size);
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                 "VideoEncoderH264::EncodeVideoFrame( "
                 "this : %p, "
                 "srcFrame : %p, "
                 "frame : %p, "
                 "mFreeBufferList.size() : %d "
                 ")",
                 this,
                 srcFrame,
                 frame,
                 mFreeBufferList.size()
                 );
    
    // 放进采样转换队列
    mConvertBufferList.lock();
    mConvertBufferList.push_back(srcFrame);
    mConvertBufferList.unlock();
}
    
void VideoEncoderH264::Stop() {
    FileLevelLog("rtmpdump",
                 KLog::LOG_WARNING,
                 "VideoEncoderH264::Stop( "
                 "this : %p "
                 ")",
                 this
                 );
    
    mRuningMutex.lock();
    if( mbRunning ) {
        mbRunning = false;
        
        // 停止重采样线程
        mConvertVideoThread.Stop();
        
        // 停止编码线程
        mEncodeVideoThread.Stop();
        
        VideoFrame* frame = NULL;
        // 释放未重采样的视频帧
        mConvertBufferList.lock();
        while( !mConvertBufferList.empty() ) {
            frame = (VideoFrame* )mConvertBufferList.front();
            mConvertBufferList.pop_front();
            if( frame != NULL ) {
                ReleaseVideoFrame(frame);
            } else {
                break;
            }
        }
        mConvertBufferList.unlock();
        
        // 释放未编码Buffer
        mEncodeBufferList.lock();
        while( !mEncodeBufferList.empty() ) {
            frame = (VideoFrame* )mEncodeBufferList.front();
            mEncodeBufferList.pop_front();
            if( frame != NULL ) {
                ReleaseVideoFrame(frame);
            } else {
                break;
            }
        }
        mEncodeBufferList.unlock();
        
        // 释放空闲Buffer
        mFreeBufferList.lock();
        while( !mFreeBufferList.empty() ) {
            frame = (VideoFrame* )mFreeBufferList.front();
            mFreeBufferList.pop_front();
            if( frame != NULL ) {
                delete frame;
            } else {
                break;
            }
        }
        mFreeBufferList.unlock();
        
        // 销毁旧的解码器
        DestroyContext();
        
    }
    mRuningMutex.unlock();
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_WARNING,
                 "VideoEncoderH264::Stop( "
                 "[Success], "
                 "this : %p "
                 ")",
                 this
                 );
}

void VideoEncoderH264::ReleaseVideoFrame(VideoFrame* videoFrame) {
    FileLevelLog("rtmpdump",
                 KLog::LOG_STAT,
                 "VideoEncoderH264::ReleaseVideoFrame( "
                 "this : %p, "
                 "videoFrame : %p, "
                 "mFreeBufferList.size() : %d "
                 ")",
                 this,
                 videoFrame,
                 mFreeBufferList.size()
                 );
    
    mFreeBufferList.lock();
    mFreeBufferList.push_back(videoFrame);
    mFreeBufferList.unlock();
}

bool VideoEncoderH264::ConvertVideoFrame(VideoFrame* srcFrame, VideoFrame* dstFrame) {
    bool bFlag = false;
    
    // 采样帧
    AVPixelFormat srcFormat = (AVPixelFormat)mSrcFormat;
    AVFrame* pictureFrame = srcFrame->mpAVFrame;
    pictureFrame->format = srcFormat;
    pictureFrame->width  = mContext->width;
    pictureFrame->height = mContext->height;
    avpicture_fill(
                   (AVPicture *)pictureFrame,
                   (uint8_t *)srcFrame->GetBuffer(),
                   srcFormat,
                   mContext->width,
                   mContext->height
                   );
    
    // 转换采样帧格式
    bFlag = mVideoFormatConverter.ConvertEncodeFrame(srcFrame, dstFrame);
    AVFrame* yuvFrame = dstFrame->mpAVFrame;
    yuvFrame->format = mContext->pix_fmt;
    yuvFrame->width  = mContext->width;
    yuvFrame->height = mContext->height;
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                 "VideoEncoderH264::ConvertVideoFrame( "
                 "this : %p, "
                 "srcFrame : %p, "
                 "dstFrame : %p "
                 ")",
                 this,
                 srcFrame,
                 dstFrame
                 );
    
    return bFlag;
}

bool VideoEncoderH264::EncodeVideoFrame(VideoFrame* srcFrame, VideoFrame* dstFrame) {
    bool bFlag = false;

    long long curTime = getCurrentTime();
    
    // 编码帧
    AVPacket pkt = {0};
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    
    // 更新timestamp
    AVFrame* yuvFrame = srcFrame->mpAVFrame;
    yuvFrame->pts = mPts++;
    dstFrame->mTimestamp = (unsigned int)(1000.0 * yuvFrame->pts / mContext->time_base.den);
    
    // 编码帧
    int bGotPicture = 0;
    int ret = avcodec_encode_video2(mContext, &pkt, yuvFrame, &bGotPicture);
    
    // 计算处理时间
    long long now = getCurrentTime();
    long long handleTime = now - curTime;
    FileLevelLog(
                 "rtmpdump",
                 KLog::LOG_MSG,
                 "VideoEncoderH264::EncodeVideoFrame( "
                 "[Encode Frame], "
                 "this : %p, "
                 "srcFrame : %p, "
                 "dstFrame : %p, "
                 "timestamp : %u, "
                 "handleTime : %lld "
                 ")",
                 this,
                 srcFrame,
                 dstFrame,
                 dstFrame->mTimestamp,
                 handleTime
                 );
    
    if ( ret >= 0 && bGotPicture) {
        // 填充数据
        dstFrame->SetBuffer(pkt.data, pkt.size);

        FileLevelLog(
                     "rtmpdump",
                     KLog::LOG_MSG,
                     "VideoEncoderH264::EncodeVideoFrame( "
                     "[Got Encode Frame], "
                     "this : %p, "
                     "srcFrame : %p, "
                     "dstFrame : %p, "
                     "timestamp : %u, "
                     "frameSize : %d "
                     ")",
                     this,
                     srcFrame,
                     dstFrame,
                     dstFrame->mTimestamp,
                     pkt.size
                     );
        
        av_free_packet(&pkt);
        
        bFlag = true;
    }
    
    return bFlag;
}
    
bool VideoEncoderH264::CreateContext() {
    FileLevelLog("rtmpdump", KLog::LOG_MSG, "VideoEncoderH264::CreateContext( this : %p, avcodec_version : %d )", this, avcodec_version());
    
    bool bFlag = true;
    avcodec_register_all();
    av_log_set_level(AV_LOG_ERROR);
    
    mCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if ( !mCodec ) {
        FileLevelLog("rtmpdump",
                     KLog::LOG_ERR_SYS,
                     "VideoDecoderH264::CreateContext( "
                    "[Codec not found], "
                    "this : %p "
                    ")",
                    this
                    );
        
        bFlag = false;
    }
    
    if( bFlag ) {
        mContext = avcodec_alloc_context3(mCodec);
        /**
         防止编码延迟修改参数
         调节编码速度和质量的平衡(ultrafast[fastest encoding]/veryslow[best compression])
         
         @description https://trac.ffmpeg.org/wiki/Encode/H.264
         
         iOS Compatability (​source)
         Profile	Level	Devices	Options
         Baseline	3.0	All devices	-profile:v baseline -level 3.0
         Baseline	3.1	iPhone 3G and later, iPod touch 2nd generation and later	-profile:v baseline -level 3.1
         Main	3.1	iPad (all versions), Apple TV 2 and later, iPhone 4 and later	-profile:v main -level 3.1
         Main	4.0	Apple TV 3 and later, iPad 2 and later, iPhone 4s and later	-profile:v main -level 4.0
         High	4.0	Apple TV 3 and later, iPad 2 and later, iPhone 4s and later	-profile:v high -level 4.0
         High	4.1	iPad 2 and later, iPhone 4s and later, iPhone 5c and later	-profile:v high -level 4.1
         High	4.2	iPad Air and later, iPhone 5s and later	-profile:v high -level 4.2
         
         **/
        av_opt_set(mContext->priv_data, "preset", "ultrafast", 0);
        av_opt_set(mContext->priv_data, "tune", "zerolatency", 0);
        av_opt_set(mContext->priv_data, "profile:v", "baseline", 0);
        av_opt_set(mContext->priv_data, "level", "3.0", 0);
        
        mContext->bit_rate = mBitRate;
        mContext->width = mWidth;
        mContext->height = mHeight;
        mContext->time_base = (AVRational){1, mFPS};
        mContext->gop_size = mKeyFrameInterval;
        mContext->max_b_frames = 0;
        AVPixelFormat srcFormat = AV_PIX_FMT_YUV420P;
        mContext->pix_fmt = srcFormat;
        
        AVDictionary* options = NULL;
        int ret = avcodec_open2(mContext, mCodec, &options);
        if ( ret < 0 ) {
            FileLevelLog("rtmpdump",
                        KLog::LOG_ERR_SYS,
                        "VideoEncoderH264::CreateContext( "
                        "[Could not open codec], "
                        "this : %p, "
                        "ret : %d "
                        ")",
                        this,
                        ret
                        );
            mContext = NULL;
            bFlag = false;
            
        } else {
            FileLog("rtmpdump",
                    "VideoEncoderH264::CreateContext( "
                    "[Codec opened], "
                    "this : %p "
                    ")",
                    this
                    );
        }
    }
    
    if( !bFlag ) {
        DestroyContext();
        
        FileLevelLog("rtmpdump",
                     KLog::LOG_ERR_SYS,
                     "VideoEncoderH264::CreateContext( "
                     "[Fail], "
                     "this : %p "
                     ")",
                     this
                     );
    }
    
    return bFlag;
}

void VideoEncoderH264::DestroyContext() {
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                 "VideoEncoderH264::DestroyContext( "
                 "this : %p "
                 ")",
                 this
                 );
    
    if( mContext ) {
        avcodec_close(mContext);
        mContext = NULL;
    }
    
    mCodec = NULL;
}



char* VideoEncoderH264::FindNalu(char* start, int size) {
    static const char naluStartCode[] = {0x00, 0x00, 0x00, 0x01};
    static const char sliceStartCode[] = {0x00, 0x00, 0x01};
    char* nalu = NULL;
    char frameType = *start & 0x1F;
    
    for(int i = 0; i < size; i++) {
        // Only SPS or PPS need to seperate by slice start code
        if( frameType == 0x07 || frameType == 0x08 ) {
            if( i + sizeof(sliceStartCode) < size &&
               memcmp(start + i, sliceStartCode, sizeof(sliceStartCode)) == 0 ) {
                nalu = start + i + sizeof(sliceStartCode);
                break;
            }
        }
        
        if ( i + sizeof(naluStartCode) < size &&
            memcmp(start + i, naluStartCode, sizeof(naluStartCode)) == 0 ) {
            nalu = start + i + + sizeof(naluStartCode);
            break;
        }
    }
    
    return nalu;
}

void VideoEncoderH264::ConvertVideoHandle() {
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                 "VideoEncoderH264::ConvertVideoHandle( "
                 "[Start], "
                 "this : %p "
                 ")",
                 this
                 );
    
    VideoFrame* srcFrame = NULL;
    VideoFrame* dstFrame = NULL;
    
    while ( mbRunning ) {
        // 获取采样帧
        srcFrame = NULL;
        dstFrame = NULL;
        
        mConvertBufferList.lock();
        if( !mConvertBufferList.empty() ) {
            srcFrame = (VideoFrame* )mConvertBufferList.front();
            mConvertBufferList.pop_front();
            
            FileLevelLog("rtmpdump",
                         KLog::LOG_MSG,
                         "VideoEncoderH264::ConvertVideoHandle( "
                         "this : %p, "
                         "mConvertBufferList.size() : %d "
                         ")",
                         this,
                         mConvertBufferList.size()
                         );
        }
        mConvertBufferList.unlock();
        
        if( srcFrame ) {
            // 获取空闲Buffer
            mFreeBufferList.lock();
            if( !mFreeBufferList.empty() ) {
                dstFrame = (VideoFrame *)mFreeBufferList.front();
                mFreeBufferList.pop_front();
            } else {
                dstFrame = new VideoFrame();
            }
            mFreeBufferList.unlock();
            
            // 重新采样
            if( ConvertVideoFrame(srcFrame, dstFrame) ) {
                // 放到待编码队列
                mEncodeBufferList.lock();
                mEncodeBufferList.push_back(dstFrame);
                mEncodeBufferList.unlock();
                
            } else {
                // 释放空闲视频帧
                ReleaseVideoFrame(dstFrame);
            }
            
            // 释放采样视频帧
            ReleaseVideoFrame(srcFrame);
        }
        
        Sleep(1);
    }
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                "VideoEncoderH264::ConvertVideoHandle( "
                "[Exit], "
                "this : %p "
                ")",
                this
                );
}
    
void VideoEncoderH264::EncodeVideoHandle() {
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                "VideoEncoderH264::EncodeVideoHandle( "
                "[Start], "
                "this : %p "
                ")",
                this
                );
    
    VideoFrame tmpFrame;
    
    VideoFrame* srcFrame = NULL;
    VideoFrame* dstFrame = &tmpFrame;
    
    while ( mbRunning ) {
        // 获取编码帧
        srcFrame = NULL;
        
        mEncodeBufferList.lock();
        if( !mEncodeBufferList.empty() ) {
            srcFrame = (VideoFrame* )mEncodeBufferList.front();
            mEncodeBufferList.pop_front();
            
            FileLevelLog("rtmpdump",
                         KLog::LOG_MSG,
                         "VideoEncoderH264::EncodeVideoHandle( "
                         "this : %p, "
                         "mEncodeBufferList.size() : %d "
                         ")",
                         this,
                         mEncodeBufferList.size()
                         );
        }
        mEncodeBufferList.unlock();
        
        if( srcFrame ) {
            // 编码帧
            if( EncodeVideoFrame(srcFrame, dstFrame) ) {
                // 分离Nalu
                bool bFinish = false;
                char* buffer = (char *)dstFrame->GetBuffer();
                int lastSize = dstFrame->mBufferLen;
                int naluSize = 0;
                
                char* naluStart = NULL;
                char* naluEnd = NULL;
                
                // 找到第一帧
                naluStart = FindNalu(buffer, dstFrame->mBufferLen);
                if( naluStart ) {
                    lastSize -= (int)(naluStart - buffer);
                }
                
                while( naluStart ) {
                    naluSize = 0;
                    
                    // 寻找Nalu
                    naluEnd = FindNalu(naluStart, lastSize);
                    if( naluEnd != NULL ) {
                        // 找到Nalu
                        naluSize = (int)(naluEnd - naluStart);
                        
                    } else {
                        // 最后一个Nalu
                        naluSize = lastSize;
                        
                        bFinish = true;
                    }
                    
                    FileLevelLog("rtmpdump",
                                 KLog::LOG_MSG,
                                 "VideoEncoderH264::EncodeVideoHandle( "
                                 "[Got Nalu], "
                                 "this : %p, "
                                 "frameType : 0x%x, "
                                 "naluSize : %d, "
                                 "lastSize : %d, "
                                 "bFinish : %s "
                                 ")",
                                 this,
                                 (*naluStart & 0x1f),
                                 naluSize,
                                 lastSize,
                                 bFinish?"true":"false"
                                 );
                    
                    // 回调编码成功
                    if (NULL != mpCallback) {
                        mpCallback->OnEncodeVideoFrame(this, naluStart, naluSize, dstFrame->mTimestamp);
                    }
                    
                    // 数据下标偏移
                    naluStart = naluEnd;
                    lastSize -= naluSize;
                    
                    if( bFinish ) {
                        break;
                    }
                    
                }
            }
            
            // 释放待编码视频帧
            ReleaseVideoFrame(srcFrame);

        }
        
        Sleep(1);
    }
    
    FileLevelLog("rtmpdump",
                 KLog::LOG_MSG,
                "VideoEncoderH264::EncodeVideoHandle( "
                "[Exit], "
                "this : %p "
                ")",
                this
                );
}
}
