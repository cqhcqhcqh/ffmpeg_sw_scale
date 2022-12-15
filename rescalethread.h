#ifndef RESCALETHREAD_H
#define RESCALETHREAD_H

#include <QThread>
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

typedef struct {
    int width;
    int height;
    AVPixelFormat fmt;
    const char *file;
} RescaleVideoSpec;

class RescaleThread : public QThread
{
    Q_OBJECT
private:
    void run() override;
public:
    RescaleThread(QObject *parent);
    ~RescaleThread();
signals:

};

#endif // RESCALETHREAD_H
