use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::ExpressionOperable;

/// support:
/// ```text
/// &ExpressionConvertibleTrait     Option<&ExpressionConvertibleTrait>
/// &ExpressionOperable             Option<&ExpressionOperable>
/// &Expression                     Option<&Expression>
/// &Column                         Option<&Column>
/// ```
pub enum ExpressionRef<'a> {
    ExpressionConvertible(Option<&'a dyn ExpressionConvertibleTrait>),
}

impl<'a> From<Option<&'a dyn ExpressionConvertibleTrait>> for ExpressionRef<'a> {
    fn from(value: Option<&'a dyn ExpressionConvertibleTrait>) -> Self {
        ExpressionRef::ExpressionConvertible(value)
    }
}

impl<'a> From<&'a dyn ExpressionConvertibleTrait> for ExpressionRef<'a> {
    fn from(value: &'a dyn ExpressionConvertibleTrait) -> Self {
        ExpressionRef::ExpressionConvertible(Some(value))
    }
}

impl<'a> From<Option<&'a ExpressionOperable>> for ExpressionRef<'a> {
    fn from(value: Option<&'a ExpressionOperable>) -> Self {
        let v = value.map(|x| x as &dyn ExpressionConvertibleTrait).into();
        ExpressionRef::ExpressionConvertible(v)
    }
}

impl<'a> From<&'a ExpressionOperable> for ExpressionRef<'a> {
    fn from(value: &'a ExpressionOperable) -> Self {
        let v = value as &dyn ExpressionConvertibleTrait;
        ExpressionRef::ExpressionConvertible(Some(v))
    }
}

impl<'a> From<Option<&'a Expression>> for ExpressionRef<'a> {
    fn from(value: Option<&'a Expression>) -> Self {
        let v = value.map(|x| x as &dyn ExpressionConvertibleTrait).into();
        ExpressionRef::ExpressionConvertible(v)
    }
}

impl<'a> From<&'a Expression> for ExpressionRef<'a> {
    fn from(value: &'a Expression) -> Self {
        ExpressionRef::ExpressionConvertible(Some(value))
    }
}

impl<'a> From<Option<&'a Column>> for ExpressionRef<'a> {
    fn from(value: Option<&'a Column>) -> Self {
        value.map(|x| x as &dyn ExpressionConvertibleTrait).into()
    }
}

impl<'a> From<&'a Column> for ExpressionRef<'a> {
    fn from(value: &'a Column) -> Self {
        ExpressionRef::ExpressionConvertible(Some(value))
    }
}
