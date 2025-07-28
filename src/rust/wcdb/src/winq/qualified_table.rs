use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use std::ffi::{c_char, c_int, c_void};
use std::ptr::null;

extern "C" {
    fn WCDBRustQualifiedTable_create(table_name: *const c_char) -> *mut c_void;
    fn WCDBRustQualifiedTable_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustQualifiedTable_configAlias(result_column: *mut c_void, alias: *const c_char);

    fn WCDBRustQualifiedTable_configIndex(cpp_obj: *mut c_void, index_name: *const c_char);

    fn WCDBRustQualifiedTable_configNotIndexed(cpp_obj: *mut c_void);

}
pub struct QualifiedTable {
    identifier: Identifier,
}

impl CppObjectConvertibleTrait for QualifiedTable {
    fn as_cpp_object(&self) -> *mut c_void {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierConvertibleTrait for QualifiedTable {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl IdentifierTrait for QualifiedTable {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for QualifiedTable {
    fn get_type() -> i32 {
        CPPType::QualifiedTableName as i32
    }
}

impl CppObjectTrait for QualifiedTable {
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

impl QualifiedTable {
    pub fn new_with_table_name(table_name: &str) -> Self {
        let cstr = table_name.to_cstring();
        let cpp_obj = unsafe { WCDBRustQualifiedTable_create(cstr.as_ptr()) };
        QualifiedTable {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
    pub fn of_string(&self, schema: &str) -> &Self {
        let cstr = schema.to_cstring();
        unsafe {
            WCDBRustQualifiedTable_configSchema(
                self.get_cpp_obj(),
                CPPType::String as i32,
                0 as *mut c_void,
                cstr.as_ptr(),
            )
        }
        self
    }

    pub fn of_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustQualifiedTable_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema),
                CppObject::get(&schema),
                null(),
            )
        }
        self
    }

    pub fn as_(&self, alias: &str) -> &Self {
        let cstr = alias.to_cstring();
        unsafe { WCDBRustQualifiedTable_configAlias(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }

    pub fn indexed(&self, index_name: &str) -> &Self {
        let cstr = index_name.to_cstring();
        unsafe { WCDBRustQualifiedTable_configIndex(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }

    pub fn not_indexed(&self) -> &Self {
        unsafe { WCDBRustQualifiedTable_configNotIndexed(self.get_cpp_obj()) }
        self
    }
}
