use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_column::StringColumn;
use crate::utils::ToCString;
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

    fn WCDBRustForeignKey_configMatch(cpp_obj: *mut c_void, match_: c_int);

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

    pub fn column<'a, T>(&self, column: T) -> &Self
    where
        T: Into<StringColumn<'a>>,
    {
        let (cpp_type, cpp_obj, name_opt) = column.into().get_params();
        let name_ptr = name_opt
            .as_ref()
            .map(|s| s.as_ptr())
            .unwrap_or(std::ptr::null());
        let mut cpp_obj_vec = Vec::new();
        cpp_obj_vec.push(cpp_obj);
        let mut c_str_vec = Vec::new();
        c_str_vec.push(name_ptr);
        unsafe {
            WCDBRustForeignKey_configColumns(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj_vec.as_ptr(),
                c_str_vec.as_ptr(),
                1,
            );
        }
        self
    }

    pub fn columns<'a, I, S>(&self, columns: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumn<'a>>,
    {
        let data_vec = columns.into_iter().map(Into::into).collect::<Vec<_>>();
        if data_vec.is_empty() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        let mut c_strings = vec![];
        for item in data_vec {
            match item {
                StringColumn::String(str) => {
                    let c_string = str.to_cstring();
                    cpp_str_vec.push(c_string.as_ptr());
                    c_strings.push(c_string);
                }
                StringColumn::Column(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustForeignKey_configColumns(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len(),
                );
            }
        } else {
            unsafe {
                WCDBRustForeignKey_configColumns(
                    self.get_cpp_obj(),
                    CPPType::Column as c_int,
                    cpp_obj_vec.as_ptr(),
                    std::ptr::null(),
                    cpp_obj_vec.len(),
                );
            }
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
