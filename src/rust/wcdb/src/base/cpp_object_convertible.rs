use crate::base::cpp_object::CppObject;

pub trait CppObjectConvertibleTrait {
    fn as_cpp_object(&self) -> &CppObject;
}