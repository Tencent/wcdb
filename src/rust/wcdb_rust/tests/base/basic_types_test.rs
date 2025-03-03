use std::any::TypeId;
use wcdb_core::base::basic_types::{BasicIntegerTypes, BasicTypes};

struct BasicTypesTest {}

impl BasicTypesTest {
    pub fn is_integer<T>(value: T) -> bool
    where
        T: BasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<i8>()
            || type_id == TypeId::of::<i16>()
            || type_id == TypeId::of::<i32>()
            || type_id == TypeId::of::<i64>()
        {
            return true;
        }
        false
    }

    pub fn is_bool<T>(value: T) -> bool
    where
        T: BasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<bool>() {
            return true;
        }
        false
    }

    pub fn is_string<T>(value: T) -> bool
    where
        T: BasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<String>() || type_id == TypeId::of::<&str>() {
            return true;
        }
        false
    }

    pub fn is_double<T>(value: T) -> bool
    where
        T: BasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<f32>() || type_id == TypeId::of::<f64>() {
            return true;
        }
        false
    }
}

#[cfg(test)]
pub mod basic_types_test {
    use crate::base::basic_types_test::BasicTypesTest;

    #[test]
    pub fn test() {
        assert!(BasicTypesTest::is_integer(1i8));
        assert!(BasicTypesTest::is_integer(i8::MAX));
        assert!(BasicTypesTest::is_integer(i8::MIN));

        assert!(BasicTypesTest::is_integer(1i16));
        assert!(BasicTypesTest::is_integer(i16::MAX));
        assert!(BasicTypesTest::is_integer(i16::MIN));

        assert!(BasicTypesTest::is_integer(1i32));
        assert!(BasicTypesTest::is_integer(i32::MAX));
        assert!(BasicTypesTest::is_integer(i32::MIN));

        assert!(BasicTypesTest::is_integer(1i64));
        assert!(BasicTypesTest::is_integer(i64::MAX));
        assert!(BasicTypesTest::is_integer(i64::MIN));

        assert!(BasicTypesTest::is_bool(true));
        assert!(BasicTypesTest::is_bool(false));

        assert!(BasicTypesTest::is_string("2134"));
        assert!(BasicTypesTest::is_string("2134".to_string()));

        assert!(BasicTypesTest::is_double(1.0f32));
        assert!(BasicTypesTest::is_double(f32::MAX));
        assert!(BasicTypesTest::is_double(f32::MIN));

        assert!(BasicTypesTest::is_double(1.0f64));
        assert!(BasicTypesTest::is_double(f64::MAX));
        assert!(BasicTypesTest::is_double(f64::MIN));
    }
}
