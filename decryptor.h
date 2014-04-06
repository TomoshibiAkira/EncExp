#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include "cryptopp/dll.h"
#include <QString>
#include <QObject>
#include <QDir>

class Decryptor : public QObject
{
    Q_OBJECT

private:
    static const int BLOCKSIZE = 1024;
    QStringList filelist;
    QDir* keyPath, *outPath;

public:
    Decryptor(const QString& keypath)
    {
        keyPath = new QDir;
        outPath = new QDir;
        setKeyPath(keypath);
    }
    ~Decryptor()
    {
        delete keyPath;
        delete outPath;
    }

    void addFiles (const QStringList &filename) {filelist.append(filename);}
    void clearFiles () {filelist.clear();}

signals:
    void updateCurrentProgress();
    void updateAllProgress();
    void finished();

public slots:
    void setOutputPath(const QString& path) {outPath->setPath(path);}
    void setKeyPath(const QString& path) {keyPath->setPath(path);}
    void decrypt();

};

#endif // DECRYPTOR_H
