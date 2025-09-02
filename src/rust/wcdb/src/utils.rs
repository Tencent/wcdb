use std::borrow::Cow;
use std::ffi::{c_char, CStr, CString};

pub(crate) trait ToCow {
    fn to_cow(&self) -> Cow<str>;
}

impl ToCow for *const c_char {
    /// 将 C 类型字符串指针，转换成 Cow<str> 类型，分三种情况
    /// 1. 如 C 类型字符串指针为 NULL，则转成的 Cow<str> 为 Cow::Borrow 类型，实际指向一个空字符串地址
    /// 2. 如 C 类型字符串指针不为 NULL，且不包含非法 UTF-8 字符，则转成的 Cow<str> 为 Cow::Borrow 类型，实际为 C 字符串的内存切片
    /// 3. 如 C 类型字符串指针不为 NULL，但包含非法 UTF-8 字符，则将非法字符转成 �，并拷贝构建一个 Cow<str> 为 Cow::Owned 的类型
    /// 注：原 *const c_char 指针仍需要手动释放
    fn to_cow(&self) -> Cow<str> {
        if *self == std::ptr::null() {
            return Cow::Borrowed("");
        }
        unsafe { CStr::from_ptr(*self).to_string_lossy() }
    }
}

pub(crate) trait ToCString {
    fn to_cstring(&self) -> CString;
}

impl ToCString for &str {
    /// 根据 &str 创建新的 CString 对象，返回 *const c_char 指针和所有权
    fn to_cstring(&self) -> CString {
        CString::new(*self).unwrap_or_default()
    }
}

impl ToCString for String {
    /// 根据 String 创建新的 CString 对象，返回 *const c_char 指针和所有权
    fn to_cstring(&self) -> CString {
        self.as_str().to_cstring()
    }
}

impl<'a> ToCString for Cow<'a, str> {
    fn to_cstring(&self) -> CString {
        self.as_ref().to_cstring()
    }
}
