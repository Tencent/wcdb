use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::identifier::Identifier;

pub trait IdentifierConvertibleTrait: CppObjectConvertibleTrait {
    fn as_identifier(&self) -> &Identifier;
}
