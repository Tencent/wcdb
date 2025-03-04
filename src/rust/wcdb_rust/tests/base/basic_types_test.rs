use std::any::TypeId;
use wcdb_core::base::basic_types::WCDBBasicTypes;

struct BasicTypesTest {}

impl BasicTypesTest {
    pub fn is_integer<T>(value: T) -> bool
    where
        T: WCDBBasicTypes,
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

    pub fn is_double<T>(value: T) -> bool
    where
        T: WCDBBasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<f32>() || type_id == TypeId::of::<f64>() {
            return true;
        }
        false
    }

    pub fn is_bool<T>(value: T) -> bool
    where
        T: WCDBBasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<bool>() {
            return true;
        }
        false
    }

    pub fn is_string<T>(value: T) -> bool
    where
        T: WCDBBasicTypes,
    {
        let type_id = TypeId::of::<T>();
        if type_id == TypeId::of::<String>() || type_id == TypeId::of::<&str>() {
            return true;
        }
        false
    }

    pub fn get_i64<T: WCDBBasicTypes>(value: T) -> i64 {
        value.get_i64()
    }

    pub fn get_f64<T: WCDBBasicTypes>(value: T) -> f64 {
        value.get_f64()
    }

    pub fn get_bool<T: WCDBBasicTypes>(value: T) -> bool {
        value.get_bool()
    }

    pub fn get_string<T: WCDBBasicTypes>(value: T) -> String {
        value.get_string()
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

    #[test]
    pub fn test_value() {
        assert_eq!(BasicTypesTest::get_i64(1i8), 1i64);
        assert_eq!(BasicTypesTest::get_i64(i8::MAX), i8::MAX as i64);
        assert_eq!(BasicTypesTest::get_i64(i8::MIN), i8::MIN as i64);

        assert_eq!(BasicTypesTest::get_i64(1i16), 1i64);
        assert_eq!(BasicTypesTest::get_i64(i16::MAX), i16::MAX as i64);
        assert_eq!(BasicTypesTest::get_i64(i16::MIN), i16::MIN as i64);

        assert_eq!(BasicTypesTest::get_i64(1i32), 1i64);
        assert_eq!(BasicTypesTest::get_i64(i32::MAX), i32::MAX as i64);
        assert_eq!(BasicTypesTest::get_i64(i32::MIN), i32::MIN as i64);

        assert_eq!(BasicTypesTest::get_i64(1i64), 1i64);
        assert_eq!(BasicTypesTest::get_i64(i64::MAX), i64::MAX);
        assert_eq!(BasicTypesTest::get_i64(i64::MIN), i64::MIN);

        assert_eq!(BasicTypesTest::get_bool(true), true);
        assert_eq!(BasicTypesTest::get_bool(false), false);

        assert_eq!(BasicTypesTest::get_string("2134"), "2134".to_string());
        assert_eq!(
            BasicTypesTest::get_string("2134".to_string()),
            "2134".to_string()
        );

        assert_eq!(BasicTypesTest::get_f64(1.0f32), 1.0f64);
        assert_eq!(BasicTypesTest::get_f64(f32::MAX), f32::MAX as f64);
        assert_eq!(BasicTypesTest::get_f64(f32::MIN), f32::MIN as f64);

        assert_eq!(BasicTypesTest::get_f64(1.0f64), 1.0f64);
        assert_eq!(BasicTypesTest::get_f64(f64::MAX), f64::MAX);
        assert_eq!(BasicTypesTest::get_f64(f64::MIN), f64::MIN);
    }
}
