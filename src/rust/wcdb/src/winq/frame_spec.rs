use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_int_expression::IntExpression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_int, c_longlong, c_void};

extern "C" {
    fn WCDBRustFrameSpec_createCppObj() -> *mut c_void;
    fn WCDBRustFrameSpec_configRange(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configRows(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configUnboundedPreceding(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configPreceding(
        cpp_obj: *mut c_void,
        c_type: c_int,
        offset_obj: c_longlong,
    );

    fn WCDBRustFrameSpec_configCurrentRow(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configBetweenUnboundedPreceding(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configBetweenPreceding(
        cpp_obj: *mut c_void,
        c_type: c_int,
        offset_obj: c_longlong,
    );

    fn WCDBRustFrameSpec_configBetweenCurrentRow(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configBetweenFollowing(
        cpp_obj: *mut c_void,
        c_type: c_int,
        offset_obj: c_longlong,
    );

    fn WCDBRustFrameSpec_configAndCurrentRow(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configAndPreceding(
        cpp_obj: *mut c_void,
        c_type: c_int,
        offset_obj: c_longlong,
    );

    fn WCDBRustFrameSpec_configAndUnboundedFollowing(cpp_obj: *mut c_void);

    fn WCDBRustFrameSpec_configAndFollowing(
        cpp_obj: *mut c_void,
        c_type: c_int,
        offset_obj: c_longlong,
    );
}

#[derive(Debug)]
pub struct FrameSpec {
    identifier: Identifier,
}

impl CppObjectTrait for FrameSpec {
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

impl CppObjectConvertibleTrait for FrameSpec {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for FrameSpec {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for FrameSpec {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl FrameSpec {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustFrameSpec_createCppObj() };
        FrameSpec {
            identifier: Identifier::new(CPPType::FrameSpec, Some(cpp_obj)),
        }
    }

    pub fn range(&self) -> &Self {
        unsafe { WCDBRustFrameSpec_configRange(self.get_cpp_obj()) }
        self
    }

    pub fn rows(&self) -> &Self {
        unsafe { WCDBRustFrameSpec_configRows(self.get_cpp_obj()) }
        self
    }

    pub fn unbounded_preceding(&self) -> &Self {
        unsafe { WCDBRustFrameSpec_configUnboundedPreceding(self.get_cpp_obj()) }
        self
    }

    pub fn preceding<'a, T>(&self, offset: T) -> &Self
    where
        T: Into<IntExpression<'a>>,
    {
        let (cpp_type, cpp_obj) = offset.into().get_params();
        unsafe {
            WCDBRustFrameSpec_configPreceding(self.get_cpp_obj(), cpp_type as c_int, cpp_obj);
        }
        self
    }

    pub fn current_row(&self) -> &Self {
        unsafe {
            WCDBRustFrameSpec_configCurrentRow(self.get_cpp_obj());
        }
        self
    }

    pub fn between_unbounded_preceding(&self) -> &Self {
        unsafe {
            WCDBRustFrameSpec_configBetweenUnboundedPreceding(self.get_cpp_obj());
        }
        self
    }

    pub fn between_preceding<'a, T>(&self, offset: T) -> &Self
    where
        T: Into<IntExpression<'a>>,
    {
        let (cpp_type, cpp_obj) = offset.into().get_params();
        unsafe {
            WCDBRustFrameSpec_configBetweenPreceding(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
            );
        }
        self
    }

    pub fn between_current_row(&self) -> &Self {
        unsafe {
            WCDBRustFrameSpec_configBetweenCurrentRow(self.get_cpp_obj());
        }
        self
    }

    pub fn between_following<'a, T>(&self, offset: T) -> &Self
    where
        T: Into<IntExpression<'a>>,
    {
        let (cpp_type, cpp_obj) = offset.into().get_params();
        unsafe {
            WCDBRustFrameSpec_configBetweenFollowing(
                self.get_cpp_obj(),
                cpp_type as c_int,
                cpp_obj,
            );
        }
        self
    }

    pub fn and_current_row(&self) -> &Self {
        unsafe {
            WCDBRustFrameSpec_configAndCurrentRow(self.get_cpp_obj());
        }
        self
    }

    pub fn and_preceding<'a, T>(&self, offset: T) -> &Self
    where
        T: Into<IntExpression<'a>>,
    {
        let (cpp_type, cpp_obj) = offset.into().get_params();
        unsafe {
            WCDBRustFrameSpec_configAndPreceding(self.get_cpp_obj(), cpp_type as c_int, cpp_obj);
        }
        self
    }

    pub fn and_unbounded_following(&self) -> &Self {
        unsafe {
            WCDBRustFrameSpec_configAndUnboundedFollowing(self.get_cpp_obj());
        }
        self
    }

    pub fn and_following<'a, T>(&self, offset: T) -> &Self
    where
        T: Into<IntExpression<'a>>,
    {
        let (cpp_type, cpp_obj) = offset.into().get_params();
        unsafe {
            WCDBRustFrameSpec_configAndFollowing(self.get_cpp_obj(), cpp_type as c_int, cpp_obj);
        }
        self
    }
}
