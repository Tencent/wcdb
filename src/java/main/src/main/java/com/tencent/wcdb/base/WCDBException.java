// Created by qiuwenchen on 2023/3/30.
//

/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.tencent.wcdb.base;

import java.util.HashMap;
import java.util.Map;

public class WCDBException extends RuntimeException{
    public enum Level{
        Ignore,
        Debug,
        Warning,
        Notice,
        Error,
        Fatal,
        Unknown;
        public static Level valueOf(int value) {
            switch (value) {
                case 1:
                    return Ignore;
                case 2:
                    return Debug;
                case 3:
                    return Warning;
                case 4:
                    return Notice;
                case 5:
                    return Error;
                case 6:
                    return Fatal;
            }
            return Unknown;
        }
        public String toString() {
            switch (this) {
                case Ignore:
                    return "IGNORE";
                case Debug:
                    return "DEBUG";
                case Warning:
                    return "WARNING";
                case Notice:
                    return "NOTICE";
                case Error:
                    return "ERROR";
                case Fatal:
                    return "FATAL";
                default:
                    return "UNKNOWN";
            }
        }
    }
    public enum Code {
        OK(0),
        Error(1),
        Internal(2),
        Permission(3),
        Abort(4),
        Busy(5),
        Locked(6),
        NoMemory(7),
        Readonly(8),
        Interrupt(9),
        IOError(10),
        Corrupt(11),
        NotFound(12),
        Full(13),
        CantOpen(14),
        Protocol(15),
        Empty(16),
        Schema(17),
        Exceed(18),
        Constraint(19),
        Mismatch(20),
        Misuse(21),
        NoLargeFileSupport(22),
        Authorization(23),
        Format(24),
        Range(25),
        NotADatabase(26),
        Notice(27),
        Warning(28),
        Row(100),
        Done(101),
        Unknown(-1);
        private int value = 0;

        private Code(int value) {
            this.value = value;
        }

        public static Code valueOf(int value) {
            switch (value) {
                case 0:
                    return OK;
                case 1:
                    return Error;
                case 2:
                    return Internal;
                case 3:
                    return Permission;
                case 4:
                    return Abort;
                case 5:
                    return Busy;
                case 6:
                    return Locked;
                case 7:
                    return NoMemory;
                case 8:
                    return Readonly;
                case 9:
                    return Interrupt;
                case 10:
                    return IOError;
                case 11:
                    return Corrupt;
                case 12:
                    return NotFound;
                case 13:
                    return Full;
                case 14:
                    return CantOpen;
                case 15:
                    return Protocol;
                case 16:
                    return Empty;
                case 17:
                    return Schema;
                case 18:
                    return Exceed;
                case 19:
                    return Constraint;
                case 20:
                    return Mismatch;
                case 21:
                    return Misuse;
                case 22:
                    return NoLargeFileSupport;
                case 23:
                    return Authorization;
                case 24:
                    return Format;
                case 25:
                    return Range;
                case 26:
                    return NotADatabase;
                case 27:
                    return Notice;
                case 28:
                    return Warning;
                case 100:
                    return Row;
                case 101:
                    return Done;
                default:
                    return Unknown;
            }
        }
        public int value() {
            return this.value;
        }
    }

