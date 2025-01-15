use crate::base::cpp_object::CppObject;

pub(crate) trait CppObjectConvertibleTrait {
    fn as_cpp_object(&self) -> &CppObject;
}
