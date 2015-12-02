#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QDir>
#include <QFileInfoList>
#include "slcmodel.h"

class SLCLoader
{
public:
    static void load (const QString &destination)
    {
        QFileInfo destinationInfo (destination);
        QElapsedTimer timer;
        if (destinationInfo.isDir ())
        {
            qDebug () << "Loading SLC(s) from directory:" << destinationInfo.absoluteFilePath ();
            QDir dir (destinationInfo.absoluteFilePath ());
            QFileInfoList infoList = dir.entryInfoList (QStringList ("*.slc"));
            timer.start ();
            for (const QFileInfo &info : infoList)
            {
                XJRP::SLCModel model;
                qDebug () << model.readFile (info.absoluteFilePath ());
                qDebug () << info.fileName ();
                // qDebug () << model.boundary ();
            }
            qDebug () << "Reading Directory Finished" << infoList.count () << "models in" << qreal (timer.nsecsElapsed ()) / 1e9 << "second(s).";
        }
        else if (destinationInfo.isFile () && destinationInfo.suffix ().toLower () == "slc")
        {
            qDebug () << "Loading SLC file:" << destinationInfo.absoluteFilePath ();
            timer.start ();
            XJRP::SLCModel model;
            qDebug () << model.readFile (destinationInfo.absoluteFilePath ());
            // qDebug () << model.boundary ();
            qDebug () << "Reading File Finished in" << qreal (timer.nsecsElapsed ()) / 1e9 << "second(s).";
        }
    }

};

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);

    int returnCode (0);
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            QString arg (argv [i]);
            SLCLoader::load (arg);
        }
    }
    else
    {
        returnCode = -1;
    }

    return returnCode;

//    QStringList filelist = {"/data/Share/RPBuild_examples/十字模型数据/120_Rescaled(2).slc", "/data/Share/RPBuild_examples/十字模型数据/s_120_Rescaled(2).slc"};
//    QStringList filelist = {"/data/Share/RPBuild_examples/model 1200/copy_1_of_s1.slc", "/data/Share/RPBuild_examples/model 1200/s_copy_1_of_s1.slc"};
//    loader.load (filelist);

//    loader.loadDirectory ("/data/Share/RPBuild_examples/model 1200/");
//    loader.loadDirectory ("/data/Share/RPBuild_examples/时代天使牙模比对/");
//    loader.load (QStringList ("/data/Share/RPBuild_examples/时代天使牙模比对/2.slc"));
}

