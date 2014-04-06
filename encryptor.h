#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <QString>
#include <QDir>
#include <QObject>
#include "cryptopp/dll.h"

using namespace CryptoPP;
using namespace std;

class Encryptor : public QObject
{
    Q_OBJECT

public:
    enum EnAlgorithm
    {
        AES_256,
        RSA
    };

private:
    static const int HEADERSIZE = 48;
    static const int BLOCKSIZE = 1024;
    EnAlgorithm algorithm;
    QStringList filelist;
    QDir* keyPath, *outPath;
    bool usingGzip, move;
    bool filenameEncryption;

public:
    Encryptor(EnAlgorithm algo, const QString& path, bool gzip = true, bool nameenc = false, bool move = false)
    {
        keyPath = new QDir;
        outPath = new QDir;
        setAlgorithm(algo);
        setGzip(gzip);
        setFilenameEncryption(nameenc);
        setKeyPath(path);
        setMove(move);
    }

    ~Encryptor()
    {
        delete keyPath;
        delete outPath;
    }

    void addFiles (const QStringList &filename) {filelist.append(filename);}
    void clearFiles () {filelist.clear();}

signals:
    void updateCurrentProgress(QString stat);
    void updateAllProgress(QString stat);
    void updateCurrentFile(QString file);
    void updateStatus(const QString& stat);
    void finished();

public slots:
    void setOutputPath(const QString& path) {outPath->setPath(QDir::fromNativeSeparators(path));}
    void setAlgorithm (EnAlgorithm algo) {algorithm = algo;}
    void setGzip (bool gzip) {usingGzip = gzip;}
    void setFilenameEncryption (bool nameenc) {filenameEncryption = nameenc;}
    void setKeyPath(const QString& path) {keyPath->setPath(QDir::fromNativeSeparators(path));}
    void setMove(bool stat) {move = stat;}
    void encrypt();

};

#endif // ENCRYPTOR_H
