#pragma once

#define _STR(x) #x
#define STR(x) _STR(x)

#define AXO_TODO(todoMessage) __pragma(message(__FILE__ "("STR(__LINE__)") : TODO: " todoMessage))
