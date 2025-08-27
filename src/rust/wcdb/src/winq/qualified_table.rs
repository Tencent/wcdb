use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use std::ffi::{c_char, c_int, c_void};

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

impl CppObjectConvertibleTrait for QualifiedTable {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for QualifiedTable {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for QualifiedTable {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl QualifiedTable {
    pub fn new(table_name: &str) -> Self {
        let cpp_obj = unsafe { WCDBRustQualifiedTable_create(table_name.to_cstring().as_ptr()) };
        QualifiedTable {
            identifier: Identifier::new(CPPType::QualifiedTableName, Some(cpp_obj)),
        }
    }

    pub fn of_string(&self, schema: &str) -> &Self {
        unsafe {
            WCDBRustQualifiedTable_configSchema(
                self.get_cpp_obj(),
                CPPType::String as i32,
                std::ptr::null_mut(),
                schema.to_cstring().as_ptr(),
            )
        }
        self
    }

    pub fn of_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustQualifiedTable_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            )
        }
        self
    }

    pub fn r#as(&self, alias: &str) -> &Self {
        unsafe {
            WCDBRustQualifiedTable_configAlias(self.get_cpp_obj(), alias.to_cstring().as_ptr())
        }
        self
    }

    pub fn indexed(&self, index_name: &str) -> &Self {
        unsafe {
            WCDBRustQualifiedTable_configIndex(self.get_cpp_obj(), index_name.to_cstring().as_ptr())
        }
        self
    }

    pub fn not_indexed(&self) -> &Self {
        unsafe { WCDBRustQualifiedTable_configNotIndexed(self.get_cpp_obj()) }
        self
    }
}
