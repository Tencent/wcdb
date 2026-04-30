use crate::orm::field::Field;
use crate::winq::column::Column;
use crate::winq::column_type::ColumnType;

pub enum StringColumnDef<'a> {
    String(&'a str, Option<ColumnType>),
    Column(&'a Column, Option<ColumnType>),
}

impl<'a> From<&'a str> for StringColumnDef<'a> {
    fn from(name: &'a str) -> Self {
        StringColumnDef::String(name, None)
    }
}

impl<'a> From<(&'a str, ColumnType)> for StringColumnDef<'a> {
    fn from((name, ty): (&'a str, ColumnType)) -> Self {
        StringColumnDef::String(name, Some(ty))
    }
}

impl<'a> From<(&'a str, Option<ColumnType>)> for StringColumnDef<'a> {
    fn from((name, ty_opt): (&'a str, Option<ColumnType>)) -> Self {
        StringColumnDef::String(name, ty_opt)
    }
}

impl<'a> From<&'a Column> for StringColumnDef<'a> {
    fn from(col: &'a Column) -> Self {
        StringColumnDef::Column(col, None)
    }
}

impl<'a> From<(&'a Column, ColumnType)> for StringColumnDef<'a> {
    fn from((col, ty): (&'a Column, ColumnType)) -> Self {
        StringColumnDef::Column(col, Some(ty))
    }
}

impl<'a> From<(&'a Column, Option<ColumnType>)> for StringColumnDef<'a> {
    fn from((col, ty_opt): (&'a Column, Option<ColumnType>)) -> Self {
        StringColumnDef::Column(col, ty_opt)
    }
}

impl<'a, T> From<&'a Field<T>> for StringColumnDef<'a> {
    fn from(field: &'a Field<T>) -> Self {
        StringColumnDef::Column(field.get_column(), None)
    }
}

impl<'a, T> From<(&'a Field<T>, ColumnType)> for StringColumnDef<'a> {
    fn from((field, ty): (&'a Field<T>, ColumnType)) -> Self {
        StringColumnDef::Column(field.get_column(), Some(ty))
    }
}

impl<'a, T> From<(&'a Field<T>, Option<ColumnType>)> for StringColumnDef<'a> {
    fn from((field, ty_opt): (&'a Field<T>, Option<ColumnType>)) -> Self {
        StringColumnDef::Column(field.get_column(), ty_opt)
    }
}
