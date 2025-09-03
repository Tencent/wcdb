use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_longlong, c_void};
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;

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

// pub trait TableConstraintIndexedByParam {
//     fn get_params(
//         &self,
//         column_vec: &mut Vec<*const c_longlong>,
//         column_name_vec: &mut Vec<*const c_char>,
//     ) -> CPPType;
// }
//
// impl<'a, T, R> TableConstraintIndexedByParam for T
// where
//     T: IntoIterator<Item = Cow<'a, R>>,
//     R: IndexedColumnConvertibleTrait,
// {
//     fn get_params(
//         &self,
//         column_vec: &mut Vec<*const c_longlong>,
//         column_name_vec: &mut Vec<*const c_char>,
//     ) -> CPPType {
//         for item in self {
//             column_vec.push(item.get_cpp_obj());
//         }
//         CPPType::String
//     }
// }
//
// impl<'a, T> TableConstraintIndexedByParam for T
// where
//     T: IntoIterator<Item = Cow<'a, str>>,
// {
//     fn get_params(
//         &self,
//         column_vec: &mut Vec<*const c_longlong>,
//         column_name_vec: &mut Vec<*const c_char>,
//     ) -> CPPType {
//         for item in self {
//
//         }
//         CPPType::String
//     }
// }

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


    pub fn indexed_by<T>(&self, column_convertible_vec: Vec<&T>) -> &Self
    where
        T: IndexedColumnConvertibleTrait,
    {
        if column_convertible_vec.is_empty() {
            return self;
        }
        let columns_void_vec_len = column_convertible_vec.len();
        let mut cpp_obj_vec = Vec::with_capacity(column_convertible_vec.len());
        let cpp_type = Identifier::get_cpp_type(column_convertible_vec[0]) as c_int;
        for item in column_convertible_vec {
            cpp_obj_vec.push(CppObject::get(item) as c_longlong);
        }
        unsafe {
            WCDBRustTableConstraint_configIndexedColumn(
                self.get_cpp_obj(),
                cpp_type,
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }

}
