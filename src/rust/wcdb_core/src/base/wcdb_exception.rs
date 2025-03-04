use crate::utils::ToCow;
use std::collections::HashMap;
use std::ffi::{c_char, c_void};
use std::fmt::{Debug, Display, Formatter};

extern "C" {
    fn WCDBRustError_getLevel(cpp_obj: *mut c_void) -> i32;
    fn WCDBRustError_getCode(cpp_obj: *mut c_void) -> i32;
    fn WCDBRustError_getMessage(cpp_obj: *mut c_void) -> *const c_char;
    fn WCDBRustError_enumerateInfo(cpp_obj: *mut c_void);
}

#[no_mangle]
pub extern "C" fn WCDBExceptionAddInfo(
    key_values_raw: *mut c_void,
    key: *const c_char,
    value_type: i32,
    int_value: i64,
    double_value: f64,
    string_value: *const c_char,
) {
    let key = unsafe { key.to_cow() };
    let value = match value_type {
        3 => ExceptionObject::Long(int_value),
        5 => ExceptionObject::Double(double_value),
        6 => ExceptionObject::String(unsafe { string_value.to_cow().to_string() }),
        _ => return,
    };
    let key_values: &mut HashMap<String, ExceptionObject> =
        unsafe { &mut *(key_values_raw as *mut HashMap<String, ExceptionObject>) };
    key_values.insert(key.to_string(), value);
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum ExceptionLevel {
    Ignore,
    Debug,
    Notice,
    Warning,
    Error,
    Fatal,
    Unknown,
}

impl ExceptionLevel {
    pub fn value_of(value: i32) -> Self {
        match value {
            1 => ExceptionLevel::Ignore,
            2 => ExceptionLevel::Debug,
            3 => ExceptionLevel::Notice,
            4 => ExceptionLevel::Warning,
            5 => ExceptionLevel::Error,
            6 => ExceptionLevel::Fatal,
            _ => ExceptionLevel::Unknown,
        }
    }

    pub fn to_str(&self) -> &'static str {
        match self {
            ExceptionLevel::Ignore => "IGNORE",
            ExceptionLevel::Debug => "DEBUG",
            ExceptionLevel::Notice => "NOTICE",
            ExceptionLevel::Warning => "WARNING",
            ExceptionLevel::Error => "ERROR",
            ExceptionLevel::Fatal => "FATAL",
            ExceptionLevel::Unknown => "UNKNOWN",
        }
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum ExceptionCode {
    OK = 0,
    Error = 1,
    Internal = 2,
    Permission = 3,
    Abort = 4,
    Busy = 5,
    Locked = 6,
    NoMemory = 7,
    Readonly = 8,
    Interrupt = 9,
    IOError = 10,
    Corrupt = 11,
    NotFound = 12,
    Full = 13,
    CantOpen = 14,
    Protocol = 15,
    Empty = 16,
    Schema = 17,
    Exceed = 18,
    Constraint = 19,
    Mismatch = 20,
    Misuse = 21,
    NoLargeFileSupport = 22,
    Authorization = 23,
    Format = 24,
    Range = 25,
    NotADatabase = 26,
    Notice = 27,
    Warning = 28,
    Row = 100,
    Done = 101,
    Unknown = -1,
}

impl ExceptionCode {
    pub fn value_of(value: i32) -> Self {
        match value {
            0 => ExceptionCode::OK,
            1 => ExceptionCode::Error,
            2 => ExceptionCode::Internal,
            3 => ExceptionCode::Permission,
            4 => ExceptionCode::Abort,
            5 => ExceptionCode::Busy,
            6 => ExceptionCode::Locked,
            7 => ExceptionCode::NoMemory,
            8 => ExceptionCode::Readonly,
            9 => ExceptionCode::Interrupt,
            10 => ExceptionCode::IOError,
            11 => ExceptionCode::Corrupt,
            12 => ExceptionCode::NotFound,
            13 => ExceptionCode::Full,
            14 => ExceptionCode::CantOpen,
            15 => ExceptionCode::Protocol,
            16 => ExceptionCode::Empty,
            17 => ExceptionCode::Schema,
            18 => ExceptionCode::Exceed,
            19 => ExceptionCode::Constraint,
            20 => ExceptionCode::Mismatch,
            21 => ExceptionCode::Misuse,
            22 => ExceptionCode::NoLargeFileSupport,
            23 => ExceptionCode::Authorization,
            24 => ExceptionCode::Format,
            25 => ExceptionCode::Range,
            26 => ExceptionCode::NotADatabase,
            27 => ExceptionCode::Notice,
            28 => ExceptionCode::Warning,
            100 => ExceptionCode::Row,
            101 => ExceptionCode::Done,
            _ => ExceptionCode::Unknown,
        }
    }
}

pub enum ExceptionExtendCode {
    ErrorMissingCollseq = 257,       // CodeError | (1 << 8)
    ErrorRetry = 513,                // Code.Error | (2 << 8)
    ErrorSnapshot = 769,             // Code.Error | (3 << 8)
    IOErrorRead = 266,               // Code.IOError | (1 << 8)
    IOErrorShortRead = 522,          // Code.IOError | (2 << 8)
    IOErrorWrite = 778,              // Code.IOError | (3 << 8)
    IOErrorFsync = 1034,             // Code.IOError | (4 << 8)
    IOErrorDirFsync = 1290,          // Code.IOError | (5 << 8)
    IOErrorTruncate = 1546,          // Code.IOError | (6 << 8)
    IOErrorFstat = 1802,             // Code.IOError | (7 << 8)
    IOErrorUnlock = 2058,            // Code.IOError | (8 << 8)
    IOErrorRdlock = 2314,            // Code.IOError | (9 << 8)
    IOErrorDelete = 2570,            // Code.IOError | (10 << 8)
    IOErrorBlocked = 2826,           // Code.IOError | (11 << 8)
    IOErrorNoMemory = 3082,          // Code.IOError | (12 << 8)
    IOErrorAccess = 3338,            // Code.IOError | (13 << 8)
    IOErrorCheckReservedLock = 3594, // Code.IOError | (14 << 8)
    IOErrorLock = 3850,              // Code.IOError | (15 << 8)
    IOErrorClose = 4106,             // Code.IOError | (16 << 8)
    IOErrorDirClose = 4362,          // Code.IOError | (17 << 8)
    IOErrorShmOpen = 4618,           // Code.IOError | (18 << 8)
    IOErrorShmSize = 4874,           // Code.IOError | (19 << 8)
    IOErrorShmLock = 5130,           // Code.IOError | (20 << 8)
    IOErrorShmMap = 5386,            // Code.IOError | (21 << 8)
    IOErrorSeek = 5642,              // Code.IOError | (22 << 8)
    IOErrorDeleteNoEntry = 5898,     // Code.IOError | (23 << 8)
    IOErrorMmap = 6154,              // Code.IOError | (24 << 8)
    IOErrorGetTempPath = 6410,       // Code.IOError | (25 << 8)
    IOErrorConvPath = 6666,          // Code.IOError | (26 << 8)
    IOErrorVnode = 6922,             // Code.IOError | (27 << 8)
    IOErrorAuthorization = 7178,     // Code.IOError | (28 << 8)
    IOErrorBeginAtomic = 7434,       // Code.IOError | (29 << 8)
    IOErrorCommitAtomic = 7690,      // Code.IOError | (30 << 8)
    IOErrorRollbackAtomic = 7946,    // Code.IOError | (31 << 8)
    LockedSharedCache = 262,         // Code.Locked | (1 << 8)
    LockedVirtualTable = 518,        // Code.Locked | (2 << 8)
    BusyRecovery = 261,              // Code.Busy | (1 << 8)
    BusySnapshot = 517,              // Code.Busy | (2 << 8)
    CantOpenNoTempDir = 270,         // Code.CantOpen | (1 << 8)
    CantOpenIsDir = 526,             // Code.CantOpen | (2 << 8)
    CantOpenFullPath = 782,          // Code.CantOpen | (3 << 8)
    CantOpenConvPath = 1038,         // Code.CantOpen | (4 << 8)
    CantOpenDirtyWal = 1294,         // Code.CantOpen | (5 << 8)
    CorruptVirtualTable = 267,       // Code.Corrupt | (1 << 8)
    CorruptSequence = 523,           // Code.Corrupt | (2 << 8)
    ReadonlyRecovery = 264,          // Code.Readonly | (1 << 8)
    ReadonlyCantLock = 520,          // Code.Readonly | (2 << 8)
    ReadonlyRollback = 776,          // Code.Readonly | (3 << 8)
    ReadonlyDatabaseMoved = 1032,    // Code.Readonly | (4 << 8)
    ReadonlyCantInit = 1288,         // Code.Readonly | (5 << 8)
    ReadonlyDirectory = 1544,        // Code.Readonly | (6 << 8)
    AbortRollback = 516,             // Code.Abort | (2 << 8)
    ConstraintCheck = 275,           // Code.Constraint | (1 << 8)
    ConstraintCommitHook = 531,      // Code.Constraint | (2 << 8)
    ConstraintForeignKey = 787,      // Code.Constraint | (3 << 8)
    ConstraintFunction = 1043,       // Code.Constraint | (4 << 8)
    ConstraintNotNull = 1299,        // Code.Constraint | (5 << 8)
    ConstraintPrimaryKey = 1555,     // Code.Constraint | (6 << 8)
    ConstraintTrigger = 1811,        // Code.Constraint | (7 << 8)
    ConstraintUnique = 2067,         // Code.Constraint | (8 << 8)
    ConstraintVirtualTable = 2323,   // Code.Constraint | (9 << 8)
    ConstraintRowID = 2579,          // Code.Constraint | (10 << 8)
    NoticeRecoverWal = 283,          // Code.Notice | (1 << 8)
    NoticeRecoverRollback = 539,     // Code.Notice | (2 << 8)
    WarningAutoIndex = 284,          // Code.Warning | (1 << 8)
    AuthorizationUser = 279,         // Code.Authorization | (1 << 8)
    OKLoadPermanently = 256,         // Code.OK | (1 << 8)
    Unknown = -1,
}

pub enum ExceptionKey {
    Tag,
    Path,
    Type,
    Source,
    SQL,
    ExtendedCode,
    Message,
}

impl Display for ExceptionKey {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            ExceptionKey::Tag => write!(f, "Tag"),
            ExceptionKey::Path => write!(f, "Path"),
            ExceptionKey::Type => write!(f, "Type"),
            ExceptionKey::Source => write!(f, "Source"),
            ExceptionKey::SQL => write!(f, "SQL"),
            ExceptionKey::ExtendedCode => write!(f, "ExtCode"),
            ExceptionKey::Message => write!(f, "Message"),
        }
    }
}

