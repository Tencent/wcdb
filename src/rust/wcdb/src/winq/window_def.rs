use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::frame_spec::FrameSpec;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::ordering_term::OrderingTerm;
use std::ffi::{c_char, c_double, c_int, c_void};

extern "C" {
    fn WCDBRustWindowDef_createCppObj() -> *mut c_void;

    fn WCDBRustWindowDef_configPartitions(
        cpp_obj: *mut c_void,
        types: *const c_int,
        columns: *const *mut c_void,
        unused: *const *mut c_double,
        column_names: *const *const c_char,
        array_len: c_int,
    );

    fn WCDBRustWindowDef_configOrders(
        cpp_obj: *mut c_void,
        columns: *const *mut c_void,
        total_length: c_int,
    );

    fn WCDBRustWindowDef_configFrameSpec(cpp_obj: *mut c_void, frame_spec: *mut c_void);
}

#[derive(Debug)]
pub struct WindowDef {
    identifier: Identifier,
}

impl CppObjectTrait for WindowDef {
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

impl CppObjectConvertibleTrait for WindowDef {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for WindowDef {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for WindowDef {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl WindowDef {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustWindowDef_createCppObj() };
        WindowDef {
            identifier: Identifier::new(CPPType::WindowDef, Some(cpp_obj)),
        }
    }

    pub fn partition_by_with_string(self, column_names: &Vec<&str>) -> Self {
        if column_names.is_empty() {
            return self;
        }
        let size = column_names.len();
        let mut column_name_vec: Vec<*const c_char> = Vec::new();
        for item in column_names {
            let cstr = item.to_cstring();
            column_name_vec.push(cstr.into_raw());
        }
        let mut types: Vec<c_int> = vec![CPPType::String as c_int; size];
        unsafe {
            WCDBRustWindowDef_configPartitions(
                self.get_cpp_obj(),
                types.as_ptr(),
                std::ptr::null_mut(),
                std::ptr::null_mut(),
                column_name_vec.as_ptr(),
                size as c_int,
            );
        }
        self
    }

    pub fn partition<T>(self, expressions: &Vec<&T>) -> Self
    where
        T: ExpressionConvertibleTrait,
    {
        if expressions.is_empty() {
            return self;
        }
        let size = expressions.len();
        let mut types: Vec<c_int> = Vec::with_capacity(size);
        let mut cpp_objs: Vec<*mut c_void> = Vec::with_capacity(size);
        for index in 0..size {
            types.push(Identifier::get_cpp_type(expressions[index]) as c_int);
            cpp_objs.push(CppObject::get(expressions[index]));
        }
        unsafe {
            WCDBRustWindowDef_configPartitions(
                self.get_cpp_obj(),
                types.as_ptr(),
                cpp_objs.as_ptr(),
                std::ptr::null_mut(),
                std::ptr::null_mut(),
                size as c_int,
            );
        }
        self
    }

    pub fn order_by(self, orders: &Vec<&OrderingTerm>) -> Self {
        if orders.is_empty() {
            return self;
        }
        let size = orders.len();
        let mut cpp_objs: Vec<*mut c_void> = Vec::with_capacity(size);
        for item in orders {
            cpp_objs.push(item.get_cpp_obj());
        }
        unsafe {
            WCDBRustWindowDef_configOrders(self.get_cpp_obj(), cpp_objs.as_ptr(), size as c_int);
        }
        self
    }

    pub fn frame_spec(self, frame_spec: &FrameSpec) -> WindowDef {
        unsafe { WCDBRustWindowDef_configFrameSpec(self.get_cpp_obj(), CppObject::get(frame_spec)) }
        self
    }
}
