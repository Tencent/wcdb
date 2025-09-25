use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_expression::StringExpression;
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

    pub fn partition_by<'a, I, S>(&self, column_names_or_expressions: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringExpression<'a>>,
    {
        let data_vec = column_names_or_expressions
            .into_iter()
            .map(Into::into)
            .collect::<Vec<_>>();
        if data_vec.is_empty() {
            return self;
        }
        let mut cpp_type_vec = vec![];
        let mut cpp_obj_vec = vec![];
        let mut cpp_str_ptrs: Vec<*const c_char> = vec![];
        let mut c_strings = vec![];
        for item in data_vec {
            let (cpp_type, cpp_obj, c_str_opt) = item.get_params();
            cpp_type_vec.push(cpp_type as c_int);
            cpp_obj_vec.push(cpp_obj);
            if let Some(c) = c_str_opt {
                cpp_str_ptrs.push(c.as_ptr());
                c_strings.push(c);
            }
        }
        unsafe {
            WCDBRustWindowDef_configPartitions(
                self.get_cpp_obj(),
                cpp_type_vec.as_ptr(),
                cpp_obj_vec.as_ptr(),
                std::ptr::null(),
                cpp_str_ptrs.as_ptr(),
                cpp_type_vec.len() as c_int,
            );
        }
        self
    }

    pub fn order_by(&self, orders: &Vec<&OrderingTerm>) -> &Self {
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

    pub fn frame_spec(&self, frame_spec: &FrameSpec) -> &Self {
        unsafe { WCDBRustWindowDef_configFrameSpec(self.get_cpp_obj(), CppObject::get(frame_spec)) }
        self
    }
}
