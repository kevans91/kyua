// Copyright 2010 Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <atf-c++.hpp>

#include "engine/test_case.hpp"
#include "engine/test_program.hpp"
#include "engine/test_result.hpp"
#include "engine/user_files/config.hpp"
#include "utils/optional.ipp"
#include "utils/sanity.hpp"
#include "utils/test_utils.hpp"

namespace fs = utils::fs;
namespace user_files = engine::user_files;

using utils::optional;


namespace {


/// Fake configuration.
static const user_files::config mock_config(
    "mock-architecture", "mock-platform", utils::none,
    user_files::test_suites_map());


/// Records the data passed to the hooks for later validation.
class capture_hooks : public engine::test_case_hooks {
public:
    /// Path to the stdout file of the test case, if received.
    optional< fs::path > stdout_path;

    /// Path to the stderr file of the test case, if received.
    optional< fs::path > stderr_path;

    /// Records the path to the stdout.
    ///
    /// Note that, in normal execution, this file is not readable outside of
    /// this hook because it is generated inside a temporary directory.
    ///
    /// \param file Fake path to the test case's stdout.
    void
    got_stdout(const fs::path& file)
    {
        stdout_path = file;
    }

    /// Records the path to the stderr.
    ///
    /// Note that, in normal execution, this file is not readable outside of
    /// this hook because it is generated inside a temporary directory.
    ///
    /// \param file Fake path to the test case's stderr.
    void
    got_stderr(const fs::path& file)
    {
        stderr_path = file;
    }
};


/// Fake implementation of a test program.
class mock_test_program : public engine::base_test_program {
public:
    /// Constructs a new test program.
    ///
    /// Both the test suite root and the test suite name are fixed and
    /// supposedly unused in this module.
    ///
    /// \param binary_ The name of the test program binary.
    mock_test_program(const fs::path& binary_) :
        base_test_program(binary_, fs::path("unused-root"), "unused-suite-name")
    {
    }


    /// Gets the list of test cases from the test program.
    ///
    /// \return Nothing; this method is not supposed to be called.
    engine::test_cases_vector
    load_test_cases(void) const
    {
        UNREACHABLE;
    }
};


/// Fake implementation of a test case.
class mock_test_case : public engine::base_test_case {
    /// Gets the collection of metadata properties of the test case.
    ///
    /// \return A static collection of properties for testing purposes.
    engine::properties_map
    get_all_properties(void) const
    {
        engine::properties_map properties;
        properties["first"] = "value";
        return properties;
    }

    /// Fakes the execution of a test case.
    ///
    /// \param config The run-time configuration.  Must be mock_config.
    /// \param hooks Hooks to introspect the execution of the test case.
    /// \param unused_stdout_path The file into which to write the stdout.
    /// \param unused_stderr_path The file into which to write the stderr.
    ///
    /// \return A static result for testing purposes.
    engine::test_result
    execute(const user_files::config& config, engine::test_case_hooks& hooks,
            const optional< fs::path >& UTILS_UNUSED_PARAM(stdout_path),
            const optional< fs::path >& UTILS_UNUSED_PARAM(stderr_path)) const
    {
        if (&config != &mock_config)
            throw std::runtime_error("Invalid config object");
        hooks.got_stdout(fs::path("fake-stdout.txt"));
        hooks.got_stderr(fs::path("fake-stderr.txt"));
        return engine::test_result(engine::test_result::skipped,
                                   "A test result");
    }

public:
    /// Constructs a new test case.
    ///
    /// \param test_program_ The test program this test case belongs to.
    /// \param name_ The name of the test case within the test program.
    mock_test_case(const engine::base_test_program& test_program_,
                   const std::string& name_) :
        base_test_case(test_program_, name_)
    {
    }
};


}  // anonymous namespace


ATF_TEST_CASE_WITHOUT_HEAD(base_test_case__ctor_and_getters)
ATF_TEST_CASE_BODY(base_test_case__ctor_and_getters)
{
    const mock_test_program test_program(fs::path("abc"));
    const mock_test_case test_case(test_program, "foo");
    ATF_REQUIRE_EQ(&test_program, &test_case.test_program());
    ATF_REQUIRE_EQ("foo", test_case.name());
}


ATF_TEST_CASE_WITHOUT_HEAD(base_test_case__all_properties__delegate)
ATF_TEST_CASE_BODY(base_test_case__all_properties__delegate)
{
    const mock_test_program test_program(fs::path("foo"));
    const mock_test_case test_case(test_program, "bar");

    engine::properties_map exp_properties;
    exp_properties["first"] = "value";
    ATF_REQUIRE(exp_properties == test_case.all_properties());
}


ATF_TEST_CASE_WITHOUT_HEAD(base_test_case__run__delegate)
ATF_TEST_CASE_BODY(base_test_case__run__delegate)
{
    const mock_test_program test_program(fs::path("foo"));
    const mock_test_case test_case(test_program, "bar");

    capture_hooks hooks;
    ATF_REQUIRE(engine::test_result(engine::test_result::skipped,
                                    "A test result") ==
                test_case.run(mock_config, hooks));
    ATF_REQUIRE(hooks.stdout_path);
    ATF_REQUIRE_EQ(fs::path("fake-stdout.txt"), hooks.stdout_path.get());
    ATF_REQUIRE(hooks.stderr_path);
    ATF_REQUIRE_EQ(fs::path("fake-stderr.txt"), hooks.stderr_path.get());
}


ATF_INIT_TEST_CASES(tcs)
{
    ATF_ADD_TEST_CASE(tcs, base_test_case__ctor_and_getters);
    ATF_ADD_TEST_CASE(tcs, base_test_case__all_properties__delegate);
    ATF_ADD_TEST_CASE(tcs, base_test_case__run__delegate);
}