    public enum ExtendCode {
        ErrorMissingCollseq(257),       // Code.Error | (1 << 8)
        ErrorRetry(513),                // Code.Error | (2 << 8)
        ErrorSnapshot(769),             // Code.Error | (3 << 8)
        IOErrorRead(266),               // Code.IOError | (1 << 8)
        IOErrorShortRead(522),          // Code.IOError | (2 << 8)
        IOErrorWrite(778),              // Code.IOError | (3 << 8)
        IOErrorFsync(1034),             // Code.IOError | (4 << 8)
        IOErrorDirFsync(1290),          // Code.IOError | (5 << 8)
        IOErrorTruncate(1546),          // Code.IOError | (6 << 8)
        IOErrorFstat(1802),             // Code.IOError | (7 << 8)
        IOErrorUnlock(2058),            // Code.IOError | (8 << 8)
        IOErrorRdlock(2314),            // Code.IOError | (9 << 8)
        IOErrorDelete(2570),            // Code.IOError | (10 << 8)
        IOErrorBlocked(2826),           // Code.IOError | (11 << 8)
        IOErrorNoMemory(3082),          // Code.IOError | (12 << 8)
        IOErrorAccess(3338),            // Code.IOError | (13 << 8)
        IOErrorCheckReservedLock(3594), // Code.IOError | (14 << 8)
        IOErrorLock(3850),              // Code.IOError | (15 << 8)
        IOErrorClose(4106),             // Code.IOError | (16 << 8)
        IOErrorDirClose(4362),          // Code.IOError | (17 << 8)
        IOErrorShmOpen(4618),           // Code.IOError | (18 << 8)
        IOErrorShmSize(4874),           // Code.IOError | (19 << 8)
        IOErrorShmLock(5130),           // Code.IOError | (20 << 8)
        IOErrorShmMap(5386),            // Code.IOError | (21 << 8)
        IOErrorSeek(5642),              // Code.IOError | (22 << 8)
        IOErrorDeleteNoEntry(5898),     // Code.IOError | (23 << 8)
        IOErrorMmap(6154),              // Code.IOError | (24 << 8)
        IOErrorGetTempPath(6410),       // Code.IOError | (25 << 8)
        IOErrorConvPath(6666),          // Code.IOError | (26 << 8)
        IOErrorVnode(6922),             // Code.IOError | (27 << 8)
        IOErrorAuthorization(7178),     // Code.IOError | (28 << 8)
        IOErrorBeginAtomic(7434),       // Code.IOError | (29 << 8)
        IOErrorCommitAtomic(7690),      // Code.IOError | (30 << 8)
        IOErrorRollbackAtomic(7946),    // Code.IOError | (31 << 8)
        LockedSharedCache(262),         // Code.Locked | (1 << 8)
        LockedVirtualTable(518),        // Code.Locked | (2 << 8)
        BusyRecovery(261),              // Code.Busy | (1 << 8)
        BusySnapshot(517),              // Code.Busy | (2 << 8)
        CantOpenNoTempDir(270),         // Code.CantOpen | (1 << 8)
        CantOpenIsDir(526),             // Code.CantOpen | (2 << 8)
        CantOpenFullPath(782),          // Code.CantOpen | (3 << 8)
        CantOpenConvPath(1038),         // Code.CantOpen | (4 << 8)
        CantOpenDirtyWal(1294),         // Code.CantOpen | (5 << 8)
        CorruptVirtualTable(267),       // Code.Corrupt | (1 << 8)
        CorruptSequence(523),           // Code.Corrupt | (2 << 8)
        ReadonlyRecovery(264),          // Code.Readonly | (1 << 8)
        ReadonlyCantLock(520),          // Code.Readonly | (2 << 8)
        ReadonlyRollback(776),          // Code.Readonly | (3 << 8)
        ReadonlyDatabaseMoved(1032),    // Code.Readonly | (4 << 8)
        ReadonlyCantInit(1288),         // Code.Readonly | (5 << 8)
        ReadonlyDirectory(1544),        // Code.Readonly | (6 << 8)
        AbortRollback(516),             // Code.Abort | (2 << 8)
        ConstraintCheck(275),           // Code.Constraint | (1 << 8)
        ConstraintCommitHook(531),      // Code.Constraint | (2 << 8)
        ConstraintForeignKey(787),      // Code.Constraint | (3 << 8)
        ConstraintFunction(1043),       // Code.Constraint | (4 << 8)
        ConstraintNotNull(1299),        // Code.Constraint | (5 << 8)
        ConstraintPrimaryKey(1555),     // Code.Constraint | (6 << 8)
        ConstraintTrigger(1811),        // Code.Constraint | (7 << 8)
        ConstraintUnique(2067),         // Code.Constraint | (8 << 8)
        ConstraintVirtualTable(2323),   // Code.Constraint | (9 << 8)
        ConstraintRowID(2579),          // Code.Constraint | (10 << 8)
        NoticeRecoverWal(283),          // Code.Notice | (1 << 8)
        NoticeRecoverRollback(539),     // Code.Notice | (2 << 8)
        WarningAutoIndex(284),          // Code.Warning | (2 << 8)
        AuthorizationUser(279),         // Code.Authorization | (2 << 8)
        OKLoadPermanently(256),         // Code.OK | (2 << 8)
        Unknown(-1);
        private int value = 0;

        private ExtendCode(int value) {
            this.value = value;
        }