pub enum ExceptionObject {
    Long(i64),
    Double(f64),
    String(String),
}

pub type WCDBResult<T> = Result<T, WCDBException>;

#[derive(Debug)]
pub enum WCDBException {
    WCDBNormalException(ExceptionInner),
    WCDBInterruptException(ExceptionInner),
    WCDBCorruptOrIOException(ExceptionInner),
}

impl WCDBException {
    pub fn create_exception(cpp_obj: *mut c_void) -> Self {
        let level = ExceptionLevel::value_of(unsafe { WCDBRustError_getLevel(cpp_obj) });
        let code = ExceptionCode::value_of(unsafe { WCDBRustError_getCode(cpp_obj) });
        if level != ExceptionLevel::Error {
            WCDBException::WCDBNormalException(ExceptionInner::new(level, code, cpp_obj))
        } else if code == ExceptionCode::Interrupt {
            WCDBException::WCDBInterruptException(ExceptionInner::new(level, code, cpp_obj))
        } else if code == ExceptionCode::Permission
            || code == ExceptionCode::Readonly
            || code == ExceptionCode::IOError
            || code == ExceptionCode::Corrupt
            || code == ExceptionCode::Full
            || code == ExceptionCode::CantOpen
            || code == ExceptionCode::NotADatabase
        {
            WCDBException::WCDBCorruptOrIOException(ExceptionInner::new(level, code, cpp_obj))
        } else {
            WCDBException::WCDBNormalException(ExceptionInner::new(level, code, cpp_obj))
        }
    }

