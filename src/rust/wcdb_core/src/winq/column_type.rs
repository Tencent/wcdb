#[derive(PartialEq)]
pub enum ColumnType {
    Null = 0,
    Integer = 1,
    Float = 2,
    Text = 3,
    BLOB = 4,
}
