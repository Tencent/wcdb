/// support : i8、i16、i32、i64
pub trait BasicIntegerTypes: 'static {}
impl BasicIntegerTypes for i8 {}
impl BasicIntegerTypes for i16 {}
impl BasicIntegerTypes for i32 {}
impl BasicIntegerTypes for i64 {}

/// support : f32、f64
pub trait BasicDoubleTypes: 'static {}
impl BasicDoubleTypes for f32 {}
impl BasicDoubleTypes for f64 {}

/// support : i8、i16、i32、i64、f32、f64
pub trait BasicNumberTypes: 'static {}
impl BasicNumberTypes for i8 {}
impl BasicNumberTypes for i16 {}
impl BasicNumberTypes for i32 {}
impl BasicNumberTypes for i64 {}
impl BasicNumberTypes for f32 {}
impl BasicNumberTypes for f64 {}

/// support : bool
pub trait BasicBoolTypes: 'static {}
impl BasicBoolTypes for bool {}

/// support : String、&str
pub trait BasicStringTypes: 'static {}
impl BasicBoolTypes for String {}
impl BasicBoolTypes for &'static str {}

/// support : i8、i16、i32、i64、f32、f64、bool、String、&str
pub trait BasicTypes: 'static {}
impl BasicTypes for i8 {}
impl BasicTypes for i16 {}
impl BasicTypes for i32 {}
impl BasicTypes for i64 {}
impl BasicTypes for f32 {}
impl BasicTypes for f64 {}
impl BasicTypes for bool {}
impl BasicTypes for String {}
impl BasicTypes for &'static str {}
