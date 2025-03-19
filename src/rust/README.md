# WCDB for Rust 项目说明文档

## 编码规范

Rust 语言接口适配以源仓库自带的 Java 接口适配为蓝本进行翻译，遵循以下原则：

1. 如非必要，所有代码提交集中在 src/rust，也就是当前目录下，外面代码尽量保持不变。
2. 核心工作是将 Java 代码翻译成 Rust 语言，同时将 JNI 翻译成 C/C++ 桥接。
3. 目录结构、文件名、函数名、变量名、代码逻辑等，在遵循 Rust 语言风格基础上，仅做必要更名，其他尽量保持一致，以便于对照查看。
4. 由于 Rust 不支持面向对象，原来的继承改为组合，将子类函数以 Trait 方式提供，尽量保持原有调用逻辑和多态性。
5. 由于 Rust 不支持异常，原来的异常处理改为返回 Result 类型，尽量保持原有调用逻辑。
6. 由于 Rust 有严格的所有权归属，对于 Java 的自引用、返回对象归属不明确的情况，可以进行必要的重构。
7. 保持最小可见性，内部尽量使用 pub(crate/super) 修饰符，避免暴露不必要的接口、内部变量。
8. 由于接口众多，所有 Rust 函数书写顺序需要跟 Java 保持一致，方便对照查看。
9. 定义 struct 时，将 Java 对象的父类，以变量方式放在第一行，Trait 实现按照 系统 -> 先祖类 -> 父类 -> 自身实现 的顺序依次进行，如：
    ```
    pub struct Database {
        handle_orm_operation: HandleORMOperation, // 第一行
        // ...
    }

    // 系统特征
    impl Display for Database { ... }

    // 先祖类特征
    impl CppObjectTrait for Database { ... }

    // 祖类特征
    impl HandleOperationTrait for Database { ... }

    // 父类特征
    impl HandleORMOperationTrait for Database { ... }

    // 自身实现
    impl Database { ... }
    ```
10. 依托 Demo/TestCase 逐步翻译，避免没有调用的逻辑实现出现，一来确保代码翻译正确，二来有不少接口调用频率不高，可以推迟实现或不实现，将有限精力用在核心接口上。
11. 提交要求满足 `cargo fmt -- --check` 检查。除此以外，空行需要跟现有风格对齐，如函数之间有空行，逻辑块与块之间有空行，勿多勿少。
12. 其余未详述细节，参照现有代码规范编写即可。
13. 根目录下执行 `cargo bench` 开始执行性能测试，输出内容在控制台和 `wcdb_rust/src/rust/target/criterion` 目录中。

## CI 检查点
1. [Git Commit Lint](https://github.com/conventional-changelog/commitlint) （另附：结尾标点符号必须是 [.!?] 之一）
2. [中英文排版规范](https://github.com/huacnlee/autocorrect)
3. [clang-format](cpp/.clang-format)
4. [cargo fmt](https://github.com/rust-lang/rustfmt)
5. Rust 集成测试用例
6. Rust 项目的单元测试默认采用单线程执行，命令为：cargo test -p wcdb_rust -- --test-threads=1。原因有两方面：一是 Java 代码的单元测试通常是单线程执行；二是某些测试场景依赖数据库的打开/关闭状态等，导致无法并行执行。
7. Rust 展开宏生成的文件，命令为：cargo expand -p wcdb_rust --test lib -- > expanded.rs