use crate::base::basic_types::WCDBBasicTypes;
use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_double, c_int, c_long, c_void, CString};

extern "C" {
    pub fn WCDBRustColumnConstraint_create(name: *const c_char) -> *mut c_void;

    pub fn WCDBRustColumnConstraint_configPrimaryKey(cpp_obj: *mut c_void);

    pub fn WCDBRustColumnConstraint_configAutoIncrement(cpp_obj: *mut c_void);

    pub fn WCDBRustColumnConstraint_configNotNull(cpp_obj: *mut c_void);

    pub fn WCDBRustColumnConstraint_configUnique(cpp_obj: *mut c_void);
    pub fn WCDBRustColumnConstraint_configDefaultValue(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        int_value: c_long,
        double_value: c_double,
        string_value: *const c_char,
    );

    pub fn WCDBRustColumnConstraint_configUnIndex(cpp_obj: *mut c_void);
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

impl IdentifierTrait for ColumnConstraint {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for ColumnConstraint {
    fn get_type() -> i32 {
        CPPType::ColumnConstraint as i32
    }
}

impl ColumnConstraint {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustColumnConstraint_create(std::ptr::null_mut()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_by_column_name(column_name: &str) -> Self {
        let c_name = CString::new(column_name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumnConstraint_create(c_name.as_ptr()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
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

    pub fn unique(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configUnique(self.get_cpp_obj());
        }
        self
    }

    pub fn default_to<T: WCDBBasicTypes>(&self, value: T) -> &Self {
        // let type_id = TypeId::of::<T>();
        // if type_id == TypeId::of::<bool>() {
        // let mut int_value = 1;
        // if value as bool {
        //     int_value = 1;
        // } else {
        //     int_value = 0;
        // }
        // self.inner_default_to(CPPType::Bool, int_value, 0f64, std::ptr::null());
        // } else
        // if type_id == TypeId::of::<i8>()
        //     || type_id == TypeId::of::<u8>()
        //     || type_id == TypeId::of::<i32>()
        //     || type_id == TypeId::of::<i64>()
        // {
        //     self.inner_default_to(CPPType::Int, value as i64, 0f64, std::ptr::null());
        // } else if type_id == TypeId::of::<f32>() || type_id == TypeId::of::<f64>() {
        //     self.inner_default_to(CPPType::Double, 0, value as f64, std::ptr::null());
        // } else if type_id == TypeId::of::<&str>() {
        //     let c_str = (value as &str).to_cstring();
        //     self.inner_default_to(CPPType::Double, 0, 0f64, c_str.as_ptr());
        // }
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
                int_value as c_long,
                double_value as c_double,
                string_value,
            );
        }
    }

    pub fn un_index(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configUnIndex(self.get_cpp_obj());
        }
        self
    }
}
