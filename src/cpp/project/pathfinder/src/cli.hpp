#ifndef HEADER_SCRIPTS_CXX_PAF_CLI_
#define HEADER_SCRIPTS_CXX_PAF_CLI_

namespace paf {
    class cli {
    public:
        cli(void) = delete;
        cli(int argc, char** argv);
        [[noreturn]] void run(void);
    };
} // namespace paf

#endif /* ifndef HEADER_SCRIPTS_CXX_PAF_CLI_ */
