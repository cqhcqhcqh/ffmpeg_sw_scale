#include "rescalethread.h"
#include <QDebug>
#include <QFile>
#include <QImage>

#ifdef Q_OS_MAC
#define IN_FILE "/Users/caitou/Desktop/in2.yuv"
#define OUT_FILE "/Users/caitou/Desktop/out2.yuv"
#else
#define IN_FILE "C:\\Workspaces\\in2.yuv"
#define OUT_FILE "C:\\Workspaces\\out2.yuv"
#endif

#define ERROR_BUF(res) \
    char errbuf[1024]; \
    av_strerror(res, errbuf, sizeof(errbuf));

RescaleThread::RescaleThread(QObject *parent) : QThread(parent) {
    connect(this, &QThread::finished, this, &QThread::deleteLater);
}

RescaleThread::~RescaleThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << "ResampleThread::~ResampleThread()";
}

void RescaleThread::run() {
    RescaleVideoSpec in;
    in.file = IN_FILE;
    in.fmt = AV_PIX_FMT_YUV420P;
    in.width = 1280;
    in.height = 720;
    int in_image_buffer_size = av_image_get_buffer_size(in.fmt, in.width, in.height, 1);
    qDebug() << "in_image_buffer_size" << in_image_buffer_size;

    RescaleVideoSpec out;
    out.file = OUT_FILE;
    out.fmt = AV_PIX_FMT_RGB24;
    out.width = 640;
    out.height = 480;
    int out_image_buffer_size = av_image_get_buffer_size(out.fmt, out.width, out.height, 1);
    qDebug() << "out_image_buffer_size" << out_image_buffer_size;

    SwsContext *ctx = sws_getContext(in.width, in.height, in.fmt, out.width, out.height, out.fmt, SWS_BILINEAR, nullptr, nullptr, nullptr);

    QFile inyuv(in.file);
    if (!inyuv.open(QFile::ReadOnly)) {
        qDebug() << "file open failure" << in.file;
        return;
    }

    QFile outyuv(out.file);
    if (!outyuv.open(QFile::WriteOnly)) {
        qDebug() << "file open failure" << out.file;
        return;
    }

    uint8_t *srcData[4], *dstData[4];
    int srcLineSize[4], dstLineSize[4];
    int ret = av_image_alloc(srcData, srcLineSize, in.width, in.height, in.fmt, 1);
    if (ret < 0) {
        ERROR_BUF(ret);
        return;
    }

    ret = av_image_alloc(dstData, dstLineSize, out.width, out.height, out.fmt, 1);
    if (ret < 0) {
        ERROR_BUF(ret);
        return;
    }

    while(inyuv.read((char *)srcData[0], in_image_buffer_size) > 0) {
        int height = sws_scale(ctx, srcData, srcLineSize, 0, in.height, dstData, dstLineSize);
        qDebug() << "sws_scale height" << height;
        if (height != out.height) {
            return;
        }
        outyuv.write((char *)dstData[0], out_image_buffer_size);
    }

    sws_freeContext(ctx);
    av_freep(&srcData[0]);
    av_freep(&dstData[0]);

    // ffplay -video_size 1280x720 -pixel_format yuv420p -i in2.yuv
    // ffplay -video_size 640x480 -pixel_format rgb24 -i out2.yuv
}
