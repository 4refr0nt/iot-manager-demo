#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addOption({{"c", "colsize"}, "Count of hex values in one row", "colsize", "12"});
    parser.addOption({{"r", "reduce"}, "Remove all chars <= space from source text"});
    parser.addOption({{"d", "deletecomments"}, "Delete comments with sourece file contentfrom result h file"});
    parser.process(a);

    if (parser.positionalArguments().size() == 1) {
        QFile jsonFile(parser.positionalArguments().at(0));
        QString jsonFilename(QFileInfo(jsonFile).fileName().remove(QRegularExpression("(\\.[^.]+)$")));

        if (jsonFile.exists() == true) {
            if (jsonFile.open(QIODevice::ReadOnly) == true) {
                QByteArray jsonRawContent;
                QString jsonHexContent;
                jsonRawContent = jsonFile.readAll();
                jsonFile.close();
                if (parser.isSet("deletecomments") == false)
                    jsonHexContent.append(QString("/** Source file %1\n").arg(jsonFile.fileName()) + jsonRawContent + "**/\n");
                if (parser.isSet("reduce"))
                    jsonRawContent = QString(jsonRawContent).remove(QRegularExpression("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)")).toLocal8Bit();
                jsonRawContent.append(char(0x00));
                jsonHexContent.append(QString("const char %1_json[] = {\n").arg(jsonFilename));

                int colCount = 0;
                for (const unsigned char jsonChar : jsonRawContent) {
                    QString hexValue(QString::number(jsonChar, 16));
                    if (hexValue.size() == 1)
                        hexValue.push_front("0");
                    hexValue.push_front("0x");
                    hexValue.append(", ");
                    jsonHexContent.append(hexValue);
                    if (parser.value("colsize").toInt() > 0)
                        if (++colCount == parser.value("colsize").toInt()) {
                            colCount = 0;
                            jsonHexContent.append('\n');
                        }
                }
                jsonHexContent.append(QString("};\nconst int %1_len = %2;\n").arg(jsonFilename).arg(jsonRawContent.size()));

                QFile hFile(jsonFile.fileName() + ".h");
                if (hFile.open(QIODevice::WriteOnly)) {
                    hFile.write(jsonHexContent.toLocal8Bit());
                    hFile.close();
                    qDebug() << "Header file" << hFile.fileName() << "has been wrote success";
                }
                else
                    qDebug() << "Can't write data to header file" << hFile.fileName();
            }
            else
                qDebug() << "Can't open file" << jsonFilename;
        }
        else
            qDebug() << "File" << jsonFilename << "not exists";
    }
    else
        qDebug() << "Json to C++ header (c) 2017 https://www.onlims.com/\r\n"
                    "Sergey Alikin, Ricardo Timmermann\r\n"
                    "Repository: https://gitlab.com/OnLIMS-public/json2h\r\n"
                    "\r\n"
                    "Use:\r\n"
                    "json2h jsonfile.json OPTIONS\r\n"
                    "-r or --reduce\r\n\tremoves from source input all chars with code <= 0x20 to reduce header file size\r\n"
                    "-c or --colsize VALUE\r\n\tset item count on each line of result array, default is 12.\r\n"
                    "-d or --deletecomments\r\n\tDeletes source file as comment in result header file\r\n"
                    "\r\n"
                    "Examples of usage:\r\n"
                    "./json2h test.json (produce non-reduced output with 12 items on each row)\r\n"
                    "./json2h -c 15 -r test.json (produce reduced output with 15 items on each row)\r\n"
                    "./json2h test.json -r -c 0 (produce reduced output with one long line)\r\n"
                    "./json2h test.json -r -d (produce reduced output, source file did not included into result file)\r\n";

    return 0;
}
