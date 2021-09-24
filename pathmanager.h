#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include <QString>

class PathManager
{
public:
    PathManager();
    QString datesFilePath(); // путь к файлу государственных и других общих праздников
    QString eventsFilePath(); // путь к файлу дней рождения и других личных праздников
    bool ok(); // обработка ошибок
    QString errString();
private:
    const QString datesFileName = "dates.txt";
    const QString eventsFileName = "events.txt";
    bool _ok;
    QString _errString;
    QString configPath, _datesFilePath, _eventsFilePath;
};

#endif // PATHMANAGER_H
