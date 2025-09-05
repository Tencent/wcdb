use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::expression_convertible_param::ExpressionConvertibleParam;
use crate::utils::ToCString;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_void};

extern "C" {
    fn WCDBRustColumnConstraint_create(name: *const c_char) -> *mut c_void;

    fn WCDBRustColumnConstraint_configPrimaryKey(cpp_obj: *mut c_void);

    fn WCDBRustColumnConstraint_configAutoIncrement(cpp_obj: *mut c_void);

    fn WCDBRustColumnConstraint_configNotNull(cpp_obj: *mut c_void);

    fn WCDBRustColumnConstraint_configConflictAction(cpp_obj: *mut c_void, action: c_int);

    fn WCDBRustColumnConstraint_configUnique(cpp_obj: *mut c_void);
    fn WCDBRustColumnConstraint_configDefaultValue(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );

    fn WCDBRustColumnConstraint_configCollation(cpp_obj: *mut c_void, collation: *const c_char);

    fn WCDBRustColumnConstraint_configUnIndex(cpp_obj: *mut c_void);
}

pub struct ColumnConstraint {
    identifier: Identifier,
}

impl CppObjectTrait for ColumnConstraint {
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

impl CppObjectConvertibleTrait for ColumnConstraint {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for ColumnConstraint {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for ColumnConstraint {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl ColumnConstraint {
    pub fn new(column_name_opt: Option<&str>) -> Self {
        let cpp_obj = match column_name_opt {
            Some(column_name) => {
                let c_name = column_name.to_cstring();
                unsafe { WCDBRustColumnConstraint_create(c_name.as_ptr()) }
            }
            None => unsafe { WCDBRustColumnConstraint_create(std::ptr::null_mut()) },
        };
        Self {
            identifier: Identifier::new(CPPType::ColumnConstraint, Some(cpp_obj)),
        }
    }

    pub fn primary_key(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configPrimaryKey(self.get_cpp_obj());
        }
        self
    }

    pub fn auto_increment(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configAutoIncrement(self.get_cpp_obj());
        }
        self
    }

    pub fn not_null(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configNotNull(self.get_cpp_obj());
        }
        self
    }

    pub fn conflict(&self, action: ConflictAction) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configConflictAction(self.get_cpp_obj(), action as c_int)
        }
        self
    }

    pub fn unique(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configUnique(self.get_cpp_obj());
        }
        self
    }

    pub fn default_to<'a, V>(&self, value: V) -> &Self
    where
        V: Into<ExpressionConvertibleParam<'a>>,
    {
        let value = value.into();
        let (cpp_type, int_value, double_value, string_value) = value.get_params();
        unsafe {
            WCDBRustColumnConstraint_configDefaultValue(
                self.get_cpp_obj(),
                cpp_type as c_int,
                int_value,
                double_value,
                string_value,
            );
        }
        self
    }

    fn inner_default_to(
        &self,
        cpp_type: CPPType,
        int_value: i64,
        double_value: f64,
        string_value: *const c_char,
    ) {
        unsafe {
            WCDBRustColumnConstraint_configDefaultValue(
                self.get_cpp_obj(),
                cpp_type as i32,
                int_value as *mut c_void,
                double_value as c_double,
                string_value,
            );
        }
    }

    pub fn collate(&self, collation: &str) -> &Self {
        let cstr = collation.to_cstring();
        unsafe { WCDBRustColumnConstraint_configCollation(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }

    pub fn un_index(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configUnIndex(self.get_cpp_obj());
        }
        self
    }
}