    pub fn message(&self) -> String {
        match self {
            WCDBException::WCDBNormalException(inner) => inner.message(),
            WCDBException::WCDBInterruptException(inner) => inner.message(),
            WCDBException::WCDBCorruptOrIOException(inner) => inner.message(),
        }
    }
}

pub struct ExceptionInner {
    pub level: ExceptionLevel,
    pub code: ExceptionCode,
    pub key_values: HashMap<String, ExceptionObject>,
}

impl Debug for ExceptionInner {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Level: {:?}", self.level)?;
        write!(f, "Code: {:?}", self.code)?;
        let mut debug_struct = f.debug_struct("Exception");
        for (key, value) in &self.key_values {
            match value {
                ExceptionObject::Long(value) => debug_struct.field(key, value),
                ExceptionObject::Double(value) => debug_struct.field(key, value),
                ExceptionObject::String(value) => debug_struct.field(key, value),
            };
        }
        debug_struct.finish()?;
        Ok(())
    }
}

impl ExceptionInner {
    pub fn new(level: ExceptionLevel, code: ExceptionCode, cpp_obj: *mut c_void) -> Self {
        let mut key_values = HashMap::new();
        let message = unsafe { WCDBRustError_getMessage(cpp_obj) };
        key_values.insert(
            ExceptionKey::Message.to_string(),
            ExceptionObject::String(message.to_cow().to_string()),
        );
        unsafe {
            WCDBRustError_enumerateInfo(cpp_obj);
        }
        ExceptionInner {
            level,
            code,
            key_values,
        }
    }

    pub fn message(&self) -> String {
        let exception_obj_opt = self.key_values.get(&ExceptionKey::Message.to_string());
        if exception_obj_opt.is_none() {
            return String::new();
        }
        let exception_obj = exception_obj_opt.unwrap();
        match exception_obj {
            ExceptionObject::Long(value) => value.to_string(),
            ExceptionObject::Double(value) => value.to_string(),
            ExceptionObject::String(value) => value.to_string(),
        }
    }
}
