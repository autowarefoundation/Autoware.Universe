# -----------------------------------------------------------------------------
# CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-16 Bradley M. Bell
#
# CppAD is distributed under the terms of the
#              Eclipse Public License Version 2.0.
#
# This Source Code may also be made available under the following
# Secondary License when the conditions for such availability set forth
# in the Eclipse Public License, Version 2.0 are satisfied:
#       GNU General Public License, Version 2.0 or later.
# -----------------------------------------------------------------------------
# =============================================================================
# print_variable(variable)
#
# variable: (in)
# The variable name and value is printed
#
MACRO(print_variable variable)
    MESSAGE(STATUS "${variable} = ${${variable}}" )
ENDMACRO(print_variable)
