#include "musicdownloadqueryxmartistlistthread.h"
#///QJson import
#include "qjson/parser.h"

#define REFER_URL   "UWhHVHk0M0QwSEJ5djJjeVZmaTNqL0R3amVKTnA0anJtdUdvck9MeExYZStnaHpT"

MusicDownLoadQueryXMArtistListThread::MusicDownLoadQueryXMArtistListThread(QObject *parent)
    : MusicDownLoadQueryArtistListThread(parent)
{
    m_pageSize = 100;
    m_pageTotal = DEFAULT_LEVEL4;
    m_queryServer = "XiaMi";
}

void MusicDownLoadQueryXMArtistListThread::startToPage(int offset)
{
    if(!m_manager)
    {
        return;
    }

    M_LOGGER_INFO(QString("%1 startToPage %2").arg(getClassName()).arg(offset));
    QString catId = "class=1&type=1";
    QStringList dds = m_searchText.split(STRING_SPLITER);
    if(dds.count() == 2)
    {
        catId = dds[0];
        if(catId.isEmpty())
        {
            catId = "class=1&type=1";
        }
    }
    QUrl musicUrl = MusicUtils::Algorithm::mdII(XM_AR_LIST_URL, false).arg(catId).arg(offset).arg(m_pageSize);

    deleteAll();
    m_interrupt = true;

    QNetworkRequest request;
    request.setUrl(musicUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Referer", MusicUtils::Algorithm::mdII(REFER_URL, false).toUtf8());
    request.setRawHeader("User-Agent", MusicUtils::Algorithm::mdII(XM_UA_URL_1, ALG_UA_KEY, false).toUtf8());
    setSslConfiguration(&request);

    m_reply = m_manager->get(request);
    connect(m_reply, SIGNAL(finished()), SLOT(downLoadFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(replyError(QNetworkReply::NetworkError)));
}

void MusicDownLoadQueryXMArtistListThread::startToSearch(const QString &artistlist)
{
    m_searchText = artistlist;
    m_searchText.replace("n", "&");
    startToPage(0);
}

void MusicDownLoadQueryXMArtistListThread::downLoadFinished()
{
    if(!m_reply || !m_manager)
    {
        deleteAll();
        return;
    }

    M_LOGGER_INFO(QString("%1 downLoadFinished").arg(getClassName()));
    emit clearAllItems();
    m_musicSongInfos.clear();
    m_interrupt = false;

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = m_reply->readAll();

        QJson::Parser parser;
        bool ok;
        QVariant data = parser.parse(bytes, &ok);
        if(ok)
        {
            QVariantMap value = data.toMap();
            if(value.contains("data") && value["state"].toInt() == 0)
            {
                value = value["data"].toMap();
                QVariantList datas = value["artists"].toList();
                foreach(const QVariant &var, datas)
                {
                    if(m_interrupt) return;

                    if(var.isNull())
                    {
                        continue;
                    }

                    value = var.toMap();

                    MusicResultsItem info;
                    info.m_id = value["artist_id"].toString();
                    info.m_name = value["name"].toString();
                    emit createArtistListItem(info);
                }
            }
        }
    }

//    emit downLoadDataChanged(QString());
    deleteAll();
    M_LOGGER_INFO(QString("%1 downLoadFinished deleteAll").arg(getClassName()));
}