        public static ExtendCode valueOf(int value) {
            switch (value) {
                case 257:
                    return ErrorMissingCollseq;
                case 513:
                    return ErrorRetry;
                case 769:
                    return ErrorSnapshot;
                case 266:
                    return IOErrorRead;
                case 522:
                    return IOErrorShortRead;
                case 778:
                    return IOErrorWrite;
                case 1034:
                    return IOErrorFsync;
                case 1290:
                    return IOErrorDirFsync;
                case 1546:
                    return IOErrorTruncate;
                case 1802:
                    return IOErrorFstat;
                case 2058:
                    return IOErrorUnlock;
                case 2314:
                    return IOErrorRdlock;
                case 2570:
                    return IOErrorDelete;
                case 2826:
                    return IOErrorBlocked;
                case 3082:
                    return IOErrorNoMemory;
                case 3338:
                    return IOErrorAccess;
                case 3594:
                    return IOErrorCheckReservedLock;
                case 3850:
                    return IOErrorLock;
                case 4106:
                    return IOErrorClose;
                case 4362:
                    return IOErrorDirClose;
                case 4618:
                    return IOErrorShmOpen;
                case 4874:
                    return IOErrorShmSize;
                case 5130:
                    return IOErrorShmLock;
                case 5386:
                    return IOErrorShmMap;
                case 5642:
                    return IOErrorSeek;
                case 5898:
                    return IOErrorDeleteNoEntry;
                case 6154:
                    return IOErrorMmap;
                case 6410:
                    return IOErrorGetTempPath;
                case 6666:
                    return IOErrorConvPath;
                case 6922:
                    return IOErrorVnode;
                case 7178:
                    return IOErrorAuthorization;
                case 7434:
                    return IOErrorBeginAtomic;
                case 7690:
                    return IOErrorCommitAtomic;
                case 7946:
                    return IOErrorRollbackAtomic;
                case 262:
                    return LockedSharedCache;
                case 518:
                    return LockedVirtualTable;
                case 261:
                    return BusyRecovery;
                case 517:
                    return BusySnapshot;
                case 270:
                    return CantOpenNoTempDir;
                case 526:
                    return CantOpenIsDir;
                case 782:
                    return CantOpenFullPath;
                case 1038:
                    return CantOpenConvPath;
                case 1294:
                    return CantOpenDirtyWal;
                case 267:
                    return CorruptVirtualTable;
                case 523:
                    return CorruptSequence;
                case 264:
                    return ReadonlyRecovery;
                case 520:
                    return ReadonlyCantLock;
                case 776:
                    return ReadonlyRollback;
                case 1032:
                    return ReadonlyDatabaseMoved;
                case 1288:
                    return ReadonlyCantInit;
                case 1544:
                    return ReadonlyDirectory;
                case 516:
                    return AbortRollback;
                case 275:
                    return ConstraintCheck;
                case 531:
                    return ConstraintCommitHook;
                case 787:
                    return ConstraintForeignKey;
                case 1043:
                    return ConstraintFunction;
                case 1299:
                    return ConstraintNotNull;
                case 1555:
                    return ConstraintPrimaryKey;
                case 1811:
                    return ConstraintTrigger;
                case 2067:
                    return ConstraintUnique;
                case 2323:
                    return ConstraintVirtualTable;
                case 2579:
                    return ConstraintRowID;
                case 283:
                    return NoticeRecoverWal;
                case 539:
                    return NoticeRecoverRollback;
                case 284:
                    return WarningAutoIndex;
                case 279:
                    return AuthorizationUser;
                case 256:
                    return OKLoadPermanently;
                default:
                    return Unknown;
            }
        }
        public int value() {
            return this.value;
        }
    }

    public enum Key {
        tag("Tag"),
        path("Path"),
        type("Type"),
        source("Source"),
        SQL("SQL"),
        extendedCode("ExtCode"),
        message("Message");

        private String value;
        private Key(String value) {
            this.value = value;
        }
        String getValue() {
            return value;
        }
    }

    public Level level;
    public Code code;
    public HashMap<String, Object> infos;

    public WCDBException(){
    }

    public WCDBException(long cppError) {
        level = Level.valueOf(getLevel(cppError));
        code = Code.valueOf(getCode(cppError));
        String message = getMessage(cppError);
        infos = new HashMap<>();
        infos.put(Key.message.value, message);
        enumerateInfo(cppError);
    }

    private native int getLevel(long error);
    private native int getCode(long error);
    private native String getMessage(long error);

    private native void enumerateInfo(long cppError);

    private void addInfo(String key, int valueType, long intValue, double doubleValue, String stringValue) {
        if(valueType == 1) {
            infos.put(key, intValue);
        } else if (valueType == 2) {
            infos.put(key, doubleValue);
        } else {
            infos.put(key, stringValue);
        }
    }

    public long tag() {
        Object tag = infos.get(Key.tag.getValue());
        if(tag != null) {
            return (Long)tag;
        }
        return 0;
    }

    public ExtendCode extendCode() {
        Object code = infos.get(Key.extendedCode.getValue());
        if(code != null) {
            return ExtendCode.valueOf(((Long)code).intValue());
        }
        return ExtendCode.Unknown;
    }

    public String message() {
        return (String) infos.get(Key.message.getValue());
    }

    public String sql() {
        return (String) infos.get(Key.SQL.getValue());
    }

    public String path() {
        return (String) infos.get(Key.path.getValue());
    }

    public String toString() {
        StringBuilder des = new StringBuilder("[WCDB] [" + level + "] Code: " + code.value());
        for (Map.Entry<String, Object> entry : infos.entrySet()) {
            des.append(", ").append(entry.getKey()).append(": ").append(entry.getValue());
        }
        return des.toString();
    }
}
