use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::column_constraint::ColumnConstraint;
use crate::winq::column_type::ColumnType;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustColumnDef_create(
        cpp_type: c_int,
        column_cpp_obj: *mut c_void,
        name: *const c_char,
        column_type: c_int,
    ) -> *mut c_void;

    fn WCDBRustColumnDef_constraint(cpp_obj: *mut c_void, constraint_cpp_obj: *mut c_void);
}

pub struct ColumnDef {
    identifier: Identifier,
}

impl CppObjectTrait for ColumnDef {
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

impl CppObjectConvertibleTrait for ColumnDef {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for ColumnDef {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for ColumnDef {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl ColumnDef {
    pub fn new<'a, T>(param: T) -> ColumnDef
    where
        T: Into<ColumnDefParam<'a>>,
    {
        let cpp_obj = match param.into() {
            ColumnDefParam::String(str, column_type_opt) => {
                let cpp_type = match column_type_opt {
                    Some(column_type) => column_type as c_int,
                    None => 0,
                };
                let c_name = str.to_cstring();
                unsafe {
                    WCDBRustColumnDef_create(
                        CPPType::String as c_int,
                        std::ptr::null_mut(),
                        c_name.as_ptr(),
                        cpp_type,
                    )
                }
            }
            ColumnDefParam::Column(column, column_type_opt) => {
                let cpp_type = match column_type_opt {
                    Some(column_type) => column_type as c_int,
                    None => 0,
                };
                unsafe {
                    WCDBRustColumnDef_create(
                        Identifier::get_cpp_type(column) as c_int,
                        CppObject::get(column),
                        std::ptr::null_mut(),
                        cpp_type,
                    )
                }
            }
        };
        Self {
            identifier: Identifier::new(CPPType::ColumnDef, Some(cpp_obj)),
        }
    }

    pub fn constraint(&self, constraint: &ColumnConstraint) -> &Self {
        unsafe {
            WCDBRustColumnDef_constraint(self.get_cpp_obj(), CppObject::get(constraint));
        }
        self
    }
}

pub enum ColumnDefParam<'a> {
    String(&'a str, Option<ColumnType>),
    Column(&'a Column, Option<ColumnType>),
}

impl<'a> From<&'a str> for ColumnDefParam<'a> {
    fn from(name: &'a str) -> Self {
        ColumnDefParam::String(name, None)
    }
}

impl<'a> From<(&'a str, ColumnType)> for ColumnDefParam<'a> {
    fn from((name, ty): (&'a str, ColumnType)) -> Self {
        ColumnDefParam::String(name, Some(ty))
    }
}

impl<'a> From<(&'a str, Option<ColumnType>)> for ColumnDefParam<'a> {
    fn from((name, ty_opt): (&'a str, Option<ColumnType>)) -> Self {
        ColumnDefParam::String(name, ty_opt)
    }
}

impl<'a> From<&'a Column> for ColumnDefParam<'a> {
    fn from(col: &'a Column) -> Self {
        ColumnDefParam::Column(col, None)
    }
}

impl<'a> From<(&'a Column, ColumnType)> for ColumnDefParam<'a> {
    fn from((col, ty): (&'a Column, ColumnType)) -> Self {
        ColumnDefParam::Column(col, Some(ty))
    }
}

impl<'a> From<(&'a Column, Option<ColumnType>)> for ColumnDefParam<'a> {
    fn from((col, ty_opt): (&'a Column, Option<ColumnType>)) -> Self {
        ColumnDefParam::Column(col, ty_opt)
    }
}
