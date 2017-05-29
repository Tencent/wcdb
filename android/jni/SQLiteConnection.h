#ifndef __WCDB_SQLITE_CONNECTION_H__
#define __WCDB_SQLITE_CONNECTION_H__

#include <sqlite3.h>
#include <string>

namespace wcdb {

struct SQLiteConnection {
    // Open flags.
    // Must be kept in sync with the constants defined in SQLiteDatabase.java.
    enum {
        OPEN_READWRITE = 0x00000000,
        OPEN_READONLY = 0x00000001,
        OPEN_READ_MASK = 0x00000001,
        NO_LOCALIZED_COLLATORS = 0x00000010,
        CREATE_IF_NECESSARY = 0x10000000,
    };

    sqlite3 *const db;
    const int openFlags;
    const std::string path;
    const std::string label;

    volatile bool canceled;

    SQLiteConnection(sqlite3 *db,
                     int openFlags,
                     const std::string &path,
                     const std::string &label)
        : db(db)
        , openFlags(openFlags)
        , path(path)
        , label(label)
        , canceled(false)
    {
    }
};
}

#endif
