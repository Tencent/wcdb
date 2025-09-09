use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_int, c_void};

#[repr(i32)]
pub enum Action {
    SetNull = 0,
    SetDefault = 1,
    Cascade = 2,
    Restrict = 3,
    NoAction = 4,
}
#[repr(i32)]
pub enum Match {
    Simple = 0,
    Full = 1,
    Partial = 2,
}
#[repr(i32)]
pub enum Initially {
    Default = 0,
    Deferred = 1,
    Immediate = 2,
}

extern "C" {
    fn WCDBRustForeignKey_createCppObject() -> *mut c_void;

    fn WCDBRustForeignKey_configReferencesTable(cpp_obj: *mut c_void, table: *const c_char);

    fn WCDBRustForeignKey_configColumns(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        objects: *const *mut c_void,
        column_names: *const *const c_char,
        count: usize,
    );

    fn WCDBRustForeignKey_configOnDeleteAction(cpp_obj: *mut c_void, action: c_int);

    fn WCDBRustForeignKey_configOnUpdateAction(cpp_obj: *mut c_void, action: c_int);

    fn WCDBRustForeignKey_configMatch(cpp_obj: *mut c_void, r#match: c_int);

    fn WCDBRustForeignKey_configDeferrable(cpp_obj: *mut c_void, r#type: c_int);

    fn WCDBRustForeignKey_configNotDeferrable(cpp_obj: *mut c_void, r#type: c_int);
}

pub struct ForeignKey {
    identifier: Identifier,
}

impl CppObjectTrait for ForeignKey {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object()
    }
}

impl CppObjectConvertibleTrait for ForeignKey {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for ForeignKey {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for ForeignKey {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl ForeignKey {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustForeignKey_createCppObject() };
        ForeignKey {
            identifier: Identifier::new(CPPType::ForeignKeyClause, Some(cpp_obj)),
        }
    }

    pub fn references(&self, table: &str) -> &Self {
        let c_str = table.to_string().to_cstring();
        unsafe {
            WCDBRustForeignKey_configReferencesTable(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn column(&self, column: Column) -> &Self {
        let mut objects: Vec<*mut c_void> = Vec::new();
        objects.push(CppObject::get(&column));
        unsafe {
            WCDBRustForeignKey_configColumns(
                self.get_cpp_obj(),
                CPPType::Column as c_int,
                objects.as_ptr(),
                std::ptr::null(),
                1,
            );
        }
        self
    }

    pub fn column_with_string(&self, column: &str) -> &Self {
        let cstr = column.to_string().to_cstring();
        let mut objects: Vec<*const c_char> = Vec::new();
        objects.push(cstr.as_ptr());
        unsafe {
            WCDBRustForeignKey_configColumns(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                std::ptr::null(),
                objects.as_ptr(),
                1,
            );
        }
        self
    }

    pub fn columns(&self, column: &Vec<Column>) -> &Self {
        if column.is_empty() {
            return self;
        }
        let size = column.len();
        let mut objects: Vec<*mut c_void> = Vec::with_capacity(size);
        for item in column {
            objects.push(CppObject::get(item));
        }
        unsafe {
            WCDBRustForeignKey_configColumns(
                self.get_cpp_obj(),
                CPPType::Column as c_int,
                objects.as_ptr(),
                std::ptr::null(),
                size,
            );
        }
        self
    }

    pub fn column_with_string_list(&self, column: &Vec<String>) -> &Self {
        if column.is_empty() {
            return self;
        }
        let size = column.len();
        let mut objects: Vec<*const c_char> = Vec::with_capacity(size);
        for item in column {
            let cstr = item.to_string().to_cstring();
            objects.push(cstr.as_ptr());
        }
        unsafe {
            WCDBRustForeignKey_configColumns(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                std::ptr::null(),
                objects.as_ptr(),
                size,
            );
        }
        self
    }

    pub fn on_delete(&self, action: Action) -> &Self {
        unsafe {
            WCDBRustForeignKey_configOnDeleteAction(self.get_cpp_obj(), action as c_int);
        }
        self
    }

    pub fn on_update(&self, action: Action) -> &Self {
        unsafe {
            WCDBRustForeignKey_configOnUpdateAction(self.get_cpp_obj(), action as c_int);
        }
        self
    }

    pub fn match_(&self, match_: Match) -> &Self {
        unsafe {
            WCDBRustForeignKey_configMatch(self.get_cpp_obj(), (match_ as c_int) + 1);
        }
        self
    }

    pub fn deferrable(&self, initially: Initially) -> &Self {
        unsafe {
            WCDBRustForeignKey_configDeferrable(self.get_cpp_obj(), initially as c_int);
        }
        self
    }

    pub fn not_deferrable(&self, initially: Initially) -> &Self {
        unsafe {
            WCDBRustForeignKey_configNotDeferrable(self.get_cpp_obj(), initially as c_int);
        }
        self
    }
}
