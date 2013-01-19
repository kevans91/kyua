// Copyright 2011 Google Inc.
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

#include "engine/context.hpp"

#include "utils/env.hpp"
#include "utils/fs/operations.hpp"

namespace fs = utils::fs;


/// Internal implementation of a context.
struct engine::context::impl {
    /// The current working directory.
    fs::path _cwd;

    /// The environment variables.
    std::map< std::string, std::string > _env;

    /// Constructor.
    ///
    /// \param cwd_ The current working directory.
    /// \param env_ The environment variables.
    impl(const fs::path& cwd_,
         const std::map< std::string, std::string >& env_) :
        _cwd(cwd_),
        _env(env_)
    {
    }

    /// Equality comparator.
    ///
    /// \param other The object to compare to.
    ///
    /// \return True if the two objects are equal; false otherwise.
    bool
    operator==(const impl& other) const
    {
        return _cwd == other._cwd && _env == other._env;
    }
};


/// Constructs a new context.
///
/// \param cwd_ The current working directory.
/// \param env_ The environment variables.
engine::context::context(const fs::path& cwd_,
                         const std::map< std::string, std::string >& env_) :
    _pimpl(new impl(cwd_, env_))
{
}


/// Destructor.
engine::context::~context(void)
{
}


/// Constructs a new context based on the current environment.
engine::context
engine::context::current(void)
{
    return context(fs::current_path(), utils::getallenv());
}


/// Returns the current working directory of the context.
///
/// \return A path.
const fs::path&
engine::context::cwd(void) const
{
    return _pimpl->_cwd;
}


/// Returns the environment variables of the context.
///
/// \return A variable name to variable value mapping.
const std::map< std::string, std::string >&
engine::context::env(void) const
{
    return _pimpl->_env;
}


/// Equality comparator.
///
/// \param other The other object to compare this one to.
///
/// \return True if this object and other are equal; false otherwise.
bool
engine::context::operator==(const context& other) const
{
    return *_pimpl == *other._pimpl;
}


/// Inequality comparator.
///
/// \param other The other object to compare this one to.
///
/// \return True if this object and other are different; false otherwise.
bool
engine::context::operator!=(const context& other) const
{
    return !(*this == other);
}
