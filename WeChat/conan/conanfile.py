from conans import ConanFile, tools


class WcdbiosConan(ConanFile):
    name = "WCDB-ios"
    version = "1.0.4"
    settings = "os", "compiler", "build_type", "arch"
    description = "<Description of Wcdbios here>"
    url = "None"
    license = "None"
    author = "None"
    topics = None

    def package(self):
        self.copy("*")

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
