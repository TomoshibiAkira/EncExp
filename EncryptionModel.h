#ifndef ENCRYPTIONMODEL_H
#define ENCRYPTIONMODEL_H

#include <QFileSystemModel>
#include <QString>
#include <fstream>

using namespace std;

class EncryptionModel : public QFileSystemModel
{
    Q_OBJECT

private:
    static const int HEADERSIZE = 48;
    enum STATUS
    {
        ENCRYPTED,
        PLAINTEXT,
        IOERROR
    };

    STATUS detectEncrypted(const QString filename) const
    {
        char HEADSTRING[10] = "ENCRXFLE", temp[10];
        ifstream fin( filename.toLocal8Bit().data(), ios::binary );
        if (fin.good())
        {
            fin.read(temp, 8);
            for (int i = 0; i < 8; i++)
                if (temp[i] != HEADSTRING[i] - 64)
                {
                    fin.close();
                    return PLAINTEXT;
                }
            fin.close();
            return ENCRYPTED;
        }
        else return IOERROR;
    }

public:
    int columnCount(const QModelIndex& parent = QModelIndex()) const
        {
            return QFileSystemModel::columnCount()+1;
        }

    void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder )
    {
        if (column == columnCount() - 1)
            return;
        else QFileSystemModel::sort( column, order );
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if ((section == columnCount() - 1) && (role == Qt::DisplayRole)) {
            return "Encrypted";
        } else {
            return QFileSystemModel::headerData(section,orientation,role);
        }
    }

    QVariant data(const QModelIndex& index,int role) const
        {
           if(!index.isValid()){return QFileSystemModel::data(index,role);}
           if(index.column()==columnCount()-1)
           {
               switch(role)
               {
                  case(Qt::DisplayRole):
                  {
                   if (!isDir(index))
                   {
                    STATUS ok = detectEncrypted(filePath(index));
                    if (ok == ENCRYPTED)
                       return QString("Yes");
                    else if (ok == PLAINTEXT)
                       return QString("No");
                    else
                       return QString("Error");
                   }
                   else return QString("");
                  }
                  case(Qt::TextAlignmentRole):
                      {return Qt::AlignLeft;}
                  default:{}
               }
           }
           return QFileSystemModel::data(index,role);
       }
};

#endif // ENCRYPTIONMODEL_H
