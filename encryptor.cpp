#include "encryptor.h"
#include <cryptopp/gzip.h>
#include <fstream>
#include <QMessageBox>
#include <QTextCodec>

void Encryptor::encrypt()
{
    byte header[HEADERSIZE - SHA256::DIGESTSIZE];
    byte hash[SHA256::DIGESTSIZE];
    byte key[AES::MAX_KEYLENGTH];
    byte iv[AES::BLOCKSIZE];
    byte keyFilename[SHA256::DIGESTSIZE * 2 + 1];
    AutoSeededRandomPool prng;
    SHA256 sha;
    int counter = 0, fileSize = 0;
    string GZIP, ALGO;
    QString curprogress;
    QString allprogress;

    byte plainData[BLOCKSIZE], cipherData[BLOCKSIZE];

    static const char HEADFLAG[10] = "ENCRXFLE";

    // Construct header
    // 1. HEADFLAG: "ENCRXFLE"
    int bp = 0;
    for (int i = 0; i < 8; i++)
        header[bp + i] = HEADFLAG[i] - 64;

    // 2.GZIP OPTION
    bp = 8;
    if (usingGzip)
        GZIP = "GZIP";
    else
        GZIP = "NGZP";
    for (int i = 0; i < 4; i++)
        header[bp + i] = GZIP[i] - 64;

    // 3.Algorithm Option
    bp = 12;
    switch (algorithm)
    {
    case AES_256:
        {ALGO = "AAES"; break;}
    case RSA:
        {ALGO = "ARSA"; break;}
    }
    for (int i = 0; i< 4; i++)
        header[bp + i] = ALGO[i] - 64;

    // Start processing
    for (int i = 0; i < filelist.size(); i++)
    {
        // Initialize
        QByteArray fp = filelist[i].toLocal8Bit();
        memset (hash, 0, sizeof(hash));
        memset (key, 0, sizeof(key));
        memset (iv, 0, sizeof(iv));
        memset (keyFilename, 0, sizeof(keyFilename));
        counter = 0;
        QFileInfo info(filelist[i]);
        QByteArray orifn = info.fileName().toLocal8Bit();

        // Test file opening.
        ifstream ftest( fp.data() , ios::binary );
        if ( !ftest.good() )
        {
            QMessageBox::critical(0, "File Invalid", "File does not exist!");
            continue;
        }

        allprogress = QString("%1 / %2").arg(i+1).arg(filelist.size());

        emit updateCurrentFile(QDir::toNativeSeparators(filelist[i]));
        emit updateAllProgress(allprogress);

        // 4. HASH
        emit updateStatus("Calculating Hash...");
        FileSource HASH(fp.data(), true /* PumpAll */, new HashFilter(sha, new ArraySink(hash, sizeof(hash))));
        ArraySource GETKF(hash, sizeof(hash), true,
            new HexEncoder(new ArraySink( keyFilename, sizeof(keyFilename) ) )
        );

        emit updateStatus("Generating Key File...");
        // Generate a random key
        prng.GenerateBlock( key, AES::MAX_KEYLENGTH );
        // Generate a random IV
        prng.GenerateBlock( iv, AES::BLOCKSIZE);

        // Setting encryptor
        CFB_Mode<AES>::Encryption aes(key, AES::MAX_KEYLENGTH, iv);

        QString newKeyFile( keyPath->absolutePath() + '/' + (char*)keyFilename );
        QFileInfo keyinfo(newKeyFile);
        QByteArray newkf = newKeyFile.toLocal8Bit();

        if (keyinfo.exists()) remove (newkf.data());

        ofstream fkey(newkf.data());
        if (!fkey.fail())
        {
            fkey.write((char*) key, sizeof(key));
            fkey.write((char*) iv, sizeof(iv));
            fkey.write((char*) hash, sizeof(hash));
            fkey.write(orifn.data(), orifn.size());
            fkey.close();
        }
        else
        {
            QMessageBox::critical(0, "Writting Key File Error", "Can't write key file!");
            continue;
        }


        QString newFile = outPath->absolutePath() + '/';
        if (!filenameEncryption)
            newFile += info.fileName() + ".Encrypted";
        else
            newFile += (char*) keyFilename;

        QByteArray newfp = newFile.toLocal8Bit();

        if (usingGzip)
        {
            emit updateStatus("Compressing File...");
            char tmpName[20] = "";
            sprintf (tmpName, "%d", (int)prng.GenerateWord32());
            QString newTemp(outPath->absolutePath() + '/' + tmpName);
            QByteArray newtmp = newTemp.toLocal8Bit();
            FileSource(fp.data() , true,
                       new Gzip(new FileSink(newtmp.data(), 9)));
            fp = newtmp;
        }

        emit updateStatus("Writing Header...");

        ofstream fout( newfp.data() );
        if (!fout.fail())
        {
            fout.write((char*) header, sizeof(header));
            fout.write((char*) hash, sizeof(hash));
        }
        else
        {
            QMessageBox::critical(0, "Writting File Error", "Can't write file!");
            continue;
        }

        // Opening file
        ifstream fin( fp , ios::binary );
        fin.seekg(0, fin.end);
        fileSize = fin.tellg();
        fin.seekg(0, fin.beg);

        curprogress = QString("%1 / %2").arg(0).arg(fileSize);
        emit updateCurrentProgress(curprogress);

        emit updateStatus("Encrypting...");
        while ( !fin.eof() )
        {
            memset (plainData, 0, sizeof(plainData));
            memset (cipherData, 0, sizeof(cipherData));
            fin.read((char*) plainData, BLOCKSIZE);

            ArraySource (plainData, true, new StreamTransformationFilter(
                             aes, new ArraySink( cipherData , sizeof(cipherData) )));
            if ( fin.eof() )
            {
                fout.write((char*) cipherData, fileSize % BLOCKSIZE);
                counter = fileSize;
            }
            else
            {
                fout.write((char*) cipherData, BLOCKSIZE);
                counter += BLOCKSIZE;
            }

            if (fout.fail())
            {
                QMessageBox::critical(0, "Writting File Error", "Can't write file!");
                break;
            }
            curprogress = QString("%1 / %2").arg(counter).arg(fileSize);
            emit updateCurrentProgress(curprogress);
         }
        fout.close();
        fin.close();
        if (usingGzip) remove (fp.data());
    }

    emit finished();
}
