#include "musicwebradioobject.h"
#include "musicdatadownloadthread.h"
#include "musicextractwrap.h"

#include <QProcess>
#ifdef MUSIC_GREATER_NEW
#  include <QtConcurrent/QtConcurrent>
#else
#  include <QtConcurrentRun>
#endif

#define RADIO_NAME      "UFZUZ21KUVVCT1dlMzVsVyt1Wk9yRXdNWFBVPQ=="
#define DOWNLOAD_NAME   "aUxJYUJXYStOZ0ZORTZMdDdvU3FkZz09"
#define DOWNLOAD_URL    "NWxja1RJSEc0Nnl6eXBCUkdMaU5ucWs1eHJ4bzF4VHNoeGlvT3UwbmJCWUhVZ0c2aTR5cXRYeUdYMlZqWHlqSzMwWk8yd1d3RWtTRFQwbzQ="

MusicWebRadioObject::MusicWebRadioObject(QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(qRegisterMetaType<MusicObject::ToolsType>("MusicObject::ToolsType"));
}

void MusicWebRadioObject::raise()
{

}

void MusicWebRadioObject::show()
{
    QString fileName = PLUGINS_DIR_FULL + MusicUtils::Algorithm::mdII(DOWNLOAD_NAME, false);
    if(QFile::exists(fileName))
    {
        dataDownloadFinished();
    }
    else
    {
        MusicDataDownloadThread *download = new MusicDataDownloadThread(MusicUtils::Algorithm::mdII(DOWNLOAD_URL, false),
                                                PLUGINS_DIR_FULL + MusicUtils::Algorithm::mdII(DOWNLOAD_NAME, false),
                                                MusicNetwork::DownloadOther, this);
        connect(download, SIGNAL(downLoadDataChanged(QString)), SLOT(dataDownloadFinished()));
        download->startToDownload();
    }
}

void MusicWebRadioObject::dataDownloadFinished()
{
    QString fileName = PLUGINS_DIR_FULL + MusicUtils::Algorithm::mdII(DOWNLOAD_NAME, false);
    if(MusicExtractWrap::outputData(fileName))
    {
        QFile::remove(fileName);
        QtConcurrent::run([&]
        {
            int code = QProcess::execute(QString("%1radio/%2").arg(PLUGINS_DIR_FULL).arg(MusicUtils::Algorithm::mdII(RADIO_NAME, false)));
            if(code != -2 && code != -1)
            {
                emit resetFlag(MusicObject::TT_WebRadio);
            }
        });
    }
}
