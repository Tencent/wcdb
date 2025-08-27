use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use core::ffi::c_size_t;
use std::borrow::Cow;
use std::ffi::{c_char, c_int, c_longlong, c_void};

extern "C" {
    fn WCDBRustTableConstraint_create(name: *const c_char) -> *mut c_void;

    fn WCDBRustTableConstraint_configPrimaryKey(cpp_obj: *mut c_void);

    fn WCDBRustTableConstraint_configUnique(cpp_obj: *mut c_void);

    fn WCDBRustTableConstraint_configIndexedColumn(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        column_vec: *const c_longlong,
        column_name_vec: *const *const c_char,
        column_vec_len: c_size_t,
    );
}

pub struct TableConstraint {
    identifier: Identifier,
}

impl CppObjectTrait for TableConstraint {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for TableConstraint {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for TableConstraint {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for TableConstraint {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

pub trait TableConstraintIndexedByParam {
    fn get_params(
        &self,
        column_vec: &mut Vec<*const c_longlong>,
        column_name_vec: &mut Vec<*const c_char>,
    ) -> CPPType;
}

impl<'a, T, R> TableConstraintIndexedByParam for T
where
    T: IntoIterator<Item = Cow<'a, R>>,
    R: IndexedColumnConvertibleTrait,
{
    fn get_params(
        &self,
        column_vec: &mut Vec<*const c_longlong>,
        column_name_vec: &mut Vec<*const c_char>,
    ) -> CPPType {
        for item in self {
            column_vec.push(item.get_cpp_obj());
        }
        CPPType::String
    }
}

impl<'a, T> TableConstraintIndexedByParam for T
where
    T: IntoIterator<Item = Cow<'a, str>>,
{
    fn get_params(
        &self,
        column_vec: &mut Vec<*const c_longlong>,
        column_name_vec: &mut Vec<*const c_char>,
    ) -> CPPType {
        for item in self {

        }
        CPPType::String
    }
}

impl TableConstraint {
    pub fn new(name_opt: Option<&str>) -> Self {
        let cpp_obj = match name_opt {
            Some(name) => unsafe { WCDBRustTableConstraint_create(name.to_cstring().as_ptr()) },
            None => unsafe { WCDBRustTableConstraint_create(std::ptr::null_mut()) },
        };
        Self {
            identifier: Identifier::new(CPPType::TableConstraint, Some(cpp_obj)),
        }
    }

    pub fn primary_key(&self) -> &Self {
        unsafe {
            WCDBRustTableConstraint_configPrimaryKey(self.get_cpp_obj());
        }
        self
    }

    pub fn unique(&self) -> &Self {
        unsafe {
            WCDBRustTableConstraint_configUnique(self.get_cpp_obj());
        }
        self
    }

    // pub fn indexed_by<'a, T, R>(&self, param_vec: T)
    // where
    //     T: IntoIterator<Item = Cow<'a, R>>,
    //     R: TableConstraintIndexedByParam,
    // {
    //     let mut cpp_type = param_vec[0];
    //     let mut cpp_obj_vec = vec![];
    //     let mut cstr_vec = vec![];
    //     for param in param_vec {
    //         let params = param.get_params();
    //         match params.0 {
    //             CPPType::String => cstr_vec.push(params.1 as *const c_char),
    //             _ => cpp_obj_vec.push(params.1 as c_longlong),
    //         }
    //     }
    //     unsafe {
    //         WCDBRustTableConstraint_configIndexedColumn(
    //             self.get_cpp_obj(),
    //             cpp_type as c_int,
    //             c_void_vec.as_ptr(),
    //             std::ptr::null(),
    //             columns_void_vec_len,
    //         );
    //     }
    //     self
    // }
}
