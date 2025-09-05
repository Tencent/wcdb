use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCow;
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use num_derive::FromPrimitive;
use std::ffi::{c_char, c_void};
use std::fmt::Debug;

extern "C" {
    pub fn WCDBRustWinq_isWriteStatement(statement: *mut c_void) -> bool;
    fn WCDBRustWinq_getDescription(statement: *mut c_void) -> *const c_char;
}

#[derive(Clone, Debug, Eq, FromPrimitive, PartialEq)]
#[repr(i32)]
pub enum CPPType {
    Invalid = 0,
    Null = 1,
    Bool = 2,
    Int = 3,
    UInt = 4,
    Double = 5,
    String = 6,

    Column = 7,
    Schema = 8,
    ColumnDef = 9,
    ColumnConstraint = 10,
    Expression = 11,
    LiteralValue = 12,
    ForeignKeyClause = 13,
    BindParameter = 14,
    RaiseFunction = 15,
    WindowDef = 16,
    Filter = 17,
    IndexedColumn = 18,
    TableConstraint = 19,
    CommonTableExpression = 20,
    QualifiedTableName = 21,
    OrderingTerm = 22,
    UpsertClause = 23,
    Pragma = 24,
    JoinClause = 25,
    TableOrSubquery = 26,
    JoinConstraint = 27,
    SelectCore = 28,
    ResultColumn = 29,
    FrameSpec = 30,

    AlterTableSTMT = 31,
    AnalyzeSTMT = 32,
    AttachSTMT = 33,
    BeginSTMT = 34,
    CommitSTMT = 35,
    RollbackSTMT = 36,
    SavepointSTMT = 37,
    ReleaseSTMT = 38,
    CreateIndexSTMT = 39,
    CreateTableSTMT = 40,
    CreateTriggerSTMT = 41,
    SelectSTMT = 42,
    InsertSTMT = 43,
    DeleteSTMT = 44,
    UpdateSTMT = 45,
    CreateViewSTMT = 46,
    CreateVirtualTableSTMT = 47,
    DetachSTMT = 48,
    DropIndexSTMT = 49,
    DropTableSTMT = 50,
    DropTriggerSTMT = 51,
    DropViewSTMT = 52,
    PragmaSTMT = 53,
    ReindexSTMT = 54,
    VacuumSTMT = 55,
    ExplainSTMT = 56,
}

#[derive(Debug, Clone)]
pub struct Identifier {
    cpp_type: CPPType,
    cpp_obj: CppObject,
}

impl CppObjectTrait for Identifier {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.cpp_obj.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.cpp_obj.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.cpp_obj.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for Identifier {
    fn as_cpp_object(&self) -> &CppObject {
        &self.cpp_obj
    }
}

pub trait IdentifierTrait: IdentifierConvertibleTrait {
    fn get_type(&self) -> CPPType;
    fn get_description(&self) -> String;
}

impl IdentifierTrait for Identifier {
    fn get_type(&self) -> CPPType {
        self.cpp_type.clone()
    }

    fn get_description(&self) -> String {
        let c_description = unsafe { WCDBRustWinq_getDescription(self.get_cpp_obj()) };
        c_description.to_cow().to_string()
    }
}

impl IdentifierConvertibleTrait for Identifier {
    fn as_identifier(&self) -> &Self {
        self
    }
}

impl Identifier {
    pub(crate) fn new(cpp_type: CPPType, cpp_obj_opt: Option<*mut c_void>) -> Self {
        Identifier {
            cpp_type,
            cpp_obj: CppObject::new(cpp_obj_opt),
        }
    }

    // todo qixinbing 是否将参数用 Option 包？
    pub(crate) fn get_cpp_type<T: IdentifierConvertibleTrait + ?Sized>(identifier: &T) -> CPPType {
        identifier.as_identifier().get_type()
    }
}
